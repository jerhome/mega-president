// Mega Président — écran de jeu : tableau de bord, lois, 49.3, télévision,
// dissolution, Assemblée (tâche 6.1, V1).
// Dessin uniquement via src/ui ; la simulation est modifiée par game.h.

#include "screens/scr_map.h"
#include "audio/music.h"
#include "screens/scr_help.h"
#include "screens/scr_options.h"
#include "audio/sfx.h"
#include <genesis.h>
#include <string.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_laws.h"
#include "content/content_world.h"
#include "content/content_texts.h"
#include "screens/evt_art.h"
#include "ui/ui_art.h"
#include "ui/ui_hemi.h"
#include "screens/scr_txt.h"
#include "screens/scr_kit.h"
#include "util/strbuf.h"

// Instantané des indicateurs pris au moment de « Fin du trimestre » : lu par
// scr_report.c pour afficher ce qui a bougé.
// Ordre : croissance, chômage, dette, 6 piliers, tension.
s16 g_report_snap[3 + PIL_TOTAL + 1];

// Tour (+1) où le joueur a déjà vu l'écran du budget : renseigné par scr_budget.c.
u8 g_budget_seen;

typedef enum { M_MENU, M_LAWS, M_VOTE, M_ASSEMBLY, M_CONFIRM, M_RESULT } Mode;
typedef enum { C_FORCE, C_DISSOLVE, C_QUIT } ConfirmKind;
typedef enum { R_TV, R_FORCE, R_DISSOLVE } ResultKind;
// Actions du menu principal
typedef enum { A_LAW, A_FORCE, A_TV, A_ASSEMBLY, A_MAP, A_REFERENDUM, A_RESHUFFLE, A_DISSOLVE, A_BUDGET,
               A_END, A_OPTIONS, A_QUIT } Action;

#define MENU_MAX 12   // toutes les actions possibles (A_LAW .. A_QUIT)
_Static_assert(A_QUIT + 1 <= MENU_MAX, "MENU_MAX : une entrée par action au plus");

static Mode mode;
static Mode prev_mode;
static bool mode_dirty;     // redessin complet du mode
static u8 detail_dirty;     // détail de la loi (mode lois)
static u8 forcing;          // 1 : la liste des lois sert au 49.3
static UiList menu_list;
static UiList law_list;
static VoteResult last_vote;
static u8 last_law;
static u8 last_adopted;
static ConfirmKind confirm_kind;
static ResultKind res_kind;
static u8 res_tv_ok;
static s16 res_pop_before, res_pop_after;

static u8 menu_act[MENU_MAX];
static const char *menu_items[MENU_MAX];
static u8 menu_n;
static u8 menu_sel, menu_top;
static bool menu_to_options;   // retour de l'écran Options : barre sur « Options »
static u8 asm_sel;           // groupe sélectionné dans la vue Assemblée
static u32 menu_dis;           // bit i = entrée i grisée
static char tv_label[UI_LINE_BUF];
static char ref_label[UI_LINE_BUF];
static char rsh_label[UI_LINE_BUF];
static char confirm_text[160];

// --- Utilitaires -----------------------------------------------------------------

// « N trimestre(s) » (20 tours) ou « N mois » (60 tours : délai x 3).
static void sb_delay(StrBuf *sb, u8 delay_q)
{
    if (game_is_monthly()) {
        sb_num(sb, delay_q * 3, 0);
        sb_str(sb, " mois");
    } else {
        sb_num(sb, delay_q, 0);
        sb_str(sb, delay_q > 1 ? " trimestres" : " trimestre");
    }
}

// Délai restant d'une loi votée, dans l'unité du mode de jeu (tours = mois ou trimestres).
static void sb_turns(StrBuf *sb, u8 turns)
{
    sb_num(sb, turns, 0);
    if (game_is_monthly()) sb_str(sb, " mois");
    else sb_str(sb, turns > 1 ? " trimestres" : " trimestre");
}

static bool quiet_back;   // redessin net (retour par B ou Annuler) plutôt que fondu doux

static void set_mode(Mode m)
{
    // Cachet musical : thème de l'Assemblée pendant les votes et sa consultation,
    // retour au thème de l'année dans les menus (la musique de l'écran vient de screen.c).
    if (m == M_VOTE || m == M_ASSEMBLY)
        music_play(MUS_ASSEMBLY);
    else if (m == M_MENU || m == M_LAWS)
        music_play_year(sim_year(&g_game.country));
    mode = m;
    mode_dirty = TRUE;
}

// Retour (B, Annuler, refus) : même chose que set_mode, avec un redessin net.
static void set_mode_quiet(Mode m)
{
    set_mode(m);
    quiet_back = TRUE;
}

// Début d'une page : redessin net pour un retour, fondu doux pour un changement de pièce.
static void page_begin(const char *title)
{
    if (quiet_back) {
        kit_page(title);
    } else {
        art_reset();
        kit_header(title);
    }
    quiet_back = FALSE;
}

// --- Tableau de bord -----------------------------------------------------------------

static u8 any_law_forceable(void)
{
    for (u16 i = 0; i < LOI_COUNT; i++)
        if (sim_can_force_law(&g_game.country, (u8)i) == LAW_OK) return 1;
    return 0;
}

static u8 any_law_referendum(void)
{
    for (u16 i = 0; i < LOI_COUNT; i++)
        if (sim_referendum_check(&g_game.country, (u8)i) == RFD_OK) return 1;
    return 0;
}

static u8 any_reshuffle(void)
{
    for (u8 p = 0; p < PM_COUNT; p++)
        if (sim_reshuffle_check(&g_game.country, p) == RSH_OK) return 1;
    return 0;
}

