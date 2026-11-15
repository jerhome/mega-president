// Mega Président — carte de France : seul fichier à toucher VDP/PAL pour la carte.
#include "ui/ui_internal.h"
#include "ui/ui_map.h"
#include "gfx_ui.h"
#include "gfx_map.h"

#define MAP_PAL      PAL2
#define MAP_PAL_BASE (MAP_PAL * 16)   // indice du premier index de la ligne
#define BLINK_FRAMES 16               // demi-période du clignotement (images)

static const char *const region_names[REGION_COUNT] = {
    "Hauts-de-France", "Normandie", "Île-de-France", "Grand Est", "Bretagne",
    "Pays de la Loire", "Centre-Val de Loire", "Bourgogne-Franche-Comté",
    "Nouvelle-Aquitaine", "Auvergne-Rhône-Alpes", "Occitanie",
    "Provence-Alpes-Côte d'Azur", "Corse"
};

static u16 region_color[REGION_COUNT];   // couleur courante (hors clignotement)
static u8 selected = REGION_NONE;
static u8 blink;
static bool map_open;

static void apply(u8 region, u16 color)
{
    PAL_setColor(MAP_PAL_BASE + 1 + region, color);
}

void ui_map_draw(u16 x, u16 y)
{
    u16 i;
    u16 base = ui_tile_icons + ts_ui_icons.numTile;   // juste après les tuiles de ui.c

    VDP_loadTileSet(&ts_map_france, base, DMA);
    PAL_setPalette(MAP_PAL, pal_map.data, DMA);
    for (i = 0; i < REGION_COUNT; i++)
        region_color[i] = pal_map.data[1 + i];
    selected = REGION_NONE;
    blink = 0;
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
    u16 r, g, color;

    if (region >= REGION_COUNT)
        return;
    if (max <= min || value <= min)
        t = 0;
    else if (value >= max)
        t = 255;
    else
        t = ((s32)(value - min) * 255) / (max - min);

    // Valeurs 0..7 par composante (VDP : 3 bits, 0x0BGR avec bits pairs).
    // rouge (7,1,1) -> jaune (7,6,1) -> vert (1,6,1)
    if (t < 128) {
        r = 7;
        g = 1 + (u16)((t * 5) / 128);
    } else {
        g = 6;
        r = 7 - (u16)(((t - 128) * 6) / 127);
    }
    color = (u16)((r << 1) | (g << 5) | (1 << 9));
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
    if (selected != REGION_NONE)
        apply(selected, 0x0EEE);
}

void ui_map_update(void)
{
    if (selected == REGION_NONE || PAL_isDoingFade())
        return;   // pas de clignotement pendant un fondu (il l'écraserait)
    blink++;
    if (blink >= 2 * BLINK_FRAMES)
        blink = 0;
    apply(selected, blink < BLINK_FRAMES ? 0x0EEE : region_color[selected]);
}

const char *ui_map_region_name(u8 region)
{
    return region < REGION_COUNT ? region_names[region] : "";
}
