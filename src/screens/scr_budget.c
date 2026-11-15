// Mega Président — budget annuel détaillé (V4, tâche 4.11).
//
// 6 lignes x 5 crans, 3 préréglages en raccourci, aperçu en direct (déficit,
// croissance, opinion par électorat, vote de l'Assemblée), vote ou 49.3.
// Trois modes selon l'état de la partie :
//  - NORMAL  : le plan se règle puis se soumet (4e trimestre, budget pas encore adopté) ;
//  - SENAT   : le plan adopté a été amendé par le Sénat ; lecture définitive à 1 CP ;
//  - COHAB   : cohabitation, le plan du Premier ministre s'impose, le président ne règle
//              que la ligne Sécurité et défense (à un cran près, sans CP).
//
// Mise en page (x 1-27 à gauche du portrait x 29-38, y 4-15) :
//   4 préréglage | 6-11 les 6 lignes | 13-15 détail de la ligne sélectionnée
//  16-22 effets du plan (déficit, croissance, opinion par électorat, vote)
//  23-25 alertes ou actions | 26 aide des boutons

#include "audio/sfx.h"
#include <genesis.h>
#include <string.h>
#include "screens.h"
#include "scr_txt.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_texts.h"
#include "screens/evt_art.h"
#include "ui/ui_art.h"

// Défini dans scr_game.c : tour (+1) où l'écran du budget a été vu.
extern u8 g_budget_seen;

typedef enum { B_NORMAL, B_SENAT, B_COHAB } Mode;
typedef enum { P_EDIT, P_ACTION, P_RESULT, P_HELP } Phase;

#define LEFT_W   27     // largeur de la colonne de gauche (x 1-27)
#define Y_PRESET 4
#define Y_LINE0  6
#define Y_INFO   13
#define Y_EFFECT 16
#define Y_ZONE   23

static Mode mode;
static Phase phase;
static BudgetPlan plan;
static u8 focus;            // 0 = préréglage, 1..6 = ligne focus - 1
static u8 pre_sel;          // dernier préréglage choisi (0..2), point de départ des flèches
static bool d_all, d_rows, d_info, d_effect, d_zone;
static UiList action_list;
static u8 rejected;         // 1 : un vote a été rejeté
static u8 forced;           // 1 : adopté par 49.3
static u8 err_cp;           // 1 : CP insuffisants
static u8 lecdef_done;      // 1 : lecture définitive faite
static u8 amend_seen;       // ligne amendée à annoncer après le résultat (ligne + 1)
static VoteResult last_vote;
static char act_force[32];
static char act_lecdef[32];
static const char *action_items[3];

static void cat_num(char *dst, s32 v, u16 dec)
{
    char b[14];
    ui_format_number(b, v, dec);
    strcat(dst, b);
}

// « +1,2 », « -0,3 », « 0 » (une valeur nulle reste « 0 »).
static void cat_signed(char *dst, s32 v, u16 dec)
{
    if (v == 0) { strcat(dst, "0"); return; }
    if (v > 0) strcat(dst, "+");
    cat_num(dst, v, dec);
}

static void draw_header(void)
{
    const Country *c = &g_game.country;
    char buf[UI_LINE_BUF];
    u16 n;
    buf[0] = 0;
    if (mode == B_COHAB) {
        strcat(buf, txt[TXT_BUD_TITRE_GOUV]);
    } else {
        strcat(buf, "Budget · année ");
        cat_num(buf, sim_year(c) + 1, 0);
    }
    ui_banner(0, 0, 40, buf, TRUE);
    buf[0] = 0;
    cat_num(buf, c->cp, 0);
    strcat(buf, "/");
    cat_num(buf, CP_MAX, 0);
    n = ui_text_len(buf);
    ui_text_right(39, 1, " CP", UI_STYLE_DIM);
    ui_text_right(36, 1, buf, UI_STYLE_TEXT);
    ui_icon_draw(34 - n, 1, UI_ICON_CP);
}