static u8 budget_blocking(void)
{
    const Country *c = &g_game.country;
    return (u8)(sim_budget_due(c) && g_budget_seen != c->turn + 1);
}

// Ajoute une entrée au menu ; au-delà de MENU_MAX (I4) elle est ignorée.
static void menu_add(Action act, const char *label, bool enabled)
{
    if (menu_n >= MENU_MAX) return;
    menu_act[menu_n] = act;
    menu_items[menu_n] = label;
    if (!enabled) menu_dis |= (u32)1 << menu_n;
    menu_n++;
}

// « Libellé (N CP) »
static const char *cp_label(char *dst, u16 cap, const char *label, u8 cp)
{
    StrBuf sb;
    sb_init(&sb, dst, cap);
    sb_str(&sb, label);
    sb_str(&sb, " (");
    sb_num(&sb, cp, 0);
    sb_str(&sb, " CP)");
    return dst;
}

static void menu_build(void)
{
    const Country *c = &g_game.country;
    StrBuf sb;

    menu_n = 0;
    menu_dis = 0;
    menu_add(A_LAW, "Proposer une loi", TRUE);
    menu_add(A_FORCE, "Forcer une loi (49.3)", any_law_forceable());
    sb_init(&sb, tv_label, sizeof tv_label);
    sb_str(&sb, "Intervention télévisée (");
    sb_num(&sb, TV_CP, 0);
    sb_str(&sb, " CP, ");
    sb_num(&sb, sim_tv_flop_chance(c), 0);
    sb_str(&sb, " %)");
    menu_add(A_TV, tv_label, sim_tv_available(c));
    menu_add(A_ASSEMBLY, "Voir l’Assemblée", TRUE);
    menu_add(A_MAP, "Carte des régions", TRUE);
    menu_add(A_REFERENDUM, cp_label(ref_label, sizeof ref_label, txt[TXT_ACT_REFERENDUM], REFERENDUM_CP),
             any_law_referendum());
    menu_add(A_RESHUFFLE, cp_label(rsh_label, sizeof rsh_label, txt[TXT_ACT_REMANIEMENT], RESHUFFLE_CP),
             any_reshuffle());
    if (sim_can_dissolve(c))
        menu_add(A_DISSOLVE, "Dissoudre l’Assemblée", TRUE);
    if (sim_cohabitation(c))
        menu_add(A_BUDGET, txt[TXT_BUD_TITRE_GOUV], TRUE);
    else if (sim_budget_due(c) && !c->budget_done)
        menu_add(A_BUDGET, "Présenter le budget", TRUE);
    else if (c->budget_done && sim_budget_amended_line(c))
        menu_add(A_BUDGET, "Budget amendé : lecture définitive", TRUE);
    if (game_is_monthly())
        menu_add(A_END, budget_blocking() ? "Fin du mois (budget d’abord)" : "Fin du mois", TRUE);
    else
        menu_add(A_END, budget_blocking() ? "Fin du trimestre (budget d’abord)" : "Fin du trimestre", TRUE);
    menu_add(A_OPTIONS, "Options", TRUE);
    menu_add(A_QUIT, "Menu principal (partie sauvegardée)", TRUE);
    if (menu_to_options) {
        menu_to_options = FALSE;
        for (u8 i = 0; i < menu_n; i++)
            if (menu_act[i] == A_OPTIONS)
                menu_sel = i;
        menu_top = 0;
    }
    ui_list_init(&menu_list, 1, 22, 38, 5, menu_items, menu_n);
    menu_list.disabled = menu_dis;
    if (menu_sel < menu_n && !(menu_dis & ((u32)1 << menu_sel))) {
        menu_list.sel = menu_sel;
        menu_list.top = menu_top < menu_n ? menu_top : 0;
        if (menu_list.sel < menu_list.top || menu_list.sel >= menu_list.top + menu_list.rows)
            menu_list.top = menu_list.sel >= menu_list.rows ? menu_list.sel - menu_list.rows + 1 : 0;
    }
    ui_list_select_first_enabled(&menu_list);
}

// Tableau de bord (charte V3) :
//  0-2   bandeau : date du tour, capital politique
//  3-20  fenêtre des indicateurs ; à droite, médaillon du Premier ministre
//        (x 29-38, y 4-15 : gueule 10 x 12 tuiles = 80 x 96 px, PAL2)
//  21-27 fenêtre des actions (5 lignes visibles)
#define DASH_X_END 28        // fin des lignes d'indicateurs (colonne exclue ; 28 = vide)
#define MED_X 29             // portrait 10 x 12 (gueule V3, 80 x 96 px)
#define MED_Y 4
#define MED_W 10
#define MED_H 12

// Cinq électorats sur deux lignes (3 + 2, cellules de 8 colonnes : « Pop 52 » + flèche de
// tendance). Libellés atténués, valeurs entières, rouge si l'électorat est sous les 50 %.
// La seconde ligne rappelle le détail (touche C).
static void draw_electorates(u16 y)
{
    char b[14];
    const Country *c = &g_game.country;
    for (u16 e = 0; e < ELEC_COUNT; e++) {
        u16 x = 3 + (e % 3) * 8;
        u16 row = y + e / 3;
        s16 o = c->opinion[e];
        x += ui_text_draw(x, row, elec_txt((u8)e, ET_ABR), UI_STYLE_DIM) + 1;
        ui_format_number(b, (o + 5) / 10, 0);
        x += ui_text_draw(x, row, b, o >= 500 ? UI_STYLE_TEXT : UI_STYLE_ALERT);
        elec_trend_icon(x, row, elec_trend(c, (u8)e));
    }
    ui_icon_draw(19, y + 1, UI_ICON_BTN_C);
    ui_text_draw(21, y + 1, "détail", UI_STYLE_DIM);
}

