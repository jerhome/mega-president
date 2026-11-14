// Mega Président — écran de jeu : tableau de bord, lois, Assemblée (tâche 6.1).
// Dessin uniquement via src/ui ; la simulation est modifiée par game.h.

#include <genesis.h>
#include <string.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_laws.h"
#include "content/content_world.h"

#define START_YEAR 2027

// Instantané des indicateurs pris au moment de « Fin du trimestre » : lu par
// scr_report.c pour afficher ce qui a bougé.
// Ordre : croissance, chômage, dette, 5 piliers, tension.
s16 g_report_snap[9];

typedef enum { M_MENU, M_LAWS, M_VOTE, M_ASSEMBLY } Mode;

static Mode mode;
static bool mode_dirty;     // redessin complet du mode
static u8 detail_dirty;     // détail de la loi (mode lois)
static UiList menu_list;
static UiList law_list;
static VoteResult last_vote;
static u8 last_law;
static u8 last_adopted;

static const char *const menu_items[] = {
    "Proposer une loi", "Voir l’Assemblée", "Fin du trimestre"
};

static const char *const pillar_names[PIL_COUNT] = {
    "Social", "Économie", "Technologie", "Écologie", "Sécurité"
};

// --- Utilitaires -----------------------------------------------------------------

static void cat_num(char *dst, s32 v)
{
    char b[14];
    ui_format_number(b, v, 0);
    strcat(dst, b);
}

static void set_mode(Mode m)
{
    mode = m;
    mode_dirty = TRUE;
}

// Ligne « libellé, jauge, valeur » : label x=1, jauge x=13 (16 tuiles), valeur à droite.
static void stat_row(u16 y, const char *label, s32 value, u16 dec, const char *unit,
                     s16 gval, s16 gmax, UiStyle style)
{
    char buf[14];
    u16 n, ul = ui_text_len(unit);

    ui_text_draw_n(1, y, label, 12, 12, UI_STYLE_TEXT);
    ui_gauge_draw(13, y, 16, gval, gmax, style);
    n = ui_format_number(buf, value, dec);
    ui_clear_rect(30, y, 8, 1);
    if (style == UI_STYLE_DIM) style = UI_STYLE_TEXT;   // valeur lisible
    ui_text_draw(38 - ul - n, y, buf, style);
    if (ul) ui_text_draw(38 - ul, y, unit, style);
}

static void draw_header(const char *title)
{
    char buf[40];
    const Country *c = &g_game.country;
    ui_window(0, 0, 40, 3);
    if (title) {
        ui_text_draw(1, 1, title, UI_STYLE_HIGHLIGHT);
    } else {
        u8 t = c->turn < 20 ? c->turn : 19;
        buf[0] = 0;
        strcat(buf, "Trimestre ");
        cat_num(buf, t + 1);
        strcat(buf, " / 20 - T");
        cat_num(buf, (t & 3) + 1);
        strcat(buf, " ");
        cat_num(buf, START_YEAR + (t >> 2));
        ui_text_draw(1, 1, buf, UI_STYLE_HIGHLIGHT);
    }
    buf[0] = 0;
    strcat(buf, "CP ");
    cat_num(buf, c->cp);
    strcat(buf, "/");
    cat_num(buf, CP_MAX);
    ui_text_draw(38 - ui_text_len(buf), 1, buf, UI_STYLE_TEXT);
}

// --- Tableau de bord -----------------------------------------------------------------

static void draw_menu_full(void)
{
    const Country *c = &g_game.country;
    ui_clear();
    draw_header(NULL);
    ui_window(0, 3, 40, 17);
    stat_row(4, "Popularité", sim_popularity(c), 1, " %", sim_popularity(c), 1000, UI_GAUGE_GOLD);
    stat_row(5, " Populaires", c->opinion[ELEC_POPULAIRES], 1, " %", c->opinion[ELEC_POPULAIRES], 1000, UI_GAUGE_POSITIVE);
    stat_row(6, " Moyennes", c->opinion[ELEC_MOYENNES], 1, " %", c->opinion[ELEC_MOYENNES], 1000, UI_GAUGE_POSITIVE);
    stat_row(7, " Aisées", c->opinion[ELEC_AISEES], 1, " %", c->opinion[ELEC_AISEES], 1000, UI_GAUGE_POSITIVE);

    stat_row(9, "Croissance", c->growth, 2, " %", c->growth - GROWTH_MIN, GROWTH_MAX - GROWTH_MIN,
             c->growth >= 0 ? UI_GAUGE_POSITIVE : UI_GAUGE_NEGATIVE);
    stat_row(10, "Chômage", c->unemployment, 2, " %", c->unemployment, UNEMP_MAX, UI_GAUGE_NEUTRAL);
    stat_row(11, "Dette", c->debt, 1, " %", c->debt, DEBT_MAX, UI_GAUGE_GOLD);

    for (u16 i = 0; i < PIL_COUNT; i++)
        stat_row(13 + i, pillar_names[i], c->pillars[i], 1, "", c->pillars[i], PILLAR_MAX, UI_GAUGE_POSITIVE);
    stat_row(18, "Tension", c->gauges[JAUGE_TENSION], 0, " %", c->gauges[JAUGE_TENSION], GAUGE_MAX, UI_GAUGE_NEGATIVE);

    ui_window(0, 20, 40, 8);
    ui_list_init(&menu_list, 1, 21, 38, 3, menu_items, 3);
    ui_list_draw(&menu_list);
    ui_text_draw(1, 25, "Haut/Bas : choisir   A : valider", UI_STYLE_DIM);
}