// --- Plan -------------------------------------------------------------------------

static u8 line_of_focus(void) { return focus ? (u8)(focus - 1) : 0; }
static u16 row_y(u8 f) { return f == 0 ? Y_PRESET : (u16)(Y_LINE0 - 1 + f); }

static void load_plan(void)
{
    const Country *c = &g_game.country;
    if (mode == B_COHAB) sim_cohab_plan(c, &plan);
    else if (mode == B_SENAT) sim_budget_current_plan(c, &plan);
    else sim_budget_preset_plan(pre_sel, &plan);
}

// Le préréglage affiché : 0..2, ou 3 (personnalisé).
static u8 preset_now(void) { return sim_budget_plan_preset(&plan); }

static bool line_editable(u8 line)
{
    if (mode == B_NORMAL) return TRUE;
    if (mode == B_COHAB) return line == BUD_SECURITE;
    return FALSE;
}

// Plan réduit à une seule ligne : effet isolé d'un cran.
static void single_plan(BudgetPlan *p, u8 line, s8 lvl)
{
    for (u8 i = 0; i < BUD_LINES; i++) p->lvl[i] = 0;
    p->lvl[line] = lvl;
}

// --- Dessin -------------------------------------------------------------------------

static void draw_rows(void)
{
    char buf[UI_LINE_BUF];
    const Country *c = &g_game.country;

    ui_select_bar_clear(1, Y_PRESET, LEFT_W);
    for (u8 i = 0; i < BUD_LINES; i++) ui_select_bar_clear(1, Y_LINE0 + i, LEFT_W);
    ui_clear_rect(1, Y_PRESET, LEFT_W + 1, 8);
    ui_select_bar(1, row_y(focus), LEFT_W);

    // Ligne du préréglage (ou du plan imposé en cohabitation).
    if (mode == B_COHAB) {
        buf[0] = 0;
        strcat(buf, "Plan de ");
        strcat(buf, txt[TXT_COHAB_PM_GAUCHE_NOM + sim_cohab_camp(c)]);
        ui_text_center(1, Y_PRESET, LEFT_W, buf, UI_STYLE_ALERT);
    } else {
        ui_icon_draw(2, Y_PRESET, UI_ICON_ARROW_LEFT);
        ui_icon_draw(LEFT_W - 1, Y_PRESET, UI_ICON_ARROW_RIGHT);
        ui_text_center(4, Y_PRESET, LEFT_W - 6, txt[TXT_PRE_0 + preset_now()],
                       mode == B_SENAT ? UI_STYLE_DIM : UI_STYLE_HIGHLIGHT);
    }
    ui_separator(1, 5, LEFT_W);

    for (u8 i = 0; i < BUD_LINES; i++) {
        u16 y = Y_LINE0 + i;
        s8 l = plan.lvl[i];
        UiStyle st = (focus == i + 1) ? UI_STYLE_HIGHLIGHT
                     : (line_editable(i) ? UI_STYLE_TEXT : UI_STYLE_DIM);
        if (mode == B_SENAT && sim_budget_amended_line(c) == i + 1) st = UI_STYLE_ALERT;
        ui_text_draw_n(2, y, bl_nom(i), 18, 18, st);
        ui_gauge_draw(20, y, 5, (s16)(l + 2), 4, l == 0 ? UI_GAUGE_NEUTRAL : UI_GAUGE_GOLD);
        buf[0] = 0;
        cat_signed(buf, l, 0);
        ui_text_right(28, y, buf, l == 0 ? UI_STYLE_DIM : st);
        if (focus == i + 1 && line_editable(i)) {   // la ligne se règle avec gauche / droite
            ui_icon_draw(25, y, UI_ICON_ARROW_LEFT);
            ui_icon_draw(28, y, UI_ICON_ARROW_RIGHT);
        }
    }
    ui_separator(1, 12, LEFT_W);
}