// Colonne du médaillon : portrait du PM (ou du PM de cohabitation), nom, majorité, CP.
// Humeur du Premier ministre : le sien suit la popularité et la grogne ; celui de
// cohabitation se réjouit des déboires du président.
static Expr pm_expr(void)
{
    const Country *c = &g_game.country;
    s16 pop = sim_popularity(c);
    if (sim_cohabitation(c))
        return pop < 400 ? EXPR_CONTENT : (pop >= 550 ? EXPR_FURIEUX : EXPR_NEUTRE);
    if (c->gauges[JAUGE_DEFIANCE] >= 60 || c->gauges[JAUGE_TENSION] >= 70)
        return EXPR_FURIEUX;
    if (pop >= 550)
        return EXPR_CONTENT;
    if (pop < 400 || c->gauges[JAUGE_SCANDALE] >= 50)
        return EXPR_GENE;
    return EXPR_NEUTRE;
}

static void draw_pm_column(void)
{
    const Country *c = &g_game.country;
    u8 cohab = sim_cohabitation(c);
    GueuleId id = cohab ? GUEULE_COHAB(sim_cohab_camp(c)) : GUEULE_PM(c->pm);
    const char *name = cohab ? cohab_pm_name(sim_cohab_camp(c)) : pm_name(c->pm);
    SB_LINE(sb);
    u8 n = sim_cp_turns_to_next(c);

    ui_medallion(MED_X, MED_Y, MED_W, MED_H);
    ui_gueule_draw(id, pm_expr(), MED_X, MED_Y, PAL2);
    ui_text_center(MED_X, MED_Y + MED_H, MED_W, surname(name), cohab ? UI_STYLE_ALERT : UI_STYLE_HIGHLIGHT);
    if (cohab)
        ui_text_center(MED_X, MED_Y + MED_H + 1, MED_W, "Cohab.", UI_STYLE_ALERT);
    else
        ui_text_center(MED_X, MED_Y + MED_H + 1, MED_W, sim_has_majority(c) ? "Maj. abs." : "Maj. rel.",
                       sim_has_majority(c) ? UI_STYLE_TEXT : UI_STYLE_ALERT);
    sb_str(&sb, "+");
    sb_num(&sb, sim_cp_gain(c), 0);
    sb_str(&sb, " CP");
    ui_text_center(MED_X, MED_Y + MED_H + 2, MED_W, sb.buf, UI_STYLE_HIGHLIGHT);
    sb_init(&sb, sb_mem, SB_LINE_SIZE);
    if (n <= 1) {
        sb_str(&sb, "par tour");
    } else {
        sb_str(&sb, "en ");
        sb_num(&sb, n, 0);
        sb_str(&sb, " mois");
    }
    ui_text_center(MED_X, MED_Y + MED_H + 3, MED_W, sb.buf, UI_STYLE_DIM);
}

static void draw_menu_full(void)
{
    static const UiIcon pillar_icons[PIL_TOTAL] = {
        UI_ICON_SOCIAL, UI_ICON_ECONOMY, UI_ICON_TECHNOLOGY,
        UI_ICON_ECOLOGY, UI_ICON_SECURITY, UI_ICON_INTERNATIONAL
    };
    const Country *c = &g_game.country;
    s16 pop = sim_popularity(c);
    s16 sc = c->gauges[JAUGE_SCANDALE], df = c->gauges[JAUGE_DEFIANCE], tn = c->gauges[JAUGE_TENSION];

    page_begin(NULL);
    ui_window(0, 3, 40, 18);

    ui_stat_row(4, &(UiStatRow){ UI_ICON_POPULARITY, "Popularité", pop, 1, " %", pop, 1000,
                UI_GAUGE_GOLD, pop >= 500 ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT, DASH_X_END });
    draw_electorates(5);
    ui_stat_row(7, &(UiStatRow){ UI_ICON_GROWTH, "Croissance", c->growth, 2, " %",
                c->growth - GROWTH_MIN, GROWTH_MAX - GROWTH_MIN,
                c->growth >= 0 ? UI_GAUGE_POSITIVE : UI_GAUGE_NEGATIVE,
                c->growth >= 0 ? UI_STYLE_TEXT : UI_STYLE_ALERT, DASH_X_END });
    ui_stat_row(8, &(UiStatRow){ UI_ICON_UNEMPLOYMENT, "Chômage", c->unemployment, 2, " %",
                c->unemployment, UNEMP_MAX, UI_GAUGE_NEUTRAL, UI_STYLE_TEXT, DASH_X_END });
    ui_stat_row(9, &(UiStatRow){ UI_ICON_DEBT, "Dette", c->debt, 1, " %", c->debt, DEBT_MAX,
                UI_GAUGE_GOLD, UI_STYLE_TEXT, DASH_X_END });
    ui_separator(1, 10, DASH_X_END - 1);
    for (u16 i = 0; i < PIL_TOTAL; i++)
        ui_stat_row(11 + i, &(UiStatRow){ pillar_icons[i], pillar_name((u8)i, TRUE), c->pillars[i], 1, "",
                    c->pillars[i], PILLAR_MAX, UI_GAUGE_POSITIVE,
                    c->pillars[i] < 250 ? UI_STYLE_ALERT : UI_STYLE_TEXT, DASH_X_END });
    ui_stat_row(17, &(UiStatRow){ UI_ICON_TENSION, "Tension", tn, 0, " %", tn, GAUGE_MAX,
                UI_GAUGE_NEGATIVE, tn >= 50 ? UI_STYLE_ALERT : UI_STYLE_TEXT, DASH_X_END });
    ui_stat_row(18, &(UiStatRow){ UI_ICON_SCANDAL, "Scandale", sc, 0, " %", sc, GAUGE_MAX,
                UI_GAUGE_NEGATIVE, sc >= 50 ? UI_STYLE_ALERT : UI_STYLE_TEXT, DASH_X_END });
    ui_stat_row(19, &(UiStatRow){ UI_ICON_DEFIANCE, "Défiance", df, 0, " %", df, GAUGE_MAX,
                UI_GAUGE_NEGATIVE, df >= 50 ? UI_STYLE_ALERT : UI_STYLE_TEXT, DASH_X_END });
    draw_pm_column();

    ui_window(0, 21, 40, 7);
    menu_build();
    ui_list_draw(&menu_list);
}