static void update_menu(void)
{
    UiListResult r = ui_list_update(&menu_list);
    if (r == UI_LIST_MOVED)
        ui_list_draw(&menu_list);
    else if (r == UI_LIST_CHOSEN) {
        switch (menu_list.sel) {
        case 0: set_mode(M_LAWS); break;
        case 1: set_mode(M_ASSEMBLY); break;
        default: {
            const Country *c = &g_game.country;
            g_report_snap[0] = c->growth;
            g_report_snap[1] = c->unemployment;
            g_report_snap[2] = c->debt;
            for (u16 i = 0; i < PIL_COUNT; i++) g_report_snap[3 + i] = c->pillars[i];
            g_report_snap[8] = c->gauges[JAUGE_TENSION];
            game_end_turn();
            break;
        }
        }
    }
}

// --- Proposer une loi -----------------------------------------------------------------

static void law_refresh_disabled(void)
{
    u32 d = 0;
    for (u16 i = 0; i < LOI_COUNT; i++)
        if (law_can_propose(&g_game.country, (u8)i) != LAW_OK)
            d |= (u32)1 << i;
    law_list.disabled = d;
}

static void draw_law_detail(void)
{
    const Country *c = &g_game.country;
    u8 id = law_list.sel;
    const LawDef *law = law_get(id);
    VoteResult v;
    char buf[40];
    u8 err;

    ui_clear_rect(1, 12, 38, 12);
    ui_text_draw(1, 12, law_name[id], UI_STYLE_HIGHLIGHT);

    buf[0] = 0;
    strcat(buf, "Coût : ");
    cat_num(buf, law->cost);
    strcat(buf, " CP   Délai : ");
    cat_num(buf, law->delay_q);
    strcat(buf, law->delay_q > 1 ? " trimestres" : " trimestre");
    ui_text_draw(1, 13, buf, UI_STYLE_TEXT);

    ui_text_wrap(1, 15, 38, 3, law_desc[id], UI_STYLE_DIM);

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

    err = law_can_propose(c, id);
    if (err == LAW_ERR_CP)
        ui_text_draw(1, 23, "Capital politique insuffisant", UI_STYLE_ALERT);
    else if (err == LAW_ERR_ADOPTED)
        ui_text_draw(1, 23, "Déjà adoptée", UI_STYLE_DIM);
    else if (err != LAW_OK)
        ui_text_draw(1, 23, "Impossible pour l’instant", UI_STYLE_ALERT);
}

static void draw_laws_full(void)
{
    ui_clear();
    draw_header("Proposer une loi");
    ui_window(0, 3, 40, 8);
    ui_window(0, 11, 40, 17);
    law_refresh_disabled();
    ui_list_draw(&law_list);
    ui_text_draw(1, 26, "A : proposer   B : retour", UI_STYLE_DIM);
    detail_dirty = TRUE;
}

static void update_laws(void)
{
    switch (ui_list_update(&law_list)) {
    case UI_LIST_MOVED:
        ui_list_draw(&law_list);
        detail_dirty = TRUE;
        break;
    case UI_LIST_CHOSEN:
        last_law = law_list.sel;
        if (game_propose_law(last_law, &last_vote) == LAW_OK) {
            last_adopted = last_vote.adopted;
            set_mode(M_VOTE);
        }
        break;
    case UI_LIST_CANCELLED:
        set_mode(M_MENU);
        break;
    default:
        break;
    }
}

// --- Résultat du vote -------------------------------------------------------------------

