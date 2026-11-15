// Mega Président — bilan de fin de mandat (tâche 6.3).
#include <genesis.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "audio/sfx.h"
#include "content/content_texts.h"
#include "content/content_world.h"
#include "scr_scores.h"
#include "ui/ui_art.h"
#include <string.h>

static bool dirty;

static const char *const pillar_names[PIL_TOTAL] = {
    "Social", "Économie", "Technologie", "Écologie", "Sécurité", "International"
};

static void end_init(void)
{
    dirty = TRUE;
    if (game_destitue())
        sfx_play(SFX_ELECTION_LOSE);
}

static void end_update(void)
{
    if (input_pressed() & (INPUT_START | INPUT_OK)) {
        if (game_destitue()) {
            screen_change(SCREEN_TITLE);   // jamais de score après une destitution
        } else {
            scores_offer(game_score_first(), game_score_total(), game_two_mandates(),
                         g_game.country.orientation, game_end_year());
            screen_change(SCREEN_SCORES);
        }
    }
}

// Ligne d'évolution : flèche colorée, libellé, « début → fin » et écart (couleur selon
// que la hausse est bonne ou non).
static void draw_evo(u16 y, const char *label, s16 from, s16 to, u16 dec,
                     const char *unit, bool higher_better)
{
    char buf[UI_LINE_BUF], n[14];
    s16 d = (s16)(to - from);
    bool good = (d > 0) == higher_better;
    UiStyle st = d == 0 ? UI_STYLE_DIM : (good ? UI_STYLE_TEXT : UI_STYLE_ALERT);

    if (d != 0)
        ui_icon_draw(1, y, d > 0 ? (good ? UI_ICON_UP_GOOD : UI_ICON_UP_BAD)
                                 : (good ? UI_ICON_DOWN_GOOD : UI_ICON_DOWN_BAD));
    ui_text_draw(3, y, label, UI_STYLE_DIM);
    ui_format_number(buf, from, dec);
    strcat(buf, " → ");
    ui_format_number(n, to, dec);
    strcat(buf, n);
    strcat(buf, unit);
    ui_text_right(32, y, buf, UI_STYLE_TEXT);
    buf[0] = 0;
    if (d > 0) strcat(buf, "+");
    ui_format_number(n, d, dec);
    strcat(buf, n);
    ui_text_right(39, y, buf, st);
}

// Défaite : score du joueur au dernier tour atteint et nom de l'élu (colonne de gauche).
static void draw_defeat(void)
{
    const ElectionResult *e = &g_game.election;
    char buf[UI_LINE_BUF], n[14];
    u8 i, w = e->round2[0] >= e->round2[1] ? 0 : 1;
    s16 score = 0;
    bool second = FALSE;

    for (i = 0; i < 2; i++)
        if (e->finalist[i] == CAND_JOUEUR) {
            second = TRUE;
            score = e->round2[i];
        }
    if (!second)
        for (i = 0; i < ELECTION_CANDIDATES; i++)
            if (e->cand[i] == CAND_JOUEUR)
                score = e->round1[i];
    buf[0] = 0;
    strcat(buf, second ? "Second tour : " : "Premier tour : ");
    ui_format_number(n, score, 1);
    strcat(buf, n);
    strcat(buf, " %");
    ui_text_draw(1, 6, buf, UI_STYLE_TEXT);
    if (e->finalist[w] < CAND_POOL_COUNT) {
        buf[0] = 0;
        strcat(buf, "Élu(e) : ");
        strcat(buf, cand_name[e->finalist[w]]);
        ui_text_draw_n(1, 7, buf, 27, 0, UI_STYLE_DIM);
    }
}

// Ligne « libellé ............ valeur » de la colonne de gauche.
static void stat(u16 y, UiIcon icon, const char *label, s32 v, u16 dec, const char *unit, UiStyle st)
{
    char buf[UI_LINE_BUF];
    ui_icon_draw(1, y, icon);
    ui_text_draw(3, y, label, UI_STYLE_DIM);
    ui_format_number(buf, v, dec);
    strcat(buf, unit);
    ui_text_right(28, y, buf, st);
}