static void start_confirm(ConfirmKind k, const char *title, const char *text,
                          const char *yes, const char *no)
{
    confirm_kind = k;
    prev_mode = mode;
    ui_confirm_open(title, text, yes, no, FALSE);
    mode = M_CONFIRM;
}

static void update_menu(void)
{
    UiListResult r = ui_list_update(&menu_list);
    if (r == UI_LIST_MOVED) {
        ui_list_draw(&menu_list);
    } else if (r == UI_LIST_CHOSEN) {
        menu_sel = menu_list.sel;
        menu_top = menu_list.top;
        switch (menu_act[menu_sel]) {
        case A_LAW: forcing = 0; set_mode(M_LAWS); break;
        case A_FORCE: forcing = 1; set_mode(M_LAWS); break;
        case A_ASSEMBLY: asm_sel = g_game.country.player_group; set_mode(M_ASSEMBLY); break;
        case A_MAP: map_open(SCREEN_GAME); break;
        case A_OPTIONS: options_open(SCREEN_GAME); break;
        case A_REFERENDUM: screen_change(SCREEN_REFERENDUM); break;
        case A_RESHUFFLE: screen_change(SCREEN_RESHUFFLE); break;
        case A_TV: {
            Country *c = &g_game.country;
            res_pop_before = sim_popularity(c);
            res_tv_ok = (sim_tv(c) == TV_SUCCES);
            res_pop_after = sim_popularity(c);
            res_kind = R_TV;
            sfx_play(res_tv_ok ? SFX_ADOPTED : SFX_REJECTED);
            set_mode(M_RESULT);
            break;
        }
        case A_DISSOLVE: {
            start_confirm(C_DISSOLVE, "Dissoudre l’Assemblée ?",
                          "Risqué : les législatives peuvent vous faire perdre la majorité. "
                          "Une seule dissolution par an.", "Dissoudre", "Annuler");
            break;
        }
        case A_BUDGET:
            screen_change(SCREEN_BUDGET);
            break;
        case A_QUIT: {
            start_confirm(C_QUIT, "Revenir au menu principal ?",
                          "La partie est sauvegardée telle quelle. Sur l’écran d’accueil : "
                          "B pour la reprendre, START pour une nouvelle partie "
                          "(qui remplacera la sauvegarde).", "Menu principal", "Annuler");
            break;
        }
        default: {
            const Country *c = &g_game.country;
            if (budget_blocking()) {
                screen_change(SCREEN_BUDGET);
                break;
            }
            g_report_snap[0] = c->growth;
            g_report_snap[1] = c->unemployment;
            g_report_snap[2] = c->debt;
            for (u16 i = 0; i < PIL_TOTAL; i++) g_report_snap[3 + i] = c->pillars[i];
            g_report_snap[3 + PIL_TOTAL] = c->gauges[JAUGE_TENSION];
            for (u16 e = 0; e < ELEC_COUNT; e++) g_opinion_prev[e] = c->opinion[e];
            g_opinion_prev_ok = 1;
            game_end_turn();
            break;
        }
        }
    }
}

// --- Proposer ou forcer une loi ----------------------------------------------------------

static u8 law_err(u8 id)
{
    return forcing ? sim_can_force_law(&g_game.country, id) : law_can_propose(&g_game.country, id);
}

static bool law_enabled(u16 i, void *ctx)
{
    (void)ctx;
    return law_err((u8)i) == LAW_OK;
}

// Liste des lois ; en cohabitation, « * » à droite des lois de souveraineté.
static void law_list_draw(void)
{
    u8 cohab = sim_cohabitation(&g_game.country);
    ui_list_draw(&law_list);
    for (u16 r = 0; r < law_list.rows; r++) {
        u16 i = law_list.top + r;
        if (i >= LOI_COUNT) break;
        // Repère d'état : coche = adoptée et appliquée, points = votée, effet en attente.
        if (law_is_adopted(&g_game.country, (u8)i))
            ui_text_draw(law_list.x + law_list.w - 4, law_list.y + r,
                         law_pending_turns(&g_game.country, (u8)i) ? "…" : "✓", UI_STYLE_HIGHLIGHT);
        if (cohab && sim_law_sovereign((u8)i))
            ui_text_draw(law_list.x + law_list.w - 2, law_list.y + r, "*",
                         ui_list_enabled(&law_list, i) ? UI_STYLE_HIGHLIGHT : UI_STYLE_DIM);
    }
}

