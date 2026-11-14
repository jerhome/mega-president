// Mega Président — début du second mandat et législatives.
#include <genesis.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_texts.h"
#include "content/content_world.h"
#include "screens/evt_art.h"
#include "ui/ui_art.h"

static bool dirty;

static void mandate2_init(void)
{
    dirty = TRUE;
}

static void mandate2_update(void)
{
    if (input_pressed() & (INPUT_OK | INPUT_START))
        game_mandate2_done();
}

static void mandate2_draw(void)
{
    const LegislativeResult *l = &g_game.legislatives;
    const Country *c = &g_game.country;
    u8 g;
    u16 x;

    if (!dirty)
        return;
    dirty = FALSE;
    art_reset();
    ui_window(0, 0, 40, 28);
    // Portrait du PM d'opposition (cohabitation) ou du PM du joueur. PAL2 empruntée : pas de style DIM.
    if (sim_cohabitation(c))
        ui_art_draw(ART_COHAB(sim_cohab_camp(c)), 31, 1);
    else if (c->pm < PM_COUNT)
        ui_art_draw(ART_PM(c->pm), 31, 1);
    ui_text_center(0, 1, UI_COLS, "SECOND MANDAT", UI_STYLE_HIGHLIGHT);
    ui_text_wrap(1, 3, 30, 5, txt[TXT_SECOND_MANDAT], UI_STYLE_TEXT);

    ui_text_center(0, 8, UI_COLS, "LÉGISLATIVES", UI_STYLE_HIGHLIGHT);
    ui_text_draw(3, 10, "Groupe", UI_STYLE_TEXT);
    ui_text_draw(20, 10, "Avant", UI_STYLE_TEXT);
    ui_text_draw(30, 10, "Après", UI_STYLE_TEXT);
    for (g = 0; g < GRP_COUNT; g++) {
        UiStyle st = (g == c->player_group) ? UI_STYLE_HIGHLIGHT : UI_STYLE_TEXT;
        u16 y = 11 + g;
        ui_text_draw(3, y, grp_sigle[g], st);
        if (sim_in_bloc(c, g))
            ui_text_draw(12, y, "bloc", UI_STYLE_TEXT);
        ui_number_draw(20, y, l->seats_before[g], 0, UI_STYLE_TEXT);
        ui_number_draw(30, y, l->seats_after[g], 0, st);
    }

    x = 3 + ui_text_draw(3, 17, "Bloc présidentiel : ", UI_STYLE_TEXT);
    x += ui_number_draw(x, 17, l->bloc_before, 0, UI_STYLE_TEXT);
    x += ui_text_draw(x, 17, " > ", UI_STYLE_TEXT);
    ui_number_draw(x, 17, l->bloc_after, 0, UI_STYLE_HIGHLIGHT);

    ui_text_draw(3, 18, l->majority ? "Majorité absolue" : "Majorité relative (289 requis)",
                 l->majority ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT);

    ui_text_wrap(2, 20, 36, 3,
                 txt[l->won ? TXT_LEGISLATIVES_GAGNEES : TXT_LEGISLATIVES_PERDUES],
                 UI_STYLE_TEXT);
    if (l->cohab_started || l->cohab_ended)
        ui_text_wrap(1, 23, 38, 3, txt[l->cohab_started ? TXT_COHAB_DEBUT : TXT_COHAB_FIN],
                     l->cohab_started ? UI_STYLE_ALERT : UI_STYLE_HIGHLIGHT);
    ui_text_center(0, 26, UI_COLS, "A : commencer le second mandat", UI_STYLE_TEXT);
}

static void mandate2_exit(void)
{
    art_reset();
}

const Screen scr_mandate2 = { mandate2_init, mandate2_update, mandate2_draw, mandate2_exit };
