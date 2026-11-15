// Mega Président — carte de la popularité par région.
// La carte emprunte PAL2 (ui_map.h) : les couleurs de PAL2 (style DIM, jauge NEUTRAL)
// y sont celles de la carte. Attention : la légende en daltonisme utilise pourtant
// UI_GAUGE_NEUTRAL (voir le rapport d'audit U4).

#include <genesis.h>
#include <string.h>
#include "screens.h"
#include "scr_map.h"
#include "ui/ui.h"
#include "ui/ui_art.h"
#include "ui/ui_map.h"
#include "save/settings.h"
#include "ui/input.h"
#include "game/game.h"
#include "sim/regions.h"

#define MAP_X     1
#define MAP_Y     4
#define PANEL_X   23
#define PANEL_W   16
#define POP_SPAN  100   // dixièmes de % autour de la moyenne nationale : rouge / vert

static ScreenId return_screen = SCREEN_GAME;
static u8 sel;
static bool dirty;

void map_open(ScreenId return_to)
{
    return_screen = return_to;
    screen_change(SCREEN_MAP);
}

static void paint_regions(void)
{
    s16 nat = sim_popularity(&g_game.country);
    for (u8 r = 0; r < REGION_COUNT; r++)
        ui_map_set_region_level(r, sim_region_popularity(&g_game.country, r),
                                (s16)(nat - POP_SPAN), (s16)(nat + POP_SPAN));
}

static void map_init(void)
{
    sel = REGION_ILE_DE_FRANCE;
    dirty = TRUE;
    ui_clear();
    ui_art_close();   // la carte emprunte PAL2 et la zone de VRAM des illustrations
    ui_window_title(0, 0, 40, 28, "Popularité par région", FALSE);
    ui_map_draw(MAP_X, MAP_Y);
    paint_regions();
    ui_map_highlight(sel);
    // Légende : une pastille de la couleur de la carte, puis son sens (écart à la
    // moyenne nationale : un tiers de ± 10 points pour chaque teinte).
    ui_separator(PANEL_X, 17, PANEL_W);
    // Légende : en daltonisme, la carte passe au bleu (celui de la jauge neutre).
    ui_gauge_draw(PANEL_X, 18, 2, 1, 1, settings_get(SET_COLORS) ? UI_GAUGE_NEUTRAL : UI_GAUGE_POSITIVE);
    ui_text_draw(PANEL_X + 3, 18, "Forte", UI_STYLE_TEXT);
    ui_gauge_draw(PANEL_X, 19, 2, 1, 1, UI_GAUGE_GOLD);
    ui_text_draw(PANEL_X + 3, 19, "Moyenne", UI_STYLE_TEXT);
    ui_gauge_draw(PANEL_X, 20, 2, 1, 1, UI_GAUGE_NEGATIVE);
    ui_text_draw(PANEL_X + 3, 20, "Faible", UI_STYLE_TEXT);
    ui_text_draw(PANEL_X, 22, "Blanc : choisie", UI_STYLE_TEXT);
    ui_footer(1, 26, 38, "Gauche/Droite : région   B : retour");
}

static void map_update(void)
{
    u16 p = input_repeat();
    if (p & (INPUT_RIGHT | INPUT_DOWN)) {
        sel = (u8)((sel + 1) % REGION_COUNT);
        dirty = TRUE;
    } else if (p & (INPUT_LEFT | INPUT_UP)) {
        sel = (u8)((sel + REGION_COUNT - 1) % REGION_COUNT);
        dirty = TRUE;
    }
    if (input_pressed() & INPUT_CANCEL) {
        ui_map_close();
        screen_change(return_screen);
        return;
    }
    if (dirty) ui_map_highlight(sel);
    ui_map_update();
}

static void map_draw(void)
{
    char buf[24], num[12];
    s16 pop, gap;
    u16 y = 4, n;

    if (!dirty) return;
    dirty = FALSE;

    // efface le panneau
    for (u16 i = 3; i < 18; i++)
        ui_text_draw(PANEL_X, i, "                ", UI_STYLE_TEXT);

    pop = sim_region_popularity(&g_game.country, sel);
    gap = (s16)(pop - sim_popularity(&g_game.country));

    n = ui_text_wrap(PANEL_X, y, PANEL_W, 3, ui_map_region_name(sel), UI_STYLE_HIGHLIGHT);
    y += n + 1;

    ui_format_number(num, pop, 1);
    buf[0] = 0;
    strcat(buf, num);
    strcat(buf, " %");
    ui_text_draw(PANEL_X, y++, buf, UI_STYLE_TEXT);
    y++;

    ui_text_draw(PANEL_X, y++, "Moyenne nat.", UI_STYLE_TEXT);
    ui_format_number(num, sim_popularity(&g_game.country), 1);
    buf[0] = 0;
    strcat(buf, num);
    strcat(buf, " %");
    ui_text_draw(PANEL_X, y++, buf, UI_STYLE_TEXT);
    y++;

    buf[0] = 0;
    strcat(buf, "Écart : ");
    if (gap > 0) strcat(buf, "+");
    ui_format_number(num, gap, 1);
    strcat(buf, num);
    ui_text_draw(PANEL_X, y, buf, gap < 0 ? UI_STYLE_ALERT : UI_STYLE_HIGHLIGHT);
}

const Screen scr_map = { map_init, map_update, map_draw, NULL };