static void draw_law_detail(void)
{
    const Country *c = &g_game.country;
    u8 id = law_list.sel;
    const LawDef *law = law_get(id);
    VoteResult v;
    SB_LINE(sb);
    u8 err, cost = sim_law_cost(c, id);

    ui_clear_rect(1, 12, 38, 12);
    ui_text_draw_n(1, 12, law_name[id], sim_cohabitation(c) && sim_law_sovereign(id) ? 25 : 38, 0,
                   UI_STYLE_HIGHLIGHT);
    if (sim_cohabitation(c) && sim_law_sovereign(id)) {
        const char *sv = txt[TXT_COHAB_SOUVERAINE];
        ui_text_draw(38 - ui_text_len(sv), 12, sv, UI_STYLE_DIM);
    }

    sb_str(&sb, "Coût : ");
    if (forcing) {
        sb_num(&sb, cost + ART49_CP, 0);
        sb_str(&sb, " CP (loi ");
        sb_num(&sb, cost, 0);
        sb_str(&sb, " + 49.3 ");
        sb_num(&sb, ART49_CP, 0);
        sb_str(&sb, ")");
        ui_text_draw(1, 13, sb.buf, UI_STYLE_TEXT);
        sb_init(&sb, sb_mem, SB_LINE_SIZE);
        sb_str(&sb, "Délai : ");
        sb_delay(&sb, law->delay_q);
        ui_text_draw(1, 14, sb.buf, UI_STYLE_TEXT);
    } else {
        sb_num(&sb, cost, 0);
        sb_str(&sb, " CP   Délai : ");
        sb_delay(&sb, law->delay_q);
        ui_text_draw(1, 13, sb.buf, UI_STYLE_TEXT);
    }

    ui_text_wrap(1, 15, 38, 3, law_desc[id], UI_STYLE_DIM);

    if (forcing) {
        ui_text_draw(1, 19, "Adoptée d’office, sans vote.", UI_STYLE_TEXT);
        ui_text_draw(1, 20, "Défiance en hausse, opposition", UI_STYLE_ALERT);
        ui_text_draw(1, 21, "furieuse. Une fois par an.", UI_STYLE_ALERT);
    } else {
        parl_vote(c, law->orientation, &v);
        sb_init(&sb, sb_mem, SB_LINE_SIZE);
        sb_str(&sb, "Aperçu du vote : ");
        sb_num(&sb, v.votes_pour, 0);
        sb_str(&sb, " voix sur ");
        sb_num(&sb, SEATS_TOTAL, 0);
        ui_text_draw(1, 19, sb.buf, UI_STYLE_TEXT);
        ui_gauge_draw(1, 20, 38, v.votes_pour, SEATS_TOTAL, v.adopted ? UI_GAUGE_POSITIVE : UI_GAUGE_NEGATIVE);
        ui_text_draw(1, 21, v.adopted ? "Adoptée si vous la proposez" : "Rejetée si vous la proposez",
                     v.adopted ? UI_STYLE_TEXT : UI_STYLE_ALERT);
    }

    err = law_err(id);
    if (err == LAW_ERR_ADOPTED) {
        u8 pt = law_pending_turns(c, id);
        sb_init(&sb, sb_mem, SB_LINE_SIZE);
        sb_str(&sb, "Adoptée");
        if (pt) {
            sb_str(&sb, ", effet dans ");
            sb_turns(&sb, pt);
        }
        ui_text_draw(1, 23, sb.buf, UI_STYLE_HIGHLIGHT);
    } else {
        kit_err_line(23, kit_err_from_law(err));
    }
}

static void draw_laws_full(void)
{
    page_begin(forcing ? "Forcer une loi (49.3)" : "Proposer une loi");
    ui_window(0, 3, 40, 8);
    ui_window(0, 11, 40, 17);
    ui_list_set_enabled_by(&law_list, law_enabled, NULL);
    ui_list_select_first_enabled(&law_list);
    law_list_draw();
    ui_footer(1, 26, 38, forcing ? "A : forcer   B : retour" : "A : proposer   B : retour");
    detail_dirty = TRUE;
}

static void update_laws(void)
{
    switch (ui_list_update(&law_list)) {
    case UI_LIST_MOVED:
        law_list_draw();
        detail_dirty = TRUE;
        break;
    case UI_LIST_CHOSEN:
        last_law = law_list.sel;
        if (forcing) {
            StrBuf sb;
            sb_init(&sb, confirm_text, sizeof confirm_text);
            sb_str(&sb, law_name[last_law]);
            sb_str(&sb, " : adoptée sans vote pour ");
            sb_num(&sb, sim_law_cost(&g_game.country, last_law) + ART49_CP, 0);
            sb_str(&sb, " CP. La défiance monte, l’opposition s’énerve.");
            start_confirm(C_FORCE, "Engager le 49.3 ?", confirm_text, "Forcer", "Annuler");
        } else if (game_propose_law(last_law, &last_vote) == LAW_OK) {
            last_adopted = last_vote.adopted;
            sfx_play(last_adopted ? SFX_ADOPTED : SFX_REJECTED);
            set_mode(M_VOTE);
        } else {
            sfx_play(SFX_ERROR);
        }
        break;
    case UI_LIST_CANCELLED:
        set_mode_quiet(M_MENU);
        break;
    default:
        break;
    }
}

// --- Confirmation (boîte modale) --------------------------------------------------------

