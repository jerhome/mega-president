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
static char tv_label[UI_LINE_BUF];
static char ref_label[UI_LINE_BUF];
static char rsh_label[UI_LINE_BUF];
static char confirm_text[160];

static const char *const pillar_names[PIL_TOTAL] = {
    "Social", "Économie", "Technologie", "Écologie", "Sécurité", "Internat."   // 13 cases avant la jauge
};

// --- Utilitaires -----------------------------------------------------------------

static void cat_num(char *dst, s32 v)
{
    char b[14];
    ui_format_number(b, v, 0);
    strcat(dst, b);
}

static void cat_dec(char *dst, s32 v, u16 dec)
{
    char b[14];
    ui_format_number(b, v, dec);
    strcat(dst, b);
}

// « N trimestre(s) » (20 tours) ou « N mois » (60 tours : délai x 3).
static void cat_delay(char *dst, u8 delay_q)
{
    if (game_is_monthly()) {
        cat_num(dst, delay_q * 3);
        strcat(dst, " mois");
    } else {
        cat_num(dst, delay_q);
        strcat(dst, delay_q > 1 ? " trimestres" : " trimestre");
    }
}

// Délai restant d'une loi votée, dans l'unité du mode de jeu (tours = mois ou trimestres).
static void cat_turns(char *dst, u8 turns)
{
    cat_num(dst, turns);
    if (game_is_monthly()) strcat(dst, " mois");
    else strcat(dst, turns > 1 ? " trimestres" : " trimestre");
}

static const char *pm_name(u8 pm)
{
    return pm < PM_COUNT ? txt[TXT_PM_TECHNOCRATE_NOM + 2 * pm] : "-";
}

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

// Ligne d'indicateur : icône x=1, libellé x=3 (13 car., atténué), jauge de x=16
// jusqu'à la valeur, valeur alignée à droite (dernière colonne x_end - 1).
static void stat_row(u16 y, UiIcon icon, const char *label, s32 value, u16 dec, const char *unit,
                     s16 gval, s16 gmax, UiStyle gstyle, UiStyle vstyle, u16 x_end)
{
    char buf[UI_LINE_BUF];

    ui_icon_draw(1, y, icon);
    ui_text_draw_n(3, y, label, 13, 13, UI_STYLE_DIM);
    ui_gauge_draw(16, y, x_end - 7 - 16, gval, gmax, gstyle);
    ui_format_number(buf, value, dec);
    strcat(buf, unit);
    ui_clear_rect(x_end - 7, y, 7, 1);
    ui_text_right(x_end, y, buf, vstyle);
}

