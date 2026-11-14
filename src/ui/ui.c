// Mega Président — couche d'interface : base, fenêtres, jauges, icônes.
#include "ui/ui_internal.h"
#include "gfx_ui.h"

u16 ui_tile_font, ui_tile_frame, ui_tile_gauge, ui_tile_icons;

void ui_init(void)
{
    u16 t = TILE_USER_INDEX;

    VDP_setScreenWidth320();
    VDP_setPlaneSize(64, 32, TRUE);

    ui_tile_font = t;
    VDP_loadTileSet(&ts_ui_font, t, DMA);
    t += ts_ui_font.numTile;
    ui_tile_frame = t;
    VDP_loadTileSet(&ts_ui_frame, t, DMA);
    t += ts_ui_frame.numTile;
    ui_tile_gauge = t;
    VDP_loadTileSet(&ts_ui_gauge, t, DMA);
    t += ts_ui_gauge.numTile;
    ui_tile_icons = t;
    VDP_loadTileSet(&ts_ui_icons, t, DMA);

    PAL_setPalette(PAL0, pal_ui_text.data, DMA);
    PAL_setPalette(PAL1, pal_ui_highlight.data, DMA);
    PAL_setPalette(PAL2, pal_ui_dim.data, DMA);
    PAL_setPalette(PAL3, pal_ui_alert.data, DMA);
    VDP_setBackgroundColor(0);   // couleur 0 de pal_ui_text

    ui_clear();
}

void ui_clear(void)
{
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
}

void ui_clear_rect(u16 x, u16 y, u16 w, u16 h)
{
    VDP_clearTileMapRect(BG_A, x, y, w, h);
}

void ui_window(u16 x, u16 y, u16 w, u16 h)
{
    const u16 b = ui_tile_frame;
    const u16 x2 = x + w - 1, y2 = y + h - 1;

    if (w < 2 || h < 2)
        return;
    VDP_setTileMapXY(BG_B, UI_ATTR(PAL0, b + FR_TL), x, y);
    VDP_setTileMapXY(BG_B, UI_ATTR(PAL0, b + FR_TR), x2, y);
    VDP_setTileMapXY(BG_B, UI_ATTR(PAL0, b + FR_BL), x, y2);
    VDP_setTileMapXY(BG_B, UI_ATTR(PAL0, b + FR_BR), x2, y2);
    if (w > 2) {
        VDP_fillTileMapRect(BG_B, UI_ATTR(PAL0, b + FR_T), x + 1, y, w - 2, 1);
        VDP_fillTileMapRect(BG_B, UI_ATTR(PAL0, b + FR_B), x + 1, y2, w - 2, 1);
    }
    if (h > 2) {
        VDP_fillTileMapRect(BG_B, UI_ATTR(PAL0, b + FR_L), x, y + 1, 1, h - 2);
        VDP_fillTileMapRect(BG_B, UI_ATTR(PAL0, b + FR_R), x2, y + 1, 1, h - 2);
    }
    if (w > 2 && h > 2)
        VDP_fillTileMapRect(BG_B, UI_ATTR(PAL0, b + FR_C), x + 1, y + 1, w - 2, h - 2);
    VDP_clearTileMapRect(BG_A, x, y, w, h);
}

void ui_window_erase(u16 x, u16 y, u16 w, u16 h)
{
    VDP_clearTileMapRect(BG_A, x, y, w, h);
    VDP_clearTileMapRect(BG_B, x, y, w, h);
}

void ui_gauge_draw(u16 x, u16 y, u16 w, s16 value, s16 max, UiStyle style)
{
    u16 row[UI_COLS];
    u16 fill, i;

    if (w == 0 || x >= UI_COLS)
        return;
    if (x + w > UI_COLS)
        w = UI_COLS - x;
    if (max <= 0 || value <= 0)
        fill = 0;
    else if (value >= max)
        fill = w * 8;
    else
        fill = (u16)(((s32)value * (w * 8) + max / 2) / max);   // arrondi

    for (i = 0; i < w; i++) {
        u16 k = fill >= 8 ? 8 : fill;
        fill -= k;
        row[i] = UI_ATTR(style, ui_tile_gauge + k);
    }
    VDP_setTileMapDataRow(BG_A, row, y, x, w, CPU);
}

void ui_icon_draw(u16 x, u16 y, UiIcon icon)
{
    VDP_setTileMapXY(BG_A, UI_ATTR(PAL0, ui_tile_icons + icon), x, y);
}

u16 ui_format_number(char *buf, s32 value, u16 decimals)
{
    char tmp[12];
    u32 v;
    u16 n = 0, len = 0;

    if (decimals > 9)
        decimals = 9;
    if (value < 0) {
        buf[len++] = '-';
        v = 0u - (u32)value;
    } else {
        v = (u32)value;
    }
    do {
        tmp[n++] = '0' + (char)(v % 10);
        v /= 10;
    } while (v || n <= decimals);   // au moins un chiffre avant la virgule
    while (n) {
        if (n == decimals)
            buf[len++] = ',';
        buf[len++] = tmp[--n];
    }
    buf[len] = 0;
    return len;
}

u16 ui_number_draw(u16 x, u16 y, s32 value, u16 decimals, UiStyle style)
{
    char buf[14];

    ui_format_number(buf, value, decimals);
    return ui_text_draw(x, y, buf, style);
}