static void draw_info(void)
{
    char buf[UI_LINE_BUF];
    const Country *c = &g_game.country;
    BudgetPlan one;
    u8 line;

    ui_clear_rect(1, Y_INFO, LEFT_W, 3);
    if (focus == 0) {
        ui_text_flow(1, Y_INFO, LEFT_W, 3,
                     mode == B_SENAT ? txt[TXT_BUD_AIDE_PRE]
                     : (preset_now() == BUDGET_PERSO ? txt[TXT_BUD_PERSO_DESC]
                                                      : txt[TXT_BUD_RIGUEUR_DESC + preset_now()]),
                     UI_STYLE_DIM);
        return;
    }
    line = line_of_focus();
    if (mode == B_SENAT && sim_budget_amended_line(c) == line + 1) {
        ui_text_draw(1, Y_INFO, "Amendé par le Sénat", UI_STYLE_ALERT);
        buf[0] = 0;
        strcat(buf, "Voulu : ");
        cat_signed(buf, c->bud_amend_from, 0);
        ui_text_draw_n(1, Y_INFO + 1, buf, LEFT_W, 0, UI_STYLE_TEXT);
        buf[0] = 0;
        strcat(buf, "Retenu : ");
        cat_signed(buf, plan.lvl[line], 0);
        ui_text_draw_n(1, Y_INFO + 2, buf, LEFT_W, 0, UI_STYLE_ALERT);
        return;
    }
    ui_text_draw_n(1, Y_INFO, bl_cran(line, plan.lvl[line]), LEFT_W, 0, UI_STYLE_HIGHLIGHT);
    single_plan(&one, line, plan.lvl[line]);
    buf[0] = 0;
    strcat(buf, "Déficit ");
    cat_signed(buf, sim_plan_deficit(&one), 1);
    strcat(buf, " pt/an");
    ui_text_draw(1, Y_INFO + 1, buf, UI_STYLE_DIM);
    buf[0] = 0;
    strcat(buf, "Croissance ");
    cat_signed(buf, sim_plan_growth(&one), 2);
    strcat(buf, " pt");
    ui_text_draw(1, Y_INFO + 2, buf, UI_STYLE_DIM);
}

// Une cellule de la grille « opinion » : valeur signée, rouge si négative, or si positive.
static void opinion_cell(u16 y, u8 e, s32 v, bool bold)
{
    char buf[14];
    buf[0] = 0;
    cat_signed(buf, v, 1);
    ui_text_right(14 + e * 5 + 5, y, buf,
                  v < 0 ? UI_STYLE_ALERT : (v > 0 && bold ? UI_STYLE_HIGHLIGHT : UI_STYLE_TEXT));
}

