// Mega Président — bilan de fin de mandat (tâche 6.3).
#include <genesis.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"

static bool dirty;

static const char *const pillar_names[PIL_COUNT] = {
    "Social", "Économie", "Technologie", "Écologie", "Sécurité"
};

static void end_init(void)
{
    dirty = TRUE;
}

static void end_update(void)
{
    if (input_pressed() & (INPUT_START | INPUT_OK))
        screen_change(SCREEN_TITLE);
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

    if (!dirty)
        return;
    dirty = FALSE;
    ui_clear();
    ui_window(0, 0, 40, 28);

    ui_text_center(0, 1, UI_COLS, "BILAN DU MANDAT", UI_STYLE_HIGHLIGHT);
    ui_text_center(0, 3, UI_COLS,
                   g_game.election.elected ? "Réélu(e)" : "Battu(e) à la présidentielle",
                   g_game.election.elected ? UI_STYLE_TEXT : UI_STYLE_ALERT);

    x = 1 + ui_text_draw(1, 5, "Score : ", UI_STYLE_TEXT);
    ui_number_draw(x, 5, g_game.score, 0, UI_STYLE_HIGHLIGHT);

    ui_text_draw(1, 7, "Popularité finale :", UI_STYLE_TEXT);
    ui_format_number(buf, sim_popularity(c), 1);
    x = 21 + ui_text_draw(21, 7, buf, UI_STYLE_HIGHLIGHT);
    ui_text_draw(x, 7, " %", UI_STYLE_HIGHLIGHT);

    for (i = 0; i < 16; i++)
        if (c->laws_adopted & (1u << i))
            nlaws++;
    ui_text_draw(1, 8, "Lois adoptées :", UI_STYLE_TEXT);
    ui_number_draw(21, 8, nlaws, 0, UI_STYLE_HIGHLIGHT);

    ui_text_draw(1, 10, "Économie (début > fin)", UI_STYLE_HIGHLIGHT);
    draw_evo(11, "Chômage", c->unemployment_start, c->unemployment, 2, " %", FALSE);
    draw_evo(12, "Dette", c->debt_start, c->debt, 1, " %", FALSE);

    ui_text_draw(1, 14, "Piliers (début > fin)", UI_STYLE_HIGHLIGHT);
    for (i = 0; i < PIL_COUNT; i++)
        draw_evo(15 + i, pillar_names[i], c->pillars_start[i], c->pillars[i], 1, "", TRUE);

    ui_text_center(0, 24, UI_COLS, "Appuyez sur START", UI_STYLE_DIM);
}

const Screen scr_end = { end_init, end_update, end_draw, NULL };
