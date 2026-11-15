// Mega Président — budget annuel : choix, aperçu des effets et du vote,
// vote ou 49.3 (V1, tâche 1.06).

#include "audio/sfx.h"
#include <genesis.h>
#include <string.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_texts.h"
#include "screens/evt_art.h"
#include "ui/ui_art.h"

// Défini dans scr_game.c : tour (+1) où l'écran du budget a été vu.
extern u8 g_budget_seen;

typedef enum { P_PICK, P_ACTION, P_RESULT } Phase;

static Phase phase;
static bool dirty;          // redessin complet
static bool detail_dirty;   // effets et aperçu du vote
static bool zone_dirty;     // zone du bas (actions, messages)
static UiList budget_list;
static UiList action_list;
static u8 chosen;
static u8 rejected;         // 1 : un vote a été rejeté
static u8 forced;           // 1 : adopté par 49.3
static u8 err_cp;           // 1 : 49.3 impossible, CP insuffisants
static VoteResult last_vote;
static char act_force[32];

static const char *budget_items[BUDGET_COUNT];
static const char *action_items[3];

static void cat_num(char *dst, s32 v, u16 dec)
{
    char b[14];
    ui_format_number(b, v, dec);
    strcat(dst, b);
}

static void cat_signed(char *dst, s32 v, u16 dec)
{
    if (v > 0) strcat(dst, "+");
    cat_num(dst, v, dec);
}