static void draw_effect(void)
{
    char buf[UI_LINE_BUF];
    s16 def = sim_plan_deficit(&plan), gr = sim_plan_growth(&plan);
    s32 pop = 0;
    u8 e;

    ui_clear_rect(1, Y_EFFECT, 38, 7);
    for (e = 0; e < ELEC_COUNT; e++) pop += (s32)elec_weight[e] * sim_plan_opinion(&plan, e);
    pop /= 100;
    ui_text_draw(1, Y_EFFECT, "Effets du plan", UI_STYLE_HIGHLIGHT);
    buf[0] = 0;
    cat_signed(buf, pop, 1);
    strcat(buf, " pt");
    ui_text_right(39, Y_EFFECT, buf, pop < 0 ? UI_STYLE_ALERT : UI_STYLE_HIGHLIGHT);
    ui_text_right(31, Y_EFFECT, "Popularité", UI_STYLE_DIM);

    ui_icon_draw(1, Y_EFFECT + 1, UI_ICON_DEBT);
    ui_text_draw(3, Y_EFFECT + 1, "Déficit", UI_STYLE_DIM);
    buf[0] = 0;
    cat_signed(buf, def, 1);
    strcat(buf, " pt");
    ui_text_right(19, Y_EFFECT + 1, buf, def > 0 ? UI_STYLE_ALERT : UI_STYLE_TEXT);
    ui_icon_draw(21, Y_EFFECT + 1, UI_ICON_GROWTH);
    ui_text_draw(23, Y_EFFECT + 1, "Croiss.", UI_STYLE_DIM);
    buf[0] = 0;
    cat_signed(buf, gr, 2);
    strcat(buf, " pt");
    ui_text_right(39, Y_EFFECT + 1, buf, gr < 0 ? UI_STYLE_ALERT : UI_STYLE_TEXT);

    // Opinion par électorat, en points : mesures, coût du déficit, total.
    ui_text_draw(1, Y_EFFECT + 2, "Opinion", UI_STYLE_DIM);
    for (e = 0; e < ELEC_COUNT; e++)
        ui_text_right(14 + e * 5 + 5, Y_EFFECT + 2, elec_txt(e, ET_ABR), UI_STYLE_DIM);
    ui_text_draw(1, Y_EFFECT + 3, "Mesures", UI_STYLE_DIM);
    ui_text_draw(1, Y_EFFECT + 4, "Marchés", UI_STYLE_DIM);
    ui_text_draw(1, Y_EFFECT + 5, "Total", UI_STYLE_TEXT);
    for (e = 0; e < ELEC_COUNT; e++) {
        s32 lines = 0, tot = sim_plan_opinion(&plan, e);
        for (u8 i = 0; i < BUD_LINES; i++) lines += sim_budget_line_opinion(i, plan.lvl[i], e);
        opinion_cell(Y_EFFECT + 3, e, lines, FALSE);
        opinion_cell(Y_EFFECT + 4, e, tot - lines, FALSE);
        opinion_cell(Y_EFFECT + 5, e, tot, TRUE);
    }
}

// Aperçu du vote de l'Assemblée (modes NORMAL) à la suite des effets, y 22.
static void draw_vote_preview(void)
{
    const Country *c = &g_game.country;
    VoteResult v;
    char buf[UI_LINE_BUF];

    ui_clear_rect(1, 22, 38, 1);
    if (mode != B_NORMAL) return;
    sim_budget_plan_preview(c, &plan, &v);
    ui_icon_draw(1, 22, UI_ICON_ASSEMBLY);
    ui_gauge_draw(3, 22, 18, v.votes_pour, SEATS_TOTAL, v.adopted ? UI_GAUGE_POSITIVE : UI_GAUGE_NEGATIVE);
    buf[0] = 0;
    cat_num(buf, v.votes_pour, 0);
    strcat(buf, " voix : ");
    strcat(buf, v.adopted ? "adopté" : "rejeté");
    ui_text_right(39, 22, buf, v.adopted ? UI_STYLE_TEXT : UI_STYLE_ALERT);
}

