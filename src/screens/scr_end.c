// Mega Président — bilan de fin de mandat (tâche 6.3).
#include <genesis.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "audio/sfx.h"
#include "content/content_texts.h"
#include "scr_scores.h"

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

// Écrit « début > fin  (+delta) » ; higher_better inverse la couleur du delta.
static void draw_evo(u16 y, const char *label, s16 from, s16 to, u16 dec,
                     const char *unit, bool higher_better)
{
    char buf[14];
    u16 x = 15, n;
    s16 d = (s16)(to - from);
    UiStyle st = UI_STYLE_DIM;

    ui_text_draw(1, y, label, UI_STYLE_TEXT);
    n = ui_format_number(buf, from, dec);
    x += ui_text_draw(x, y, buf, UI_STYLE_DIM);
    x += ui_text_draw(x, y, unit, UI_STYLE_DIM);
    x += ui_text_draw(x, y, " > ", UI_STYLE_DIM);
    ui_format_number(buf, to, dec);
    x += ui_text_draw(x, y, buf, UI_STYLE_HIGHLIGHT);
    x += ui_text_draw(x, y, unit, UI_STYLE_HIGHLIGHT);
    (void)n;
    if (d != 0)
        st = ((d > 0) == higher_better) ? UI_STYLE_TEXT : UI_STYLE_ALERT;
    x += 1;
    if (d > 0)
        ui_text_draw(x, y, "+", st), x++;
    ui_format_number(buf, d, dec);
    ui_text_draw(x, y, buf, st);
}

static void end_draw(void)
{
    const Country *c = &g_game.country;
    char buf[14];
    u16 x, i, nlaws = 0;
    bool two = game_two_mandates();

    if (!dirty)
        return;
    dirty = FALSE;
    ui_clear();
    ui_window(0, 0, 40, 28);

    if (game_destitue()) {
        ui_text_center(0, 8, UI_COLS, "DESTITUTION", UI_STYLE_ALERT);
        ui_text_wrap(2, 11, 36, 5, txt[TXT_DESTITUTION_VOTEE], UI_STYLE_TEXT);
        ui_text_center(0, 18, UI_COLS, "Fin de partie, aucun score retenu.", UI_STYLE_DIM);
        ui_text_center(0, 24, UI_COLS, "Appuyez sur START", UI_STYLE_DIM);
        return;
    }

    ui_text_center(0, 1, UI_COLS, "BILAN DU MANDAT", UI_STYLE_HIGHLIGHT);
    if (two)
        ui_text_wrap(2, 2, 36, 3, txt[TXT_FIN_SECOND_MANDAT], UI_STYLE_TEXT);
    else
        ui_text_center(0, 3, UI_COLS, "Battu(e) à la présidentielle", UI_STYLE_ALERT);

    x = 1 + ui_text_draw(1, 6, two ? "Score du 1er mandat : " : "Score : ", UI_STYLE_TEXT);
    ui_number_draw(x, 6, game_score_first(), 0, UI_STYLE_HIGHLIGHT);
    if (two) {
        x = 1 + ui_text_draw(1, 7, "Score cumulé (10 ans) : ", UI_STYLE_TEXT);
        ui_number_draw(x, 7, game_score_total(), 0, UI_STYLE_HIGHLIGHT);
    }

    ui_text_draw(1, 9, "Popularité finale :", UI_STYLE_TEXT);
    ui_format_number(buf, sim_popularity(c), 1);
    x = 21 + ui_text_draw(21, 9, buf, UI_STYLE_HIGHLIGHT);
    ui_text_draw(x, 9, " %", UI_STYLE_HIGHLIGHT);

    for (i = 0; i < LOI_COUNT; i++)
        if (law_mask_get(&c->laws_adopted, (u8)i))
            nlaws++;
    ui_text_draw(1, 10, "Lois adoptées :", UI_STYLE_TEXT);
    ui_number_draw(21, 10, nlaws, 0, UI_STYLE_HIGHLIGHT);

    ui_text_draw(1, 12, "Économie (début > fin)", UI_STYLE_HIGHLIGHT);
    draw_evo(13, "Chômage", c->unemployment_start, c->unemployment, 2, " %", FALSE);
    draw_evo(14, "Dette", c->debt_start, c->debt, 1, " %", FALSE);

    ui_text_draw(1, 16, "Piliers (début > fin)", UI_STYLE_HIGHLIGHT);
    for (i = 0; i < PIL_TOTAL; i++)
        draw_evo(17 + i, pillar_names[i], c->pillars_start[i], c->pillars[i], 1, "", TRUE);

    ui_text_center(0, 24, UI_COLS, "START : meilleurs scores", UI_STYLE_DIM);
}

const Screen scr_end = { end_init, end_update, end_draw, NULL };