static void end_draw(void)
{
    const Country *c = &g_game.country;
    u16 i, nlaws = 0;
    bool two = game_two_mandates();
    bool dest = game_destitue();

    if (!dirty)
        return;
    dirty = FALSE;
    ui_clear();
    ui_window_title(0, 0, 40, 28, dest ? "Destitution" : (two ? "Bilan des deux mandats" : "Bilan du mandat"),
                    FALSE);
    ui_medallion(29, 3, UI_PORTRAIT_W, UI_PORTRAIT_H);
    ui_gueule_draw(GUEULE_PRES(c->orientation), dest ? EXPR_FURIEUX : (two ? EXPR_CONTENT : EXPR_GENE),
                   29, 3, PAL2);

    for (i = 0; i < LOI_COUNT; i++)
        if (law_mask_get(&c->laws_adopted, (u8)i))
            nlaws++;

    if (dest) {
        char when[UI_LINE_BUF];
        ui_text_flow(1, 4, 27, 4, txt[TXT_DESTITUTION_VOTEE], UI_STYLE_TEXT);
        ui_separator(1, 9, 27);
        stat(10, UI_ICON_SCANDAL, "Scandale", c->gauges[JAUGE_SCANDALE], 0, " %", UI_STYLE_ALERT);
        stat(11, UI_ICON_DEFIANCE, "Défiance", c->gauges[JAUGE_DEFIANCE], 0, " %", UI_STYLE_ALERT);
        stat(12, UI_ICON_POPULARITY, "Popularité", sim_popularity(c), 1, " %", UI_STYLE_TEXT);
        stat(13, UI_ICON_CHECK, "Lois adoptées", nlaws, 0, "", UI_STYLE_TEXT);
        game_turn_label(c->turn ? c->turn - 1 : 0, when);
        ui_icon_draw(1, 16, UI_ICON_CALENDAR);
        ui_text_draw(3, 16, "Chute", UI_STYLE_DIM);
        ui_text_right(39, 16, when, UI_STYLE_TEXT);
        ui_text_center(1, 20, 38, "Fin de partie, aucun score retenu.", UI_STYLE_ALERT);
        ui_text_center(1, 22, 38, "Déménagement par la grille du Coq.", UI_STYLE_DIM);
        ui_footer(1, 26, 38, "START : écran titre");
        return;
    }

    if (two)
        ui_text_flow(1, 4, 27, 5, txt[TXT_FIN_SECOND_MANDAT], UI_STYLE_TEXT);
    else {
        ui_text_wrap(1, 4, 27, 2, "Battu(e) à la présidentielle.", UI_STYLE_ALERT);
        draw_defeat();
    }
    ui_separator(1, 9, 27);
    stat(10, UI_ICON_CP, two ? "Score 1er mandat" : "Score", game_score_first(), 0, "", UI_STYLE_HIGHLIGHT);
    if (two)
        stat(11, UI_ICON_CP, "Score sur 10 ans", game_score_total(), 0, "", UI_STYLE_HIGHLIGHT);
    stat(12, UI_ICON_POPULARITY, "Popularité", sim_popularity(c), 1, " %", UI_STYLE_TEXT);
    stat(13, UI_ICON_CHECK, "Lois adoptées", nlaws, 0, "", UI_STYLE_TEXT);

    ui_text_draw(1, 15, "Économie", UI_STYLE_HIGHLIGHT);
    ui_separator(10, 15, 29);
    draw_evo(16, "Chômage", c->unemployment_start, c->unemployment, 2, " %", FALSE);
    draw_evo(17, "Dette", c->debt_start, c->debt, 1, " %", FALSE);
    ui_text_draw(1, 18, "Piliers", UI_STYLE_HIGHLIGHT);
    ui_separator(9, 18, 30);
    for (i = 0; i < PIL_TOTAL; i++)
        draw_evo(19 + i, pillar_names[i], c->pillars_start[i], c->pillars[i], 1, "", TRUE);

    ui_footer(1, 26, 38, "START : meilleurs scores");
}

static void end_exit(void)
{
    ui_clear();
    ui_art_close();
}

const Screen scr_end = { end_init, end_update, end_draw, end_exit };