// Alertes en bas de l'écran pendant le réglage : ce que le plan déclenche.
static void draw_alerts(void)
{
    const Country *c = &g_game.country;
    char buf[UI_LINE_BUF];
    s16 def = sim_plan_deficit(&plan);
    u16 y = Y_ZONE;
    u8 e, l;
    s32 worst = 0;

    if (mode == B_COHAB) {
        const char *t = txt[TXT_COHAB_PLAN_GAUCHE + sim_cohab_camp(c)];
        ui_text_wrap(1, y, 38, 3, t, UI_STYLE_TEXT);
        return;
    }
    // Défiance des marchés : le coût du déficit est le plus lourd (Relance).
    for (e = 0; e < ELEC_COUNT; e++) {
        s32 lines = 0;
        for (u8 i = 0; i < BUD_LINES; i++) lines += sim_budget_line_opinion(i, plan.lvl[i], e);
        if (sim_plan_opinion(&plan, e) - lines < worst) worst = sim_plan_opinion(&plan, e) - lines;
    }
    if (worst <= -5 && y < 26) {
        buf[0] = 0;
        strcat(buf, "Marchés inquiets : jusqu’à ");
        cat_signed(buf, worst, 1);
        strcat(buf, " pt");
        ui_icon_draw(1, y, UI_ICON_DEFIANCE);
        ui_text_draw(3, y++, buf, UI_STYLE_ALERT);
    }
    if (c->debt > 1200 && def > 10 && y < 26) {
        ui_icon_draw(1, y, UI_ICON_DEBT);
        ui_text_draw(3, y++, "Prime de risque : dette trop élevée", UI_STYLE_ALERT);
    }
    if (sim_plan_insincere_malus(&plan) > 0 && y < 26) {
        ui_icon_draw(1, y, UI_ICON_SCANDAL);
        ui_text_draw(3, y++, "Budget insincère : vote plus difficile", UI_STYLE_ALERT);
    }
    l = sim_budget_senate_amend_line(c, &plan);
    if (l && y < 26) {
        buf[0] = 0;
        strcat(buf, "Le Sénat amendera : ");
        strcat(buf, bl_nom((u8)(l - 1)));
        ui_icon_draw(1, y, UI_ICON_ASSEMBLY);
        ui_text_draw_n(3, y++, buf, 36, 0, UI_STYLE_DIM);
    }
    if (y == Y_ZONE) {   // rien à signaler
        ui_icon_draw(1, y, UI_ICON_CHECK);
        ui_text_draw(3, y, "Aucune alerte sur ce plan", UI_STYLE_DIM);
    }
}

static void actions_setup(void)
{
    const Country *c = &g_game.country;
    if (mode == B_SENAT) {
        act_lecdef[0] = 0;
        strcat(act_lecdef, "Lecture définitive (");
        cat_num(act_lecdef, BUDGET_SENATE_CP, 0);
        strcat(act_lecdef, " CP)");
        action_items[0] = act_lecdef;
        action_items[1] = "Accepter l’amendement";
        ui_list_init(&action_list, 1, Y_ZONE, 38, 2, action_items, 2);
        action_list.disabled = c->cp < BUDGET_SENATE_CP ? 1u : 0u;
        return;
    }
    action_items[0] = "Faire voter l’Assemblée";
    act_force[0] = 0;
    strcat(act_force, "Utiliser le 49.3 (");
    cat_num(act_force, BUDGET_493_CP, 0);
    strcat(act_force, " CP)");
    action_items[1] = act_force;
    action_items[2] = rejected ? "Laisser reconduire" : "Modifier le plan";
    ui_list_init(&action_list, 1, Y_ZONE, 38, 3, action_items, 3);
    action_list.disabled = c->cp < BUDGET_493_CP ? 2u : 0u;
}