static void update_confirm(void)
{
    s16 r = ui_confirm_update();   // B = non
    if (r < 0) return;
    ui_confirm_close();
    if (r == 0) {
        set_mode_quiet(prev_mode);
        return;
    }
    if (confirm_kind == C_QUIT) {
        game_save_and_quit();
    } else if (confirm_kind == C_FORCE) {
        if (sim_force_law(&g_game.country, last_law) == LAW_OK) {
            law_mask_set(&g_game.laws_tried, last_law);
            law_mask_set(&g_game.laws_adopted, last_law);
            res_kind = R_FORCE;
            sfx_play(SFX_ADOPTED);
            set_mode(M_RESULT);
        } else {
            sfx_play(SFX_ERROR);
            set_mode_quiet(prev_mode);
        }
    } else {
        if (sim_dissolve(&g_game.country, &g_game.legislatives)) {
            res_kind = R_DISSOLVE;
            sfx_play(g_game.legislatives.won ? SFX_ELECTION_WIN : SFX_ELECTION_LOSE);
            set_mode(M_RESULT);
        } else {
            sfx_play(SFX_ERROR);
            set_mode_quiet(prev_mode);
        }
    }
}

// --- Résultats (télévision, 49.3, dissolution) ------------------------------------------

static bool hemi_live;   // hémicycle affiché (dissolution) : animation

static void draw_result_full(void)
{
    const Country *c = &g_game.country;
    const LegislativeResult *lr = &g_game.legislatives;
    u16 y;

    art_reset();   // résultat d'une décision payée en CP : fondu doux
    hemi_live = FALSE;
    ui_window(0, 3, 40, 25);
    if (res_kind == R_TV) {
        // Plateau de la chaîne d'info : la journaliste savoure (ou pas) votre passage.
        kit_header("Intervention télévisée");
        ui_medallion(29, 4, UI_PORTRAIT_W, UI_PORTRAIT_H);
        ui_gueule_draw(GUEULE_JOUR_CHAINE_INFO, res_tv_ok ? EXPR_CONTENT : EXPR_FURIEUX, 29, 4, PAL2);
        ui_text_draw(1, 5, res_tv_ok ? "Succès d’audience" : "Le flop", res_tv_ok ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT);
        ui_separator(1, 6, 27);
        ui_text_flow(1, 7, 27, 9, txt[res_tv_ok ? TXT_TV_SUCCES : TXT_TV_FLOP], UI_STYLE_TEXT);
        ui_gueule_say(ui_gueule_talk_ms(txt[res_tv_ok ? TXT_TV_SUCCES : TXT_TV_FLOP]));
        ui_icon_draw(1, 18, UI_ICON_POPULARITY);
        ui_text_draw(3, 18, "Popularité", UI_STYLE_DIM);
        {
            SB_LINE(sb);
            sb_num(&sb, res_pop_before, 1);
            sb_str(&sb, " % → ");
            sb_num(&sb, res_pop_after, 1);
            sb_str(&sb, " %");
            ui_text_right(39, 18, sb.buf, res_tv_ok ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT);
        }
        ui_gauge_draw(1, 19, 38, res_pop_after, 1000, res_tv_ok ? UI_GAUGE_GOLD : UI_GAUGE_NEGATIVE);
    } else if (res_kind == R_FORCE) {
        const LawDef *law = law_get(last_law);
        kit_header("49.3 engagé");
        ui_medallion(29, 4, UI_PORTRAIT_W, UI_PORTRAIT_H);
        ui_gueule_draw(GUEULE_JOUR_EDITORIALISTE, EXPR_FURIEUX, 29, 4, PAL2);
        ui_text_wrap(1, 5, 27, 2, law_name[last_law], UI_STYLE_HIGHLIGHT);
        ui_text_flow(1, 8, 27, 7, txt[TXT_493_UTILISE], UI_STYLE_TEXT);
        ui_gueule_say(ui_gueule_talk_ms(txt[TXT_493_UTILISE]));
        ui_icon_draw(1, 17, UI_ICON_NEWS);
        ui_text_draw(3, 17, "Le Quotidien du Matin", UI_STYLE_DIM);
        ui_text_wrap(3, 18, 36, 2, law_journal_ok[last_law], UI_STYLE_TEXT);
        {
            SB_LINE(sb);
            sb_str(&sb, "Effets dans ");
            sb_delay(&sb, law->delay_q);
            sb_str(&sb, ".");
            ui_text_draw(3, 20, sb.buf, UI_STYLE_DIM);
        }
        ui_stat_row(22, &(UiStatRow){ UI_ICON_DEFIANCE, "Défiance", c->gauges[JAUGE_DEFIANCE], 0, " %",
                    c->gauges[JAUGE_DEFIANCE], GAUGE_MAX, UI_GAUGE_NEGATIVE, UI_STYLE_ALERT, 39 });
    } else {
        // Dissolution : la nouvelle Assemblée, puis les sièges groupe par groupe.
        kit_header("Dissolution");
        ui_hemi_draw(10, 4, UI_HEMI_ASSEMBLY, lr->seats_after);
        ui_hemi_focus(c->player_group);
        hemi_live = TRUE;
        ui_hemi_majority(1, 14, 38, lr->bloc_after, SEATS_TOTAL, SEATS_MAJORITY);
        for (u16 g = 0; g < GRP_COUNT; g++) {
            u16 x = 2 + g * 7;
            s16 d = (s16)lr->seats_after[g] - (s16)lr->seats_before[g];
            ui_text_draw(x + 1, 16, grp_sigle[g], g == c->player_group ? UI_STYLE_HIGHLIGHT : UI_STYLE_TEXT);
            SB_LINE(sb);
            sb_num(&sb, lr->seats_after[g], 0);
            ui_text_center(x, 17, 6, sb.buf, UI_STYLE_TEXT);
            sb_init(&sb, sb_mem, SB_LINE_SIZE);
            sb_signed(&sb, d, 0);
            ui_text_center(x, 18, 6, sb.buf, d >= 0 ? UI_STYLE_DIM : UI_STYLE_ALERT);
        }
        {
            SB_LINE(sb);
            sb_str(&sb, "Bloc présidentiel ");
            sb_arrow(&sb, lr->bloc_before, lr->bloc_after, 0);
            sb_str(&sb, lr->majority ? " · majorité absolue" : " · majorité relative");
            ui_text_center(1, 20, 38, sb.buf, lr->majority ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT);
        }
        y = 21 + (u16)ui_text_wrap(1, 21, 38, 2, txt[lr->won ? TXT_LEGISLATIVES_GAGNEES : TXT_LEGISLATIVES_PERDUES],
                                   lr->won ? UI_STYLE_TEXT : UI_STYLE_ALERT);
        if ((lr->cohab_started || lr->cohab_ended) && y <= 24)
            ui_text_wrap(1, y, 38, 25 - y, txt[lr->cohab_started ? TXT_COHAB_DEBUT : TXT_COHAB_FIN],
                         lr->cohab_started ? UI_STYLE_ALERT : UI_STYLE_HIGHLIGHT);
    }
    ui_footer(1, 26, 38, "A : continuer");
}