static void draw_header(void)
{
    const Country *c = &g_game.country;
    char buf[UI_LINE_BUF];
    u16 n;
    buf[0] = 0;
    strcat(buf, "Budget · année ");
    cat_num(buf, sim_year(c) + 1, 0);
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

static void actions_setup(void)
{
    action_items[0] = "Faire voter l’Assemblée";
    act_force[0] = 0;
    strcat(act_force, "Utiliser le 49.3 (");
    cat_num(act_force, BUDGET_493_CP, 0);
    strcat(act_force, " CP)");
    action_items[1] = act_force;
    action_items[2] = rejected ? "Laisser reconduire" : "Retour";
    ui_list_init(&action_list, 1, 21, 38, 3, action_items, 3);
    action_list.disabled = g_game.country.cp < BUDGET_493_CP ? 2u : 0u;
}

static void budget_init(void)
{
    g_budget_seen = g_game.country.turn + 1;
    budget_items[BUDGET_RIGUEUR] = txt[TXT_BUD_RIGUEUR];
    budget_items[BUDGET_EQUILIBRE] = txt[TXT_BUD_EQUILIBRE];
    budget_items[BUDGET_RELANCE] = txt[TXT_BUD_RELANCE];
    ui_list_init(&budget_list, 1, 4, 27, 3, budget_items, 3);
    budget_list.sel = BUDGET_EQUILIBRE;
    phase = P_PICK;
    rejected = forced = err_cp = 0;
    dirty = TRUE;
}

static void leave(void)
{
    screen_change(SCREEN_GAME);
}

static void budget_update(void)
{
    Country *c = &g_game.country;

    if (dirty) return;
    switch (phase) {
    case P_PICK:
        switch (ui_list_update(&budget_list)) {
        case UI_LIST_MOVED:
            ui_list_draw(&budget_list);
            detail_dirty = TRUE;
            break;
        case UI_LIST_CHOSEN:
            chosen = budget_list.sel;
            phase = P_ACTION;
            actions_setup();
            zone_dirty = TRUE;
            break;
        case UI_LIST_CANCELLED:
            leave();
            break;
        default:
            break;
        }
        break;
    case P_ACTION:
        switch (ui_list_update(&action_list)) {
        case UI_LIST_MOVED:
            ui_list_draw(&action_list);
            break;
        case UI_LIST_CANCELLED:
            if (rejected) {
                leave();
            } else {
                phase = P_PICK;
                zone_dirty = TRUE;
            }
            break;
        case UI_LIST_CHOSEN:
            if (action_list.sel == 0) {
                if (sim_budget_vote(c, chosen, &last_vote) == BUDGET_OK) {
                    if (last_vote.adopted) {
                        forced = 0;
                        phase = P_RESULT;
                        sfx_play(SFX_ADOPTED);
                    } else {
                        rejected = 1;
                        actions_setup();
                        action_list.sel = 0;
                        sfx_play(SFX_REJECTED);
                    }
                    detail_dirty = TRUE;
                    zone_dirty = TRUE;
                    draw_header();   // défiance : pas affichée ici, CP inchangés
                } else {
                    sfx_play(SFX_ERROR);
                }
            } else if (action_list.sel == 1) {
                if (sim_budget_force(c, chosen) == BUDGET_OK) {
                    forced = 1;
                    phase = P_RESULT;
                    sfx_play(SFX_ADOPTED);
                    draw_header();
                    zone_dirty = TRUE;
                } else {
                    err_cp = 1;
                    sfx_play(SFX_ERROR);
                    zone_dirty = TRUE;
                }
            } else {
                if (rejected) leave();
                else {
                    phase = P_PICK;
                    zone_dirty = TRUE;
                }
            }
            break;
        default:
            break;
        }
        break;
    default:
        if (input_pressed() & (INPUT_OK | INPUT_START))
            leave();
        break;
    }
}

// Ligne « icône, libellé, valeur » de la colonne de gauche (fin en colonne 27).
static void bud_row(u16 y, UiIcon icon, const char *label, const char *val, UiStyle st)
{
    ui_icon_draw(1, y, icon);
    ui_text_draw(3, y, label, UI_STYLE_DIM);
    ui_text_right(28, y, val, st);
}

static void draw_detail(void)
{
    const Country *c = &g_game.country;
    char buf[UI_LINE_BUF];
    u8 b = (phase == P_PICK) ? budget_list.sel : chosen;
    s16 def = sim_budget_deficit(b), gr = sim_budget_growth(b), tn = sim_budget_tension(b);
    VoteResult v;

    ui_clear_rect(1, 8, 27, 8);
    ui_clear_rect(1, 16, 38, 4);
    // La ministre de l'Économie juge le budget : ravie de la rigueur, gênée par le déficit.
    ui_gueule_expr(GUEULE_MIN_ECONOMIE, def < 0 ? EXPR_CONTENT : (def > 0 ? EXPR_GENE : EXPR_NEUTRE),
                   29, 4, PAL2);
    ui_text_wrap(1, 8, 27, 3, txt[TXT_BUD_RIGUEUR_DESC + b], UI_STYLE_TEXT);

    buf[0] = 0;
    cat_signed(buf, def, 1);
    strcat(buf, " pt PIB/an");
    bud_row(12, UI_ICON_DEBT, "Déficit", buf, def > 0 ? UI_STYLE_ALERT : UI_STYLE_TEXT);
    buf[0] = 0;
    cat_signed(buf, gr, 2);
    strcat(buf, " pt");
    bud_row(13, UI_ICON_GROWTH, "Croissance", buf, gr < 0 ? UI_STYLE_ALERT : UI_STYLE_TEXT);
    buf[0] = 0;
    cat_signed(buf, tn, 0);
    bud_row(14, UI_ICON_TENSION, "Tension", buf, tn > 0 ? UI_STYLE_ALERT : UI_STYLE_TEXT);
    buf[0] = 0;
    strcat(buf, "Pop ");
    cat_signed(buf, sim_budget_opinion(b, ELEC_POPULAIRES), 1);
    strcat(buf, " Moy ");
    cat_signed(buf, sim_budget_opinion(b, ELEC_MOYENNES), 1);
    strcat(buf, " Ais ");
    cat_signed(buf, sim_budget_opinion(b, ELEC_AISEES), 1);
    ui_icon_draw(1, 15, UI_ICON_SOCIAL);
    ui_text_draw_n(3, 15, buf, 26, 0, UI_STYLE_TEXT);

    sim_budget_preview(c, b, &v);
    buf[0] = 0;
    strcat(buf, "Aperçu du vote : ");
    cat_num(buf, v.votes_pour, 0);
    strcat(buf, " voix sur ");
    cat_num(buf, SEATS_TOTAL, 0);
    ui_icon_draw(1, 17, UI_ICON_ASSEMBLY);
    ui_text_draw(3, 17, buf, UI_STYLE_TEXT);
    ui_gauge_draw(1, 18, 38, v.votes_pour, SEATS_TOTAL, v.adopted ? UI_GAUGE_POSITIVE : UI_GAUGE_NEGATIVE);
    ui_text_draw(3, 19, v.adopted ? "Adopté si vous le soumettez" : "Rejeté si vous le soumettez",
                 v.adopted ? UI_STYLE_TEXT : UI_STYLE_ALERT);
}

static void draw_zone(void)
{
    char buf[UI_LINE_BUF];

    ui_clear_rect(1, 20, 38, 7);
    ui_select_bar_clear(1, 21, 37);
    ui_select_bar_clear(1, 22, 37);
    ui_select_bar_clear(1, 23, 37);
    if (phase == P_PICK) {
        ui_footer(1, 26, 38, "A : choisir ce budget   B : retour");
    } else if (phase == P_ACTION) {
        action_list.bar_sel = 0xFF;
        ui_list_draw(&action_list);
        if (rejected) {
            buf[0] = 0;
            strcat(buf, "Rejeté : ");
            cat_num(buf, last_vote.votes_pour, 0);
            strcat(buf, " voix (majorité ");
            cat_num(buf, SEATS_MAJORITY, 0);
            strcat(buf, "). Défiance en hausse.");
            ui_text_wrap(1, 24, 38, 2, buf, UI_STYLE_ALERT);
            ui_text_draw(1, 26, "Sans budget : reconduction (équilibre)", UI_STYLE_DIM);
        } else if (err_cp) {
            ui_text_draw(1, 25, "Capital politique insuffisant", UI_STYLE_ALERT);
        }
    } else {
        ui_icon_draw(9, 21, UI_ICON_CHECK);
        ui_text_center(1, 21, 38, forced ? "BUDGET ADOPTÉ PAR 49.3" : "BUDGET ADOPTÉ", UI_STYLE_HIGHLIGHT);
        ui_text_wrap(1, 22, 38, 3, txt[forced ? TXT_BUDGET_493 : TXT_BUDGET_ADOPTE], UI_STYLE_TEXT);
        ui_footer(1, 26, 38, "A : continuer");
    }
}

static void budget_draw(void)
{
    if (dirty) {
        dirty = FALSE;
        art_reset();
        draw_header();
        ui_window(0, 3, 40, 25);
        ui_medallion(29, 4, UI_PORTRAIT_W, UI_PORTRAIT_H);
        ui_gueule_draw(GUEULE_MIN_ECONOMIE, EXPR_NEUTRE, 29, 4, PAL2);
        budget_list.bar_sel = 0xFF;
        ui_list_draw(&budget_list);
        detail_dirty = TRUE;
        zone_dirty = TRUE;
    }
    if (detail_dirty) {
        detail_dirty = FALSE;
        draw_detail();
    }
    if (zone_dirty) {
        zone_dirty = FALSE;
        draw_zone();
    }
}

static void budget_exit(void)
{
    art_reset();
}

const Screen scr_budget = { budget_init, budget_update, budget_draw, budget_exit };