// Bandeau du haut : titre (ou date du tour) à gauche, capital politique à droite.
static void draw_header(const char *title)
{
    char buf[UI_LINE_BUF];
    const Country *c = &g_game.country;
    u16 n;

    if (title) {
        ui_banner(0, 0, 40, title, TRUE);
    } else {
        game_turn_label(c->turn, buf);
        ui_banner(0, 0, 40, buf, TRUE);
    }
    buf[0] = 0;
    cat_num(buf, c->cp);
    strcat(buf, "/");
    cat_num(buf, CP_MAX);
    n = ui_text_len(buf);
    ui_text_right(39, 1, " CP", UI_STYLE_DIM);
    ui_text_right(36, 1, buf, UI_STYLE_TEXT);
    ui_icon_draw(34 - n, 1, UI_ICON_CP);
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

static void menu_build(void)
{
    const Country *c = &g_game.country;
    u32 dis = 0;
    u8 n = 0;

    menu_act[n] = A_LAW;      menu_items[n++] = "Proposer une loi";
    if (!any_law_forceable()) dis |= (u32)1 << n;
    menu_act[n] = A_FORCE;    menu_items[n++] = "Forcer une loi (49.3)";
    tv_label[0] = 0;
    strcat(tv_label, "Intervention télévisée (");
    cat_num(tv_label, TV_CP);
    strcat(tv_label, " CP, ");
    cat_num(tv_label, sim_tv_flop_chance(c));
    strcat(tv_label, " %)");
    if (!sim_tv_available(c)) dis |= (u32)1 << n;
    menu_act[n] = A_TV;       menu_items[n++] = tv_label;
    menu_act[n] = A_ASSEMBLY; menu_items[n++] = "Voir l’Assemblée";
    menu_act[n] = A_MAP;      menu_items[n++] = "Carte des régions";
    ref_label[0] = 0;
    strcat(ref_label, txt[TXT_ACT_REFERENDUM]);
    strcat(ref_label, " (");
    cat_num(ref_label, REFERENDUM_CP);
    strcat(ref_label, " CP)");
    if (!any_law_referendum()) dis |= (u32)1 << n;
    menu_act[n] = A_REFERENDUM; menu_items[n++] = ref_label;
    rsh_label[0] = 0;
    strcat(rsh_label, txt[TXT_ACT_REMANIEMENT]);
    strcat(rsh_label, " (");
    cat_num(rsh_label, RESHUFFLE_CP);
    strcat(rsh_label, " CP)");
    if (!any_reshuffle()) dis |= (u32)1 << n;
    menu_act[n] = A_RESHUFFLE; menu_items[n++] = rsh_label;
    if (sim_can_dissolve(c)) {
        menu_act[n] = A_DISSOLVE; menu_items[n++] = "Dissoudre l’Assemblée";
    }
    if (sim_cohabitation(c)) {
        menu_act[n] = A_BUDGET;   menu_items[n++] = txt[TXT_BUD_TITRE_GOUV];
    } else if (sim_budget_due(c) && !c->budget_done) {
        menu_act[n] = A_BUDGET;   menu_items[n++] = "Présenter le budget";
    } else if (c->budget_done && sim_budget_amended_line(c)) {
        menu_act[n] = A_BUDGET;   menu_items[n++] = "Budget amendé : lecture définitive";
    }
    menu_act[n] = A_END;
    if (game_is_monthly())
        menu_items[n++] = budget_blocking() ? "Fin du mois (budget d’abord)" : "Fin du mois";
    else
        menu_items[n++] = budget_blocking() ? "Fin du trimestre (budget d’abord)" : "Fin du trimestre";
    menu_act[n] = A_OPTIONS;  menu_items[n++] = "Options";
    menu_act[n] = A_QUIT;     menu_items[n++] = "Menu principal (partie sauvegardée)";
    menu_n = n;
    if (menu_to_options) {
        u8 i;
        menu_to_options = FALSE;
        for (i = 0; i < n; i++)
            if (menu_act[i] == A_OPTIONS)
                menu_sel = i;
        menu_top = 0;
    }
    ui_list_init(&menu_list, 1, 22, 38, 5, menu_items, n);
    menu_list.disabled = dis;
    if (menu_sel < n && !(dis & ((u32)1 << menu_sel))) {
        menu_list.sel = menu_sel;
        menu_list.top = menu_top < n ? menu_top : 0;
        if (menu_list.sel < menu_list.top || menu_list.sel >= menu_list.top + menu_list.rows)
            menu_list.top = menu_list.sel >= menu_list.rows ? menu_list.sel - menu_list.rows + 1 : 0;
    }
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

// Nom de famille (après la dernière espace) : la colonne du médaillon fait 10 cases.
static const char *surname(const char *full)
{
    const char *s = full, *last = full;
    for (; *s; s++)
        if (*s == ' ') last = s + 1;
    return last;
}

// Cinq électorats sur deux lignes (3 + 2, cellules de 8 colonnes : « Pop 52 » + flèche de
// tendance). Libellés atténués, valeurs entières, rouge si l'électorat est sous les 50 %.
// La seconde ligne rappelle le détail (touche C).
static void draw_electorates(u16 y)
{
    char b[14];
    const Country *c = &g_game.country;
    for (u16 e = 0; e < ELEC_COUNT; e++) {
        u16 x = 2 + (e % 3) * 8;
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
    const char *name = cohab ? txt[TXT_COHAB_PM_GAUCHE_NOM + sim_cohab_camp(c)] : pm_name(c->pm);
    char buf[UI_LINE_BUF];
    u8 n = sim_cp_turns_to_next(c);

    ui_medallion(MED_X, MED_Y, MED_W, MED_H);
    ui_gueule_draw(id, pm_expr(), MED_X, MED_Y, PAL2);
    ui_text_center(MED_X, MED_Y + MED_H, MED_W, surname(name), cohab ? UI_STYLE_ALERT : UI_STYLE_HIGHLIGHT);
    if (cohab)
        ui_text_center(MED_X, MED_Y + MED_H + 1, MED_W, "Cohab.", UI_STYLE_ALERT);
    else
        ui_text_center(MED_X, MED_Y + MED_H + 1, MED_W, sim_has_majority(c) ? "Maj. abs." : "Maj. rel.",
                       sim_has_majority(c) ? UI_STYLE_TEXT : UI_STYLE_ALERT);
    buf[0] = 0;
    strcat(buf, "+");
    cat_num(buf, sim_cp_gain(c));
    strcat(buf, " CP");
    ui_text_center(MED_X, MED_Y + MED_H + 2, MED_W, buf, UI_STYLE_HIGHLIGHT);
    buf[0] = 0;
    if (n <= 1) {
        strcat(buf, "par tour");
    } else {
        strcat(buf, "en ");
        cat_num(buf, n);
        strcat(buf, " mois");
    }
    ui_text_center(MED_X, MED_Y + MED_H + 3, MED_W, buf, UI_STYLE_DIM);
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

    art_reset();
    draw_header(NULL);
    ui_window(0, 3, 40, 18);

    stat_row(4, UI_ICON_POPULARITY, "Popularité", pop, 1, " %", pop, 1000, UI_GAUGE_GOLD,
             pop >= 500 ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT, DASH_X_END);
    draw_electorates(5);
    stat_row(7, UI_ICON_GROWTH, "Croissance", c->growth, 2, " %", c->growth - GROWTH_MIN,
             GROWTH_MAX - GROWTH_MIN, c->growth >= 0 ? UI_GAUGE_POSITIVE : UI_GAUGE_NEGATIVE,
             c->growth >= 0 ? UI_STYLE_TEXT : UI_STYLE_ALERT, DASH_X_END);
    stat_row(8, UI_ICON_UNEMPLOYMENT, "Chômage", c->unemployment, 2, " %", c->unemployment, UNEMP_MAX,
             UI_GAUGE_NEUTRAL, UI_STYLE_TEXT, DASH_X_END);
    stat_row(9, UI_ICON_DEBT, "Dette", c->debt, 1, " %", c->debt, DEBT_MAX, UI_GAUGE_GOLD,
             UI_STYLE_TEXT, DASH_X_END);
    ui_separator(1, 10, DASH_X_END - 1);
    for (u16 i = 0; i < PIL_TOTAL; i++)
        stat_row(11 + i, pillar_icons[i], pillar_names[i], c->pillars[i], 1, "", c->pillars[i], PILLAR_MAX,
                 UI_GAUGE_POSITIVE, c->pillars[i] < 250 ? UI_STYLE_ALERT : UI_STYLE_TEXT, DASH_X_END);
    stat_row(17, UI_ICON_TENSION, "Tension", tn, 0, " %", tn, GAUGE_MAX, UI_GAUGE_NEGATIVE,
             tn >= 50 ? UI_STYLE_ALERT : UI_STYLE_TEXT, DASH_X_END);
    stat_row(18, UI_ICON_SCANDAL, "Scandale", sc, 0, " %", sc, GAUGE_MAX, UI_GAUGE_NEGATIVE,
             sc >= 50 ? UI_STYLE_ALERT : UI_STYLE_TEXT, DASH_X_END);
    stat_row(19, UI_ICON_DEFIANCE, "Défiance", df, 0, " %", df, GAUGE_MAX, UI_GAUGE_NEGATIVE,
             df >= 50 ? UI_STYLE_ALERT : UI_STYLE_TEXT, DASH_X_END);
    draw_pm_column();

    ui_window(0, 21, 40, 7);
    menu_build();
    ui_list_draw(&menu_list);
}

static void start_confirm(ConfirmKind k, const char *title, const char *text,
                          const char *const *choices)
{
    confirm_kind = k;
    prev_mode = mode;
    ui_msgbox_open(title, text, choices, 2);
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
            static const char *const ch[2] = { "Dissoudre", "Annuler" };
            start_confirm(C_DISSOLVE, "Dissoudre l’Assemblée ?",
                          "Risqué : les législatives peuvent vous faire perdre la majorité. "
                          "Une seule dissolution par an.", ch);
            break;
        }
        case A_BUDGET:
            screen_change(SCREEN_BUDGET);
            break;
        case A_QUIT: {
            static const char *const ch[2] = { "Menu principal", "Annuler" };
            start_confirm(C_QUIT, "Revenir au menu principal ?",
                          "La partie est sauvegardée telle quelle. Sur l’écran d’accueil : "
                          "B pour la reprendre, START pour une nouvelle partie "
                          "(qui remplacera la sauvegarde).", ch);
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

static void law_refresh_disabled(void)
{
    law_list.disabled = 0;
    law_list.disabled_hi = 0;
    for (u16 i = 0; i < LOI_COUNT; i++)
        if (law_err((u8)i) != LAW_OK)
            ui_list_set_disabled(&law_list, i, TRUE);
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
    char buf[UI_LINE_BUF];
    u8 err, cost = sim_law_cost(c, id);

    ui_clear_rect(1, 12, 38, 12);
    ui_text_draw_n(1, 12, law_name[id], sim_cohabitation(c) && sim_law_sovereign(id) ? 25 : 38, 0,
                   UI_STYLE_HIGHLIGHT);
    if (sim_cohabitation(c) && sim_law_sovereign(id)) {
        const char *sv = txt[TXT_COHAB_SOUVERAINE];
        ui_text_draw(38 - ui_text_len(sv), 12, sv, UI_STYLE_DIM);
    }

    buf[0] = 0;
    strcat(buf, "Coût : ");
    if (forcing) {
        cat_num(buf, cost + ART49_CP);
        strcat(buf, " CP (loi ");
        cat_num(buf, cost);
        strcat(buf, " + 49.3 ");
        cat_num(buf, ART49_CP);
        strcat(buf, ")");
        ui_text_draw(1, 13, buf, UI_STYLE_TEXT);
        buf[0] = 0;
        strcat(buf, "Délai : ");
        cat_delay(buf, law->delay_q);
        ui_text_draw(1, 14, buf, UI_STYLE_TEXT);
    } else {
        cat_num(buf, cost);
        strcat(buf, " CP   Délai : ");
        cat_delay(buf, law->delay_q);
        ui_text_draw(1, 13, buf, UI_STYLE_TEXT);
    }

    ui_text_wrap(1, 15, 38, 3, law_desc[id], UI_STYLE_DIM);

    if (forcing) {
        ui_text_draw(1, 19, "Adoptée d’office, sans vote.", UI_STYLE_TEXT);
        ui_text_draw(1, 20, "Défiance en hausse, opposition", UI_STYLE_ALERT);
        ui_text_draw(1, 21, "furieuse. Une fois par an.", UI_STYLE_ALERT);
    } else {
        parl_vote(c, law->orientation, &v);
        buf[0] = 0;
        strcat(buf, "Aperçu du vote : ");
        cat_num(buf, v.votes_pour);
        strcat(buf, " voix sur ");
        cat_num(buf, SEATS_TOTAL);
        ui_text_draw(1, 19, buf, UI_STYLE_TEXT);
        ui_gauge_draw(1, 20, 38, v.votes_pour, SEATS_TOTAL, v.adopted ? UI_GAUGE_POSITIVE : UI_GAUGE_NEGATIVE);
        ui_text_draw(1, 21, v.adopted ? "Adoptée si vous la proposez" : "Rejetée si vous la proposez",
                     v.adopted ? UI_STYLE_TEXT : UI_STYLE_ALERT);
    }

    err = law_err(id);
    if (err == LAW_ERR_CP)
        ui_text_draw(1, 23, "Capital politique insuffisant", UI_STYLE_ALERT);
    else if (err == LAW_ERR_ADOPTED)
    {
        u8 pt = law_pending_turns(c, id);
        buf[0] = 0;
        strcat(buf, "Adoptée");
        if (pt) {
            strcat(buf, ", effet dans ");
            cat_turns(buf, pt);
        }
        ui_text_draw(1, 23, buf, UI_STYLE_HIGHLIGHT);
    }
    else if (err == LAW_ERR_493)
        ui_text_draw(1, 23, "49.3 déjà utilisé cette année", UI_STYLE_ALERT);
    else if (err == LAW_ERR_COHAB)
        ui_text_wrap(1, 23, 38, 2, txt[TXT_COHAB_LOI_REFUSEE], UI_STYLE_ALERT);
    else if (err != LAW_OK)
        ui_text_draw(1, 23, "Impossible pour l’instant", UI_STYLE_ALERT);
}

static void draw_laws_full(void)
{
    art_reset();
    draw_header(forcing ? "Forcer une loi (49.3)" : "Proposer une loi");
    ui_window(0, 3, 40, 8);
    ui_window(0, 11, 40, 17);
    law_refresh_disabled();
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
            static const char *const ch[2] = { "Forcer", "Annuler" };
            confirm_text[0] = 0;
            strcat(confirm_text, law_name[last_law]);
            strcat(confirm_text, " : adoptée sans vote pour ");
            cat_num(confirm_text, sim_law_cost(&g_game.country, last_law) + ART49_CP);
            strcat(confirm_text, " CP. La défiance monte, l’opposition s’énerve.");
            start_confirm(C_FORCE, "Engager le 49.3 ?", confirm_text, ch);
        } else if (game_propose_law(last_law, &last_vote) == LAW_OK) {
            last_adopted = last_vote.adopted;
            sfx_play(last_adopted ? SFX_ADOPTED : SFX_REJECTED);
            set_mode(M_VOTE);
        } else {
            sfx_play(SFX_ERROR);
        }
        break;
    case UI_LIST_CANCELLED:
        set_mode(M_MENU);
        break;
    default:
        break;
    }
}

// --- Confirmation (boîte modale) --------------------------------------------------------

static void update_confirm(void)
{
    s16 r = ui_msgbox_update();
    if (r < 0 && (input_pressed() & INPUT_CANCEL)) r = 1;
    if (r < 0) return;
    ui_msgbox_close();
    if (r == 1) {
        set_mode(prev_mode);
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
            set_mode(prev_mode);
        }
    } else {
        if (sim_dissolve(&g_game.country, &g_game.legislatives)) {
            res_kind = R_DISSOLVE;
            sfx_play(g_game.legislatives.won ? SFX_ELECTION_WIN : SFX_ELECTION_LOSE);
            set_mode(M_RESULT);
        } else {
            sfx_play(SFX_ERROR);
            set_mode(prev_mode);
        }
    }
}

// --- Résultats (télévision, 49.3, dissolution) ------------------------------------------

static bool hemi_live;   // hémicycle affiché (dissolution) : animation

static void draw_result_full(void)
{
    const Country *c = &g_game.country;
    const LegislativeResult *lr = &g_game.legislatives;
    char buf[UI_LINE_BUF];
    u16 y;

    art_reset();
    hemi_live = FALSE;
    ui_window(0, 3, 40, 25);
    if (res_kind == R_TV) {
        // Plateau de la chaîne d'info : la journaliste savoure (ou pas) votre passage.
        draw_header("Intervention télévisée");
        ui_medallion(29, 4, UI_PORTRAIT_W, UI_PORTRAIT_H);
        ui_gueule_draw(GUEULE_JOUR_CHAINE_INFO, res_tv_ok ? EXPR_CONTENT : EXPR_FURIEUX, 29, 4, PAL2);
        ui_text_draw(1, 5, res_tv_ok ? "Succès d’audience" : "Le flop", res_tv_ok ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT);
        ui_separator(1, 6, 27);
        ui_text_flow(1, 7, 27, 9, txt[res_tv_ok ? TXT_TV_SUCCES : TXT_TV_FLOP], UI_STYLE_TEXT);
        ui_gueule_talk(ui_gueule_talk_ms(txt[res_tv_ok ? TXT_TV_SUCCES : TXT_TV_FLOP]));
        ui_icon_draw(1, 18, UI_ICON_POPULARITY);
        ui_text_draw(3, 18, "Popularité", UI_STYLE_DIM);
        buf[0] = 0;
        cat_dec(buf, res_pop_before, 1);
        strcat(buf, " % → ");
        cat_dec(buf, res_pop_after, 1);
        strcat(buf, " %");
        ui_text_right(39, 18, buf, res_tv_ok ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT);
        ui_gauge_draw(1, 19, 38, res_pop_after, 1000, res_tv_ok ? UI_GAUGE_GOLD : UI_GAUGE_NEGATIVE);
    } else if (res_kind == R_FORCE) {
        const LawDef *law = law_get(last_law);
        draw_header("49.3 engagé");
        ui_medallion(29, 4, UI_PORTRAIT_W, UI_PORTRAIT_H);
        ui_gueule_draw(GUEULE_JOUR_EDITORIALISTE, EXPR_FURIEUX, 29, 4, PAL2);
        ui_text_wrap(1, 5, 27, 2, law_name[last_law], UI_STYLE_HIGHLIGHT);
        ui_text_flow(1, 8, 27, 7, txt[TXT_493_UTILISE], UI_STYLE_TEXT);
        ui_gueule_talk(ui_gueule_talk_ms(txt[TXT_493_UTILISE]));
        ui_icon_draw(1, 17, UI_ICON_NEWS);
        ui_text_draw(3, 17, "Le Quotidien du Matin", UI_STYLE_DIM);
        ui_text_wrap(3, 18, 36, 2, law_journal_ok[last_law], UI_STYLE_TEXT);
        buf[0] = 0;
        strcat(buf, "Effets dans ");
        cat_delay(buf, law->delay_q);
        strcat(buf, ".");
        ui_text_draw(3, 20, buf, UI_STYLE_DIM);
        stat_row(22, UI_ICON_DEFIANCE, "Défiance", c->gauges[JAUGE_DEFIANCE], 0, " %", c->gauges[JAUGE_DEFIANCE],
                 GAUGE_MAX, UI_GAUGE_NEGATIVE, UI_STYLE_ALERT, 39);
    } else {
        // Dissolution : la nouvelle Assemblée, puis les sièges groupe par groupe.
        draw_header("Dissolution");
        ui_hemi_draw(10, 4, UI_HEMI_ASSEMBLY, lr->seats_after);
        ui_hemi_focus(c->player_group);
        hemi_live = TRUE;
        ui_hemi_majority(1, 14, 38, lr->bloc_after, SEATS_TOTAL, SEATS_MAJORITY);
        for (u16 g = 0; g < GRP_COUNT; g++) {
            u16 x = 2 + g * 7;
            s16 d = (s16)lr->seats_after[g] - (s16)lr->seats_before[g];
            ui_text_draw(x + 1, 16, grp_sigle[g], g == c->player_group ? UI_STYLE_HIGHLIGHT : UI_STYLE_TEXT);
            buf[0] = 0;
            cat_num(buf, lr->seats_after[g]);
            ui_text_center(x, 17, 6, buf, UI_STYLE_TEXT);
            buf[0] = 0;
            if (d > 0) strcat(buf, "+");
            cat_num(buf, d);
            ui_text_center(x, 18, 6, buf, d >= 0 ? UI_STYLE_DIM : UI_STYLE_ALERT);
        }
        buf[0] = 0;
        strcat(buf, "Bloc présidentiel ");
        cat_num(buf, lr->bloc_before);
        strcat(buf, " → ");
        cat_num(buf, lr->bloc_after);
        strcat(buf, lr->majority ? " · majorité absolue" : " · majorité relative");
        ui_text_center(1, 20, 38, buf, lr->majority ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT);
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

// Légende des groupes sur deux lignes : coche ou croix et sigle, puis sièges.
static void group_legend(u16 y, const u8 *pour, const u16 *seats)
{
    const Country *c = &g_game.country;
    char buf[UI_LINE_BUF];
    for (u16 g = 0; g < GRP_COUNT; g++) {
        u16 x = 2 + g * 7;   // cases de 7 colonnes, x 2 à 36
        if (pour) ui_icon_draw(x + 1, y, pour[g] ? UI_ICON_CHECK : UI_ICON_CROSS);
        ui_text_draw(x + 2, y, grp_sigle[g], g == c->player_group ? UI_STYLE_HIGHLIGHT : UI_STYLE_TEXT);
        buf[0] = 0;
        cat_num(buf, seats[g]);
        ui_text_center(x + 1, y + 1, 5, buf, UI_STYLE_DIM);
    }
}

static void draw_vote_verdict(void)
{
    char buf[UI_LINE_BUF];
    const char *verdict = last_adopted ? "LOI ADOPTÉE" : "LOI REJETÉE";
    u16 n = ui_text_len(verdict);

    buf[0] = 0;
    strcat(buf, "Pour ");
    cat_num(buf, last_vote.votes_pour);
    strcat(buf, " · Contre ");
    cat_num(buf, last_vote.votes_contre);
    strcat(buf, " · Majorité ");
    cat_num(buf, SEATS_MAJORITY);
    ui_text_center(1, 19, 38, buf, UI_STYLE_TEXT);
    ui_hemi_majority(1, 15, 38, last_vote.votes_pour, SEATS_TOTAL, SEATS_MAJORITY);
    ui_icon_draw(19 - n / 2 - 2, 21, last_adopted ? UI_ICON_CHECK : UI_ICON_CROSS);
    ui_text_center(1, 21, 38, verdict, last_adopted ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT);
    ui_icon_draw(1, 22, UI_ICON_NEWS);   // rangée 25 laissée vide avant le pied de page
    ui_text_wrap(3, 22, 36, 2, last_adopted ? law_journal_ok[last_law] : law_journal_ko[last_law],
                 UI_STYLE_TEXT);
    if (last_adopted) {
        const LawDef *law = law_get(last_law);
        buf[0] = 0;
        strcat(buf, "Effets dans ");
        cat_delay(buf, law->delay_q);
        strcat(buf, ".");
        ui_text_draw(3, 24, buf, UI_STYLE_DIM);
    }
    ui_footer(1, 26, 38, "A : continuer");
}

static void draw_vote_full(void)
{
    const Country *c = &g_game.country;

    art_reset();
    draw_header("Vote de l’Assemblée");
    ui_window(0, 3, 40, 25);
    ui_text_center(1, 4, 38, law_name[last_law], UI_STYLE_HIGHLIGHT);
    ui_hemi_draw(HEMI_X, HEMI_Y, UI_HEMI_ASSEMBLY, c->seats);
    ui_hemi_vote(last_vote.pour, TRUE);
    group_legend(17, last_vote.pour, c->seats);
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
    char buf[UI_LINE_BUF];

    for (u16 g = 0; g < GRP_COUNT; g++) {
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
        buf[0] = 0;
        cat_num(buf, c->seats[g]);
        ui_text_right(13, y, buf, UI_STYLE_TEXT);
        ui_gauge_draw(16, y, 18, c->relation[g], RELATION_MAX,
                      c->relation[g] >= 50 ? UI_GAUGE_POSITIVE : UI_GAUGE_NEGATIVE);
        buf[0] = 0;
        cat_num(buf, c->relation[g]);
        ui_clear_rect(35, y, 4, 1);
        ui_text_right(39, y, buf, me ? UI_STYLE_HIGHLIGHT : UI_STYLE_TEXT);
    }
    ui_clear_rect(1, 24, 38, 1);
    ui_text_center(1, 24, 38, grp_name[asm_sel], UI_STYLE_DIM);
    ui_hemi_focus(asm_sel);
}

static void draw_assembly_full(void)
{
    const Country *c = &g_game.country;
    char buf[UI_LINE_BUF];
    u16 bloc = sim_bloc_seats(c), sup = parl_support_seats(c);
    bool maj = sim_has_majority(c);

    art_reset();
    draw_header("Assemblée nationale");
    ui_window(0, 3, 40, 25);
    ui_text_draw(1, 4, "Majorité absolue", UI_STYLE_DIM);
    ui_number_draw(1, 5, SEATS_MAJORITY, 0, UI_STYLE_TEXT);
    ui_text_right(39, 4, "Bloc présidentiel", UI_STYLE_DIM);
    buf[0] = 0;
    cat_num(buf, bloc);
    ui_text_right(39, 5, buf, maj ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT);
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
        set_mode(M_MENU);
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