static void draw_vote_full(void)
{
    const Country *c = &g_game.country;
    char buf[40];
    u16 y;

    ui_clear();
    draw_header("Vote de l’Assemblée");
    ui_window(0, 3, 40, 25);
    ui_text_draw(1, 4, law_name[last_law], UI_STYLE_HIGHLIGHT);
    for (u16 g = 0; g < GRP_COUNT; g++) {
        y = 6 + g;
        ui_text_draw(1, y, grp_sigle[g], g == c->player_group ? UI_STYLE_HIGHLIGHT : UI_STYLE_TEXT);
        buf[0] = 0;
        cat_num(buf, c->seats[g]);
        strcat(buf, " sièges");
        ui_text_draw(7, y, buf, UI_STYLE_DIM);
        if (last_vote.pour[g]) ui_text_draw(30, y, "Pour", UI_STYLE_TEXT);
        else ui_text_draw(30, y, "Contre", UI_STYLE_ALERT);
    }
    buf[0] = 0;
    strcat(buf, "Pour ");
    cat_num(buf, last_vote.votes_pour);
    strcat(buf, "  Contre ");
    cat_num(buf, last_vote.votes_contre);
    strcat(buf, "  (majorité ");
    cat_num(buf, SEATS_MAJORITY);
    strcat(buf, ")");
    ui_text_draw(1, 12, buf, UI_STYLE_TEXT);
    ui_gauge_draw(1, 13, 38, last_vote.votes_pour, SEATS_TOTAL,
                  last_adopted ? UI_GAUGE_POSITIVE : UI_GAUGE_NEGATIVE);

    ui_text_center(1, 15, 38, last_adopted ? "LOI ADOPTÉE" : "LOI REJETÉE",
                   last_adopted ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT);
    ui_text_draw(1, 17, "Le Quotidien du Matin :", UI_STYLE_DIM);
    ui_text_wrap(1, 18, 38, 3, last_adopted ? law_journal_ok[last_law] : law_journal_ko[last_law],
                 UI_STYLE_TEXT);
    if (last_adopted) {
        const LawDef *law = law_get(last_law);
        buf[0] = 0;
        strcat(buf, "Effets dans ");
        cat_num(buf, law->delay_q);
        strcat(buf, law->delay_q > 1 ? " trimestres." : " trimestre.");
        ui_text_draw(1, 22, buf, UI_STYLE_DIM);
    }
    ui_text_draw(1, 26, "A : continuer", UI_STYLE_DIM);
}

static void update_vote(void)
{
    if (input_pressed() & (INPUT_OK | INPUT_CANCEL))
        set_mode(M_MENU);
}

// --- Assemblée ---------------------------------------------------------------------------

static void draw_assembly_full(void)
{
    const Country *c = &g_game.country;
    char buf[40];
    u16 y;

    ui_clear();
    draw_header("Assemblée nationale");
    ui_window(0, 3, 40, 25);
    for (u16 g = 0; g < GRP_COUNT; g++) {
        UiStyle st = g == c->player_group ? UI_STYLE_HIGHLIGHT : UI_STYLE_TEXT;
        y = 4 + g * 3;
        if (g == c->player_group) ui_icon_draw(1, y, UI_ICON_CURSOR);
        ui_text_draw(3, y, grp_sigle[g], st);
        ui_text_draw_n(7, y, grp_name[g], 31, 0, st);
        buf[0] = 0;
        cat_num(buf, c->seats[g]);
        strcat(buf, " sièges");
        ui_text_draw(3, y + 1, buf, UI_STYLE_DIM);
        ui_text_draw(15, y + 1, "Relation", UI_STYLE_DIM);
        ui_gauge_draw(24, y + 1, 11, c->relation[g], RELATION_MAX,
                      c->relation[g] >= 50 ? UI_GAUGE_POSITIVE : UI_GAUGE_NEGATIVE);
        buf[0] = 0;
        cat_num(buf, c->relation[g]);
        ui_text_draw(36, y + 1, buf, st);
    }
    buf[0] = 0;
    strcat(buf, "Majorité : ");
    cat_num(buf, SEATS_MAJORITY);
    strcat(buf, " sièges sur ");
    cat_num(buf, SEATS_TOTAL);
    ui_text_draw(1, 20, buf, UI_STYLE_TEXT);
    buf[0] = 0;
    strcat(buf, "Soutien actuel : ");
    cat_num(buf, parl_support_seats(c));
    strcat(buf, " sièges");
    ui_text_draw(1, 21, buf, UI_STYLE_TEXT);
    ui_text_draw(1, 26, "B : retour", UI_STYLE_DIM);
}

static void update_assembly(void)
{
    if (input_pressed() & (INPUT_OK | INPUT_CANCEL))
        set_mode(M_MENU);
}

// --- Écran ---------------------------------------------------------------------------------

static void game_init(void)
{
    ui_list_init(&law_list, 1, 4, 38, 5, law_name, LOI_COUNT);
    mode = M_MENU;
    mode_dirty = TRUE;
}

static void game_update(void)
{
    if (mode_dirty) return;   // laisse draw() finir le changement de mode
    switch (mode) {
    case M_MENU: update_menu(); break;
    case M_LAWS: update_laws(); break;
    case M_VOTE: update_vote(); break;
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
        default: draw_assembly_full(); break;
        }
    }
    if (mode == M_LAWS && detail_dirty) {
        detail_dirty = FALSE;
        draw_law_detail();
    }
}

const Screen scr_game = { game_init, game_update, game_draw, NULL };