static void draw_zone(void)
{
    const Country *c = &g_game.country;
    char buf[UI_LINE_BUF];

    ui_clear_rect(1, Y_ZONE, 38, 4);
    ui_select_bar_clear(1, Y_ZONE, 37);
    ui_select_bar_clear(1, Y_ZONE + 1, 37);
    ui_select_bar_clear(1, Y_ZONE + 2, 37);
    if (phase == P_EDIT || phase == P_HELP) {
        if (phase == P_EDIT && mode == B_SENAT) {   // revisite : lecture définitive encore possible ?
            ui_icon_draw(1, Y_ZONE, UI_ICON_CROSS);
            buf[0] = 0;
            strcat(buf, "Amendement : ");
            strcat(buf, bl_nom((u8)(sim_budget_amended_line(c) - 1)));
            ui_text_draw_n(3, Y_ZONE, buf, 36, 0, UI_STYLE_ALERT);
            ui_text_wrap(1, Y_ZONE + 1, 38, 2, txt[TXT_SENAT_LECDEF_AIDE], UI_STYLE_DIM);
        } else if (phase == P_EDIT) {
            draw_alerts();
        }
        if (mode == B_COHAB)
            ui_footer(1, 26, 38, "Gauche/Droite : défense   C : aide");
        else if (mode == B_SENAT)
            ui_footer(1, 26, 38, "A : décider   B : retour   C : aide");
        else
            ui_footer(1, 26, 38, "A : valider   B : retour   C : aide");
    } else if (phase == P_ACTION) {
        action_list.bar_sel = 0xFF;
        ui_list_draw(&action_list);
        if (mode == B_SENAT) {
            ui_clear_rect(1, 22, 38, 1);
            ui_icon_draw(1, 22, UI_ICON_CROSS);
            buf[0] = 0;
            strcat(buf, "Amendement : ");
            strcat(buf, bl_nom((u8)(sim_budget_amended_line(c) - 1)));
            ui_text_draw_n(3, 22, buf, 36, 0, UI_STYLE_ALERT);
            if (c->cp < BUDGET_SENATE_CP)
                ui_text_wrap(1, Y_ZONE + 2, 38, 2, txt[TXT_SENAT_LECDEF_REFUSE], UI_STYLE_ALERT);
            else
                ui_text_wrap(1, Y_ZONE + 2, 38, 1, txt[TXT_SENAT_LECDEF_CONFIRME], UI_STYLE_DIM);
        } else {
            if (rejected || err_cp) ui_clear_rect(1, 22, 38, 1);
            if (rejected) {
                buf[0] = 0;
                strcat(buf, "Rejeté : ");
                cat_num(buf, last_vote.votes_pour, 0);
                strcat(buf, " voix sur ");
                cat_num(buf, SEATS_MAJORITY, 0);
                strcat(buf, " requises. Défiance +3.");
                ui_text_draw_n(1, 22, buf, 38, 0, UI_STYLE_ALERT);
            } else if (err_cp) {
                ui_text_draw(1, 22, "Capital politique insuffisant", UI_STYLE_ALERT);
            }
        }
        ui_footer(1, 26, 38, "A : valider   B : retour");
    } else {   // résultat
        const char *t, *body;
        ui_clear_rect(1, 22, 38, 1);
        if (lecdef_done) { t = "PLAN RÉTABLI"; body = txt[TXT_BUDJ_LECDEF]; }
        else if (forced) { t = "BUDGET ADOPTÉ PAR 49.3"; body = txt[TXT_BUDJ_493]; }
        else { t = "BUDGET ADOPTÉ"; body = txt[TXT_BUDJ_ADOPTE]; }
        {
            u16 n = ui_text_len(t);
            ui_text_center(2, 22, 37, t, UI_STYLE_HIGHLIGHT);
            ui_icon_draw(2 + (37 - n) / 2 - 2, 22, UI_ICON_CHECK);
        }
        ui_text_wrap(1, Y_ZONE, 38, 3, body, UI_STYLE_TEXT);
        ui_footer(1, 26, 38, "A : continuer");
    }
}

static void draw_portrait(void)
{
    const Country *c = &g_game.country;
    s16 def = sim_plan_deficit(&plan);
    ui_medallion(29, Y_PRESET, UI_PORTRAIT_W, UI_PORTRAIT_H);
    if (mode == B_COHAB)
        ui_gueule_draw(GUEULE_COHAB(sim_cohab_camp(c)), EXPR_CONTENT, 29, Y_PRESET, PAL2);
    else
        ui_gueule_draw(GUEULE_MIN_ECONOMIE, def < 0 ? EXPR_CONTENT : (def > 0 ? EXPR_GENE : EXPR_NEUTRE),
                       29, Y_PRESET, PAL2);
}

// L'expression de la ministre suit le déficit sans redessiner tout l'écran.
static void update_portrait_expr(void)
{
    s16 def = sim_plan_deficit(&plan);
    if (mode == B_COHAB) return;
    ui_gueule_expr(GUEULE_MIN_ECONOMIE, def < 0 ? EXPR_CONTENT : (def > 0 ? EXPR_GENE : EXPR_NEUTRE),
                   29, Y_PRESET, PAL2);
}

// --- Logique ------------------------------------------------------------------------