static void update_result(void)
{
    if (input_pressed() & (INPUT_OK | INPUT_CANCEL))
        set_mode(M_MENU);
}

// --- Résultat du vote -------------------------------------------------------------------
// Hémicycle dynamique (src/ui/ui_hemi.h) : les groupes « pour » s'allument de
// gauche à droite, puis le verdict et l'écho de la presse s'affichent.

#define HEMI_X 10
#define HEMI_Y 5
static bool vote_revealed;

static void draw_vote_verdict(void)
{
    SB_LINE(sb);
    const char *verdict = last_adopted ? "LOI ADOPTÉE" : "LOI REJETÉE";
    u16 n = ui_text_len(verdict);

    sb_str(&sb, "Pour ");
    sb_num(&sb, last_vote.votes_pour, 0);
    sb_str(&sb, " · Contre ");
    sb_num(&sb, last_vote.votes_contre, 0);
    sb_str(&sb, " · Majorité ");
    sb_num(&sb, SEATS_MAJORITY, 0);
    ui_text_center(1, 19, 38, sb.buf, UI_STYLE_TEXT);
    ui_hemi_majority(1, 15, 38, last_vote.votes_pour, SEATS_TOTAL, SEATS_MAJORITY);
    ui_icon_draw(19 - n / 2 - 2, 21, last_adopted ? UI_ICON_CHECK : UI_ICON_CROSS);
    ui_text_center(1, 21, 38, verdict, last_adopted ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT);
    ui_icon_draw(1, 22, UI_ICON_NEWS);   // rangée 25 laissée vide avant le pied de page
    ui_text_wrap(3, 22, 36, 2, last_adopted ? law_journal_ok[last_law] : law_journal_ko[last_law],
                 UI_STYLE_TEXT);
    if (last_adopted) {
        const LawDef *law = law_get(last_law);
        sb_init(&sb, sb_mem, SB_LINE_SIZE);
        sb_str(&sb, "Effets dans ");
        sb_delay(&sb, law->delay_q);
        sb_str(&sb, ".");
        ui_text_draw(3, 24, sb.buf, UI_STYLE_DIM);
    }
    ui_footer(1, 26, 38, "A : continuer");
}

static void draw_vote_full(void)
{
    const Country *c = &g_game.country;

    art_reset();
    kit_header("Vote de l’Assemblée");
    ui_window(0, 3, 40, 25);
    ui_text_center(1, 4, 38, law_name[last_law], UI_STYLE_HIGHLIGHT);
    ui_hemi_draw(HEMI_X, HEMI_Y, UI_HEMI_ASSEMBLY, c->seats);
    ui_hemi_vote(last_vote.pour, TRUE);
    kit_group_seats(17, c->seats, NULL, last_vote.pour);
    vote_revealed = FALSE;
}

static void update_vote(void)
{
    if (!(input_pressed() & (INPUT_OK | INPUT_CANCEL)))
        return;
    if (!vote_revealed) {           // A pendant l'animation : résultat immédiat
        ui_hemi_vote(last_vote.pour, FALSE);
        return;
    }
    set_mode(M_MENU);
}

static void draw_vote_anim(void)
{
    if (ui_hemi_update() && !vote_revealed) {
        vote_revealed = TRUE;
        draw_vote_verdict();
    }
}

// --- Assemblée ---------------------------------------------------------------------------

// Assemblée (revue DA P1-4) :
//  4-5   « Majorité absolue » et sa valeur (gauche), « Bloc présidentiel » et la sienne (droite)
//  6-15  hémicycle ; à gauche le soutien probable à vos lois, à droite le type de majorité
//  16-17 jauge de majorité
//  18-23 en-têtes puis une ligne par groupe (★ / ♦, sigle, sièges, jauge de relation, valeur),
//        nom complet du groupe sélectionné en note (rangée 24), rangée 25 vide
#define ASM_ROW0 19

