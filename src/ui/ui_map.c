// Mega Président — carte de France : seul fichier à toucher VDP/PAL pour la carte.
#include "ui/ui_internal.h"
#include "ui/ui_map.h"
#include "ui/ui_art.h"
#include "ui/input.h"
#include "gfx_ui.h"
#include "gfx_map.h"

#define MAP_PAL      PAL2
#define MAP_PAL_BASE (MAP_PAL * 16)   // indice du premier index de la ligne
#define BLINK_TICKS  96               // demi-période du clignotement : 320 ms (1/300 s)

static const char *const region_names[REGION_COUNT] = {
    "Hauts-de-France", "Normandie", "Île-de-France", "Grand Est", "Bretagne",
    "Pays de la Loire", "Centre-Val de Loire", "Bourgogne-Franche-Comté",
    "Nouvelle-Aquitaine", "Auvergne-Rhône-Alpes", "Occitanie",
    "Provence-Alpes-Côte d'Azur", "Corse"
};

static u16 region_color[REGION_COUNT];   // couleur courante (hors clignotement)
static u8 selected = REGION_NONE;
static u16 blink;          // 1/300 s écoulées dans la période
static bool blink_hi;      // état affiché : TRUE = blanc
static u16 blink_col;      // mot lu par le DMA (doit rester valide jusqu'au VBlank)
static bool map_open;
static bool colorblind;

void ui_map_set_colorblind(bool on)
{
    colorblind = on;
}

static void apply(u8 region, u16 color)
{
    PAL_setColor(MAP_PAL_BASE + 1 + region, color);
}

void ui_map_draw(u16 x, u16 y)
{
    u16 i;
    u16 base = ui_tile_icons + ts_ui_icons.numTile;   // juste après les tuiles de ui.c

    ui_gueule_anim_cover(0, 0, 64, 32);   // la carte réutilise la VRAM des illustrations
    VDP_loadTileSet(&ts_map_france, base, DMA);
    PAL_setPalette(MAP_PAL, pal_map.data, DMA);
    for (i = 0; i < REGION_COUNT; i++)
        region_color[i] = pal_map.data[1 + i];
    selected = REGION_NONE;
    blink = 0;
    blink_hi = TRUE;
    map_open = TRUE;
    VDP_fillTileMapRectInc(BG_B, UI_ATTR(MAP_PAL, base), x, y, UI_MAP_TILES, UI_MAP_TILES);
}

void ui_map_close(void)
{
    if (!map_open)
        return;
    map_open = FALSE;
    selected = REGION_NONE;
    PAL_setPalette(MAP_PAL, pal_ui_dim.data, DMA);
}

void ui_map_clear_region(u8 region)
{
    if (region >= REGION_COUNT)
        return;
    region_color[region] = pal_map.data[1 + region];
    if (region != selected)
        apply(region, region_color[region]);
}

void ui_map_set_region_level(u8 region, s16 value, s16 min, s16 max)
{
    s32 t;   // 0..255
    u16 color;

    if (region >= REGION_COUNT)
        return;
    if (max <= min || value <= min)
        t = 0;
    else if (value >= max)
        t = 255;
    else
        t = ((s32)(value - min) * 255) / (max - min);

    // Trois teintes franches, celles que nomme la légende de la carte (revue DA P1-5) :
    // rouge (7,2,1) sous la moyenne, jaune (7,6,1) autour, vert (1,6,1) au-dessus.
    // Valeurs 0..7 par composante (VDP : 3 bits, 0x0BGR avec bits pairs).
    if (t < 85)
        color = colorblind ? UI_MAP_LOW_CB : UI_MAP_LOW;
    else if (t > 170)
        color = colorblind ? UI_MAP_HIGH_CB : UI_MAP_HIGH;
    else
        color = UI_MAP_MID;
    region_color[region] = color;
    if (region != selected)
        apply(region, color);
}

void ui_map_highlight(u8 region)
{
    if (selected < REGION_COUNT)
        apply(selected, region_color[selected]);
    selected = region < REGION_COUNT ? region : REGION_NONE;
    blink = 0;
    blink_hi = TRUE;
    if (selected != REGION_NONE)
        apply(selected, 0x0EEE);
}

void ui_map_update(void)
{
    bool hi;

    if (selected == REGION_NONE || PAL_isDoingFade())
        return;   // pas de clignotement pendant un fondu (il l'écraserait)
    blink += input_frame_ticks();
    if (blink >= 2 * BLINK_TICKS)
        blink -= 2 * BLINK_TICKS;
    hi = blink < BLINK_TICKS;
    if (hi == blink_hi)
        return;   // palette écrite seulement au changement d'état
    blink_hi = hi;
    blink_col = hi ? 0x0EEE : region_color[selected];
    PAL_setColors(MAP_PAL_BASE + 1 + selected, &blink_col, 1, DMA_QUEUE);
}

const char *ui_map_region_name(u8 region)
{
    return region < REGION_COUNT ? region_names[region] : "";
}