static void budget_init(void)
{
    const Country *c = &g_game.country;
    pre_sel = BUDGET_EQUILIBRE;
    phase = P_EDIT;
    rejected = forced = err_cp = lecdef_done = amend_seen = 0;
    focus = 1;
    if (sim_cohabitation(c)) {
        mode = B_COHAB;
        focus = (u8)(BUD_SECURITE + 1);
    } else if (c->budget_done && sim_budget_amended_line(c)) {
        mode = B_SENAT;
        focus = sim_budget_amended_line(c);
    } else {
        mode = B_NORMAL;
        g_budget_seen = c->turn + 1;
        focus = 0;
    }
    load_plan();
    d_all = TRUE;
}

static void leave(void)
{
    screen_change(SCREEN_GAME);
}

static void mark_plan_changed(void)
{
    d_rows = d_info = d_effect = d_zone = TRUE;
}

static void open_help(void)
{
    const char *title, *text;
    if (focus == 0) {
        title = "Préréglages";
        text = txt[TXT_BUD_AIDE_PRE];
    } else if (mode == B_COHAB && line_of_focus() == BUD_SECURITE) {
        title = txt[TXT_COHAB_DEFENSE];
        text = txt[TXT_COHAB_DEFENSE_AIDE];
    } else if (mode == B_SENAT && sim_budget_amended_line(&g_game.country) == focus) {
        title = bl_long(line_of_focus());
        text = txt[TXT_SENAT_LECDEF_AIDE];
    } else {
        title = bl_long(line_of_focus());
        text = bl_aide(line_of_focus());
    }
    ui_msgbox_open(title, text, NULL, 0);
    phase = P_HELP;
}

static void change_value(s8 dir)
{
    Country *c = &g_game.country;
    if (focus == 0) {
        s8 p;
        if (mode != B_NORMAL) return;
        p = (s8)pre_sel + dir;
        if (p < 0 || p >= BUDGET_COUNT) { sfx_play(SFX_ERROR); return; }
        pre_sel = (u8)p;
        sim_budget_preset_plan(pre_sel, &plan);
        sfx_play(SFX_MOVE);
        mark_plan_changed();
        update_portrait_expr();
        return;
    }
    if (!line_editable(line_of_focus())) { sfx_play(SFX_ERROR); return; }
    if (mode == B_COHAB) {
        s8 a = (s8)(c->cohab_def_adj + dir);
        if (a < -1 || a > 1) { sfx_play(SFX_ERROR); return; }
        sim_cohab_set_defense(c, a);
        sim_cohab_plan(c, &plan);
    } else {
        s8 l = (s8)(plan.lvl[line_of_focus()] + dir);
        if (l < BUD_LVL_MIN || l > BUD_LVL_MAX) { sfx_play(SFX_ERROR); return; }
        plan.lvl[line_of_focus()] = l;
    }
    sfx_play(SFX_MOVE);
    mark_plan_changed();
    update_portrait_expr();
}

static void do_vote(void)
{
    Country *c = &g_game.country;
    if (sim_budget_plan_vote(c, &plan, &last_vote) != BUDGET_OK) {
        sfx_play(SFX_ERROR);
        return;
    }
    if (last_vote.adopted) {
        forced = 0;
        phase = P_RESULT;
        amend_seen = sim_budget_amended_line(c);
        sfx_play(SFX_ADOPTED);
    } else {
        rejected = 1;
        err_cp = 0;
        actions_setup();
        action_list.sel = 0;
        sfx_play(SFX_REJECTED);
    }
    d_zone = TRUE;
    draw_header();   // défiance : pas affichée ici, CP inchangés
}

static void do_force(void)
{
    Country *c = &g_game.country;
    if (sim_budget_plan_force(c, &plan) == BUDGET_OK) {
        forced = 1;
        phase = P_RESULT;
        amend_seen = sim_budget_amended_line(c);
        sfx_play(SFX_ADOPTED);
        draw_header();
    } else {
        err_cp = 1;
        sfx_play(SFX_ERROR);
    }
    d_zone = TRUE;
}