static void draw_assembly_rows(void)
{
    const Country *c = &g_game.country;

    for (u16 g = 0; g < GRP_COUNT; g++) {
        SB_LINE(sb);
        u16 y = ASM_ROW0 + g;
        bool me = (g == c->player_group);
        if (g == asm_sel) {
            ui_select_bar(1, y, 38);
            ui_icon_draw(1, y, UI_ICON_CURSOR);
        } else {
            ui_select_bar_clear(1, y, 38);
            ui_clear_rect(1, y, 1, 1);
        }
        if (me) ui_icon_draw(2, y, UI_ICON_CP);
        else if (sim_in_bloc(c, (u8)g)) ui_icon_draw(2, y, UI_ICON_BULLET);
        else ui_clear_rect(2, y, 1, 1);
        ui_text_draw(4, y, grp_sigle[g], UI_STYLE_HIGHLIGHT);
        sb_num(&sb, c->seats[g], 0);
        ui_text_right(13, y, sb.buf, UI_STYLE_TEXT);
        ui_gauge_draw(16, y, 18, c->relation[g], RELATION_MAX,
                      c->relation[g] >= 50 ? UI_GAUGE_POSITIVE : UI_GAUGE_NEGATIVE);
        sb_init(&sb, sb_mem, SB_LINE_SIZE);
        sb_num(&sb, c->relation[g], 0);
        ui_clear_rect(35, y, 4, 1);
        ui_text_right(39, y, sb.buf, me ? UI_STYLE_HIGHLIGHT : UI_STYLE_TEXT);
    }
    ui_clear_rect(1, 24, 38, 1);
    ui_text_center(1, 24, 38, grp_name[asm_sel], UI_STYLE_DIM);
    ui_hemi_focus(asm_sel);
}

static void draw_assembly_full(void)
{
    const Country *c = &g_game.country;
    SB_LINE(sb);
    u16 bloc = sim_bloc_seats(c), sup = parl_support_seats(c);
    bool maj = sim_has_majority(c);

    art_reset();
    kit_header("Assemblée nationale");
    ui_window(0, 3, 40, 25);
    ui_text_draw(1, 4, "Majorité absolue", UI_STYLE_DIM);
    ui_number_draw(1, 5, SEATS_MAJORITY, 0, UI_STYLE_TEXT);
    ui_text_right(39, 4, "Bloc présidentiel", UI_STYLE_DIM);
    sb_num(&sb, bloc, 0);
    ui_text_right(39, 5, sb.buf, maj ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT);
    ui_hemi_draw(HEMI_X, 6, UI_HEMI_ASSEMBLY, c->seats);
    // Repères de part et d'autre de l'hémicycle (9 colonnes de chaque côté).
    ui_text_draw(1, 9, "Soutien", UI_STYLE_DIM);
    ui_text_draw(1, 10, "aux lois", UI_STYLE_DIM);
    ui_number_draw(1, 11, sup, 0, sup >= SEATS_MAJORITY ? UI_STYLE_TEXT : UI_STYLE_ALERT);
    ui_text_right(39, 9, "Majorité", UI_STYLE_DIM);
    ui_text_right(39, 10, maj ? "absolue" : "relative", maj ? UI_STYLE_TEXT : UI_STYLE_ALERT);
    ui_hemi_majority(1, 16, 38, bloc, SEATS_TOTAL, SEATS_MAJORITY);
    ui_text_right(13, 18, "Sièges", UI_STYLE_DIM);
    ui_text_draw(16, 18, "Relation", UI_STYLE_DIM);
    draw_assembly_rows();
    ui_footer(1, 26, 38, "Haut/Bas : groupe   B : retour");
}

static void update_assembly(void)
{
    u16 r = input_repeat();
    if (r & (INPUT_UP | INPUT_DOWN)) {
        asm_sel = (u8)((asm_sel + ((r & INPUT_UP) ? GRP_COUNT - 1 : 1)) % GRP_COUNT);
        sfx_play(SFX_MOVE);
        draw_assembly_rows();
        return;
    }
    if (input_pressed() & (INPUT_OK | INPUT_CANCEL)) {
        asm_sel = g_game.country.player_group;
        set_mode_quiet(M_MENU);
    }
}

// --- Écran ---------------------------------------------------------------------------------

static void game_init(void)
{
    ui_list_init(&law_list, 1, 4, 38, 5, law_name, LOI_COUNT);
    if (g_game.country.turn == 0) {   // nouvelle partie ou 2nd mandat
        g_budget_seen = 0;
        g_opinion_prev_ok = 0;
    }
    menu_sel = 0;
    menu_top = 0;
    menu_to_options = options_take_return();
    forcing = 0;
    mode = M_MENU;
    mode_dirty = TRUE;
}

static void game_update(void)
{
    if (mode != M_CONFIRM && (input_pressed() & INPUT_C)) {
        if (mode == M_MENU) screen_change(SCREEN_ELEC);   // détail des électorats
        else help_open(SCREEN_GAME);                      // aide
        return;
    }
    if (mode_dirty) return;   // laisse draw() finir le changement de mode
    switch (mode) {
    case M_MENU: update_menu(); break;
    case M_LAWS: update_laws(); break;
    case M_VOTE: update_vote(); break;
    case M_CONFIRM: update_confirm(); break;
    case M_RESULT: update_result(); break;
    default: update_assembly(); break;
    }
}

static void game_draw(void)
{
    if (mode_dirty) {
        mode_dirty = FALSE;
        switch (mode) {
        case M_MENU: draw_menu_full(); break;
        case M_LAWS: draw_laws_full(); break;
        case M_VOTE: draw_vote_full(); break;
        case M_RESULT: draw_result_full(); break;
        case M_CONFIRM: break;
        default: draw_assembly_full(); break;
        }
    }
    if (mode == M_LAWS && detail_dirty) {
        detail_dirty = FALSE;
        draw_law_detail();
    }
    if (mode == M_VOTE)
        draw_vote_anim();
    else if (mode == M_ASSEMBLY || (mode == M_RESULT && hemi_live))
        ui_hemi_update();
}

static void game_exit(void)
{
    art_reset();
}

const Screen scr_game = { game_init, game_update, game_draw, game_exit };