static void do_lecdef(void)
{
    Country *c = &g_game.country;
    if (sim_budget_definitive_reading(c) == BUDGET_OK) {
        lecdef_done = 1;
        sim_budget_current_plan(c, &plan);
        phase = P_RESULT;
        mode = B_NORMAL;   // le plan est rétabli : plus d'amendement à afficher
        sfx_play(SFX_ADOPTED);
        draw_header();
        d_rows = d_info = d_effect = TRUE;
    } else {
        sfx_play(SFX_ERROR);
    }
    d_zone = TRUE;
}

static void update_edit(void)
{
    u16 r = input_repeat();
    u16 p = input_pressed();
    u8 first = (mode == B_NORMAL) ? 0 : 1;

    if (p & INPUT_C) {
        open_help();
        return;
    }
    if (p & INPUT_B) {
        leave();
        return;
    }
    if (r & INPUT_DOWN) {
        focus = (u8)(focus >= BUD_LINES ? first : focus + 1);
        if (focus < first) focus = first;
        sfx_play(SFX_MOVE);
        d_rows = d_info = TRUE;
    } else if (r & INPUT_UP) {
        focus = (u8)(focus <= first ? BUD_LINES : focus - 1);
        sfx_play(SFX_MOVE);
        d_rows = d_info = TRUE;
    } else if (r & INPUT_LEFT) {
        change_value(-1);
    } else if (r & INPUT_RIGHT) {
        change_value(1);
    } else if ((p & (INPUT_A | INPUT_START)) && mode != B_COHAB) {
        sfx_play(SFX_OK);
        phase = P_ACTION;
        actions_setup();
        d_zone = TRUE;
    }
}

static void update_action(void)
{
    switch (ui_list_update(&action_list)) {
    case UI_LIST_MOVED:
        ui_list_draw(&action_list);
        break;
    case UI_LIST_CANCELLED:
        phase = P_EDIT;
        d_zone = TRUE;
        break;
    case UI_LIST_CHOSEN:
        if (mode == B_SENAT) {
            if (action_list.sel == 0) do_lecdef();
            else leave();
        } else if (action_list.sel == 0) {
            do_vote();
        } else if (action_list.sel == 1) {
            do_force();
        } else if (rejected) {
            leave();   // reconduction en fin d'année
        } else {
            phase = P_EDIT;
            d_zone = TRUE;
        }
        break;
    default:
        break;
    }
}

static void budget_update(void)
{
    if (d_all) return;
    switch (phase) {
    case P_EDIT: update_edit(); break;
    case P_ACTION: update_action(); break;
    case P_HELP:
        if (ui_msgbox_update() >= 0) {
            ui_msgbox_close();
            phase = P_EDIT;
            d_all = TRUE;
        }
        break;
    default:
        if (input_pressed() & (INPUT_OK | INPUT_START)) {
            if (amend_seen && !lecdef_done && sim_budget_amended_line(&g_game.country)) {
                // Le Sénat a amendé le plan adopté : lecture définitive ou acceptation.
                mode = B_SENAT;
                amend_seen = 0;
                focus = sim_budget_amended_line(&g_game.country);
                load_plan();
                phase = P_ACTION;
                actions_setup();
                d_all = TRUE;
            } else {
                leave();
            }
        }
        break;
    }
}

static void budget_draw(void)
{
    if (d_all) {
        d_all = FALSE;
        art_reset();
        draw_header();
        ui_window(0, 3, 40, 25);
        draw_portrait();
        d_rows = d_info = d_effect = d_zone = TRUE;
    }
    if (d_rows) { d_rows = FALSE; draw_rows(); }
    if (d_info) { d_info = FALSE; draw_info(); }
    if (d_effect) {
        d_effect = FALSE;
        draw_effect();
        draw_vote_preview();
    }
    if (d_zone) { d_zone = FALSE; draw_zone(); }
}

static void budget_exit(void)
{
    art_reset();
}

const Screen scr_budget = { budget_init, budget_update, budget_draw, budget_exit };
