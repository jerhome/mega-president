// Mega Président — couche d'interface : base, fenêtres, bandeaux, jauges,
// icônes, fondus (charte V3 : docs/CHARTE_V3.md).
#include "ui/ui_internal.h"
#include "ui/input.h"
#include "ui/ui_art.h"
#include "ui/ui_map.h"
#include "gfx_ui.h"
#include "util/strbuf.h"

u16 ui_tile_font, ui_tile_frame, ui_tile_gauge, ui_tile_icons;

_Static_assert(UI_ICON_LAST == UI_ICON_COUNT, "UiIcon et tools/ui/make_ui.py (ICONS) divergent");

// --- Fondus et animations ---------------------------------------------------------

typedef enum { FX_NONE, FX_OUT, FX_HIDDEN, FX_IN, FX_SOFT } FxState;

static FxState fx;
static bool soft_req;
static u16 pal_target[64];
static u16 pal_from[64];
static u16 anim_ticks;     // 1/300 s, pour le curseur animé
static u8 cursor_frame;
static u8 fade_speed;      // réglage « Transitions » : 0 douces, 1 rapides, 2 aucune
static bool colorblind;    // réglage « Couleurs » : daltonisme

// Durée d'un fondu en images selon le réglage (moitié en « Rapides »).
static u16 ms_frames(u16 ms)
{
    u16 f;
    if (fade_speed == 1)
        ms /= 2;
    f = (u16)((u32)ms * (IS_PAL_SYSTEM ? 50 : 60) / 1000);
    return f ? f : 1;
}

void ui_set_fade_speed(u8 s)
{
    fade_speed = s > 2 ? 2 : s;
}

// Daltonisme : jauge A et flèches favorables (PAL0 6-7) en turquoise au lieu du vert.
#define CB_COL6 RGB3_3_3_TO_VDPCOLOR(0, 4, 6)
#define CB_COL7 RGB3_3_3_TO_VDPCOLOR(3, 7, 7)

static void colorblind_apply(void)
{
    u16 c[2];
    if (colorblind) {
        c[0] = CB_COL6;
        c[1] = CB_COL7;
    } else {
        c[0] = pal_ui_text.data[6];
        c[1] = pal_ui_text.data[7];
    }
    PAL_setColors(6, c, 2, DMA);
    // Fondu en cours vers les anciennes couleurs : sa cible change aussi.
    pal_target[6] = c[0];
    pal_target[7] = c[1];
}

void ui_set_colorblind(bool on)
{
    colorblind = on;
    colorblind_apply();
    ui_map_set_colorblind(on);
}

void ui_soft_replay(void)
{
    if (fade_speed == 2 || fx != FX_NONE)
        return;
    soft_req = TRUE;
}

void ui_soft_cancel(void)
{
    soft_req = FALSE;
}

void ui_fade_out_start(void)
{
    ui_gueule_anim_stop();   // visage au repos avant le fondu au noir
    soft_req = FALSE;
    if (PAL_isDoingFade())
        PAL_interruptFade();
    if (fade_speed == 2) {   // « Aucune » : changement d'écran sans fondu
        fx = FX_NONE;
        return;
    }
    PAL_fadeOut(0, 63, ms_frames(UI_FADE_OUT_MS), TRUE);
    fx = FX_OUT;
    input_block(INPUT_BLOCK_DISCARD);
}

bool ui_fade_busy(void)
{
    return PAL_isDoingFade();
}

bool ui_anim_allowed(void)
{
    return fx == FX_NONE && !soft_req && !PAL_isDoingFade();
}

void ui_palettes_reset(void)
{
    PAL_setPalette(PAL0, pal_ui_text.data, DMA);
    PAL_setPalette(PAL1, pal_ui_highlight.data, DMA);
    PAL_setPalette(PAL2, pal_ui_dim.data, DMA);
    PAL_setPalette(PAL3, pal_ui_alert.data, DMA);
    if (colorblind)
        colorblind_apply();
}

void ui_transition_hide(void)
{
    if (PAL_isDoingFade())
        PAL_interruptFade();
    VDP_setBackgroundColor(1);   // affichage coupé : couleur de fond = PAL0[1] (noir)
    VDP_setEnable(FALSE);
    fx = FX_HIDDEN;
    input_block(INPUT_BLOCK_DISCARD);
}

void ui_transition_show(void)
{
    if (fade_speed == 2) {   // « Aucune » : l'écran apparaît d'un coup, déjà dessiné
        VDP_setBackgroundColor(0);
        VDP_setEnable(TRUE);
        fx = FX_NONE;
        input_block(INPUT_BLOCK_OFF);
        soft_req = FALSE;
        return;
    }
    // Palettes voulues par le nouvel écran (écrites en CRAM par init / draw).
    PAL_getColors(0, pal_target, 64);
    PAL_setColors(0, palette_black, 64, CPU);
    VDP_setBackgroundColor(0);
    VDP_setEnable(TRUE);
    PAL_fade(0, 63, palette_black, pal_target, ms_frames(UI_FADE_IN_MS), TRUE);
    fx = FX_IN;
    input_block(INPUT_BLOCK_DEFER);
    soft_req = FALSE;
}

// Couleur au tiers de sa luminosité (composantes VDP sur 3 bits).
static u16 dim_color(u16 c)
{
    u16 r = (c >> 1) & 7, g = (c >> 5) & 7, b = (c >> 9) & 7;
    r = (r + 1) / 3;
    g = (g + 1) / 3;
    b = (b + 1) / 3;
    return (u16)((r << 1) | (g << 5) | (b << 9));
}

static void soft_start(void)
{
    u16 i;

    PAL_getColors(0, pal_target, 64);
    for (i = 0; i < 64; i++)
        pal_from[i] = dim_color(pal_target[i]);
    PAL_setColors(0, pal_from, 64, CPU);   // le reste de l'image s'affiche déjà atténué
    PAL_fade(0, 63, pal_from, pal_target, ms_frames(UI_SOFT_MS), TRUE);
    fx = FX_SOFT;
    input_block(INPUT_BLOCK_DEFER);
}

void ui_frame_end(void)
{
    u8 f;

    switch (fx) {
    case FX_HIDDEN:
        ui_transition_show();
        break;
    case FX_IN:
    case FX_SOFT:
        if (!PAL_isDoingFade()) {
            // Les pas du fondu SGDK arrondissent : on pose la palette exacte.
            PAL_setColors(0, pal_target, 64, DMA_QUEUE);
            fx = FX_NONE;
            input_block(INPUT_BLOCK_OFF);
        }
        break;
    default:
        break;
    }
    if (soft_req) {
        soft_req = FALSE;
        if (fx == FX_NONE && fade_speed != 2)
            soft_start();
    }

    // Curseur animé : 4 images, 100 ms chacune (30/300 s).
    anim_ticks += input_frame_ticks();
    if (anim_ticks >= 30 * 4)
        anim_ticks -= 30 * 4;
    f = (u8)(anim_ticks / 30);
    if (f != cursor_frame) {
        cursor_frame = f;
        VDP_loadTileData(ts_ui_cursor.tiles + 8 * f, ui_tile_icons + UI_ICON_CURSOR, 1, DMA_QUEUE);
    }
}

// --- Base ---------------------------------------------------------------------------

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
    ui_tile_icons = t;                       // en dernier (voir ui_internal.h)
    VDP_loadTileSet(&ts_ui_icons, t, DMA);

    PAL_setPalette(PAL0, pal_ui_text.data, DMA);
    PAL_setPalette(PAL1, pal_ui_highlight.data, DMA);
    PAL_setPalette(PAL2, pal_ui_dim.data, DMA);
    PAL_setPalette(PAL3, pal_ui_alert.data, DMA);
    VDP_setBackgroundColor(0);   // couleur 0 de PAL0 : fond d'écran
    fx = FX_NONE;

    ui_clear();
    soft_req = FALSE;
}

u16 ui_bg_attr(u16 y)
{
    // 4 bandes de grain, de plus en plus dense vers le bas.
    u16 band = (u16)((y * 4) / UI_ROWS);
    return UI_FRAME_ATTR(UI_T_BG0 + (band > 3 ? 3 : band));
}

void ui_bg_rect(u16 x, u16 y, u16 w, u16 h)
{
    u16 j;

    if (x >= UI_COLS || y >= UI_ROWS)
        return;
    if (x + w > UI_COLS)
        w = UI_COLS - x;
    if (y + h > UI_ROWS)
        h = UI_ROWS - y;
    ui_gueule_anim_cover(x, y, w, h);
    for (j = 0; j < h; j++)
        VDP_fillTileMapRect(BG_B, ui_bg_attr(y + j), x, y + j, w, 1);
}

void ui_clear(void)
{
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    ui_bg_rect(0, 0, UI_COLS, UI_ROWS);
    soft_req = TRUE;
}

void ui_clear_quiet(void)
{
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    ui_bg_rect(0, 0, UI_COLS, UI_ROWS);
}

void ui_clear_rect(u16 x, u16 y, u16 w, u16 h)
{
    VDP_clearTileMapRect(BG_A, x, y, w, h);
}

// --- Fenêtres et décor --------------------------------------------------------------

// Rangée de décor : bout gauche, milieu répété, bout droit.
static void deco_row(u16 x, u16 y, u16 w, u16 tl, u16 tm, u16 tr)
{
    if (w == 0)
        return;
    ui_gueule_anim_cover(x, y, w, 1);
    if (w == 1) {
        VDP_setTileMapXY(BG_B, UI_FRAME_ATTR(tm), x, y);
        return;
    }
    VDP_setTileMapXY(BG_B, UI_FRAME_ATTR(tl), x, y);
    if (w > 2)
        VDP_fillTileMapRect(BG_B, UI_FRAME_ATTR(tm), x + 1, y, w - 2, 1);
    VDP_setTileMapXY(BG_B, UI_FRAME_ATTR(tr), x + w - 1, y);
}

// Corps d'une fenêtre de la rangée y0 (côtés) à la rangée du bas y + h - 1.
static void window_body(u16 x, u16 y0, u16 w, u16 y_bottom)
{
    if (y_bottom > y0) {
        u16 n = y_bottom - y0;
        ui_gueule_anim_cover(x, y0, w, n);
        VDP_fillTileMapRect(BG_B, UI_FRAME_ATTR(UI_T_WIN_L), x, y0, 1, n);
        if (w > 2)
            VDP_fillTileMapRect(BG_B, UI_FRAME_ATTR(UI_T_WIN_C), x + 1, y0, w - 2, n);
        VDP_fillTileMapRect(BG_B, UI_FRAME_ATTR(UI_T_WIN_R), x + w - 1, y0, 1, n);
    }
    deco_row(x, y_bottom, w, UI_T_WIN_BL, UI_T_WIN_B, UI_T_WIN_BR);
}

void ui_window(u16 x, u16 y, u16 w, u16 h)
{
    if (w < 2 || h < 2)
        return;
    deco_row(x, y, w, UI_T_WIN_TL, UI_T_WIN_T, UI_T_WIN_TR);
    window_body(x, y + 1, w, y + h - 1);
    VDP_clearTileMapRect(BG_A, x, y, w, h);
}

void ui_window_erase(u16 x, u16 y, u16 w, u16 h)
{
    VDP_clearTileMapRect(BG_A, x, y, w, h);
    ui_bg_rect(x, y, w, h);
}

static void banner_title(u16 x, u16 y, u16 w, const char *title, bool left)
{
    if (!title)
        return;
    if (left)
        ui_text_draw_n(x + 1, y + 1, title, w - 2, 0, UI_STYLE_HIGHLIGHT);
    else
        ui_text_center(x + 1, y + 1, w - 2, title, UI_STYLE_HIGHLIGHT);
}

void ui_window_title(u16 x, u16 y, u16 w, u16 h, const char *title, bool left)
{
    if (w < 2 || h < 4)
        return;
    deco_row(x, y, w, UI_T_HD_TL, UI_T_HD_T, UI_T_HD_TR);
    deco_row(x, y + 1, w, UI_T_HD_L, UI_T_HD_M, UI_T_HD_R);
    deco_row(x, y + 2, w, UI_T_HD_SL, UI_T_HD_S, UI_T_HD_SR);
    window_body(x, y + 3, w, y + h - 1);
    VDP_clearTileMapRect(BG_A, x, y, w, h);
    banner_title(x, y, w, title, left);
}

void ui_banner(u16 x, u16 y, u16 w, const char *title, bool left)
{
    if (w < 2)
        return;
    deco_row(x, y, w, UI_T_HD_TL, UI_T_HD_T, UI_T_HD_TR);
    deco_row(x, y + 1, w, UI_T_HD_L, UI_T_HD_M, UI_T_HD_R);
    deco_row(x, y + 2, w, UI_T_BN_BL, UI_T_BN_B, UI_T_BN_BR);
    VDP_clearTileMapRect(BG_A, x, y, w, 3);
    banner_title(x, y, w, title, left);
}

void ui_separator(u16 x, u16 y, u16 w)
{
    deco_row(x, y, w, UI_T_SEP_L, UI_T_SEP_M, UI_T_SEP_R);
    VDP_clearTileMapRect(BG_A, x, y, w, 1);
}

// Rangée du cadre de portrait sur le plan A.
static void ring_row(u16 x, u16 y, u16 w, u16 tl, u16 tm, u16 tr)
{
    VDP_setTileMapXY(BG_A, UI_FRAME_ATTR(tl), x, y);
    if (w > 2)
        VDP_fillTileMapRect(BG_A, UI_FRAME_ATTR(tm), x + 1, y, w - 2, 1);
    VDP_setTileMapXY(BG_A, UI_FRAME_ATTR(tr), x + w - 1, y);
}

void ui_medallion(u16 x, u16 y, u16 w, u16 h)
{
    if (w < 2 || h < 2)
        return;
    // Fond du portrait (plan B, recouvert par l'illustration) et cadre doré sur le
    // bord du rectangle (plan A : seules les marges des tuiles sont opaques).
    ui_gueule_anim_cover(x, y, w, h);
    VDP_fillTileMapRect(BG_B, UI_FRAME_ATTR(UI_T_PF_C), x, y, w, h);
    VDP_clearTileMapRect(BG_A, x, y, w, h);
    ring_row(x, y, w, UI_T_PF_TL, UI_T_PF_T, UI_T_PF_TR);
    if (h > 2) {
        VDP_fillTileMapRect(BG_A, UI_FRAME_ATTR(UI_T_PF_L), x, y + 1, 1, h - 2);
        VDP_fillTileMapRect(BG_A, UI_FRAME_ATTR(UI_T_PF_R), x + w - 1, y + 1, 1, h - 2);
    }
    ring_row(x, y + h - 1, w, UI_T_PF_BL, UI_T_PF_B, UI_T_PF_BR);
}

void ui_select_bar(u16 x, u16 y, u16 w)
{
    deco_row(x, y, w, UI_T_SEL_L, UI_T_SEL_M, UI_T_SEL_R);
}

void ui_select_bar_clear(u16 x, u16 y, u16 w)
{
    if (w) {
        ui_gueule_anim_cover(x, y, w, 1);
        VDP_fillTileMapRect(BG_B, UI_FRAME_ATTR(UI_T_WIN_C), x, y, w, 1);
    }
}

// --- Aide des boutons ----------------------------------------------------------------

// Bouton au début de s (« A : », « B : », « C : », « START : ») : icône, sinon UI_ICON_LAST.
static UiIcon button_at(const char *s, u16 *skip)
{
    static const char start[] = "START : ";
    u16 i;

    if ((s[0] == 'A' || s[0] == 'B' || s[0] == 'C') && s[1] == ' ' && s[2] == ':' && s[3] == ' ') {
        *skip = 4;
        return (UiIcon)(UI_ICON_BTN_A + (s[0] - 'A'));
    }
    for (i = 0; start[i]; i++)
        if (s[i] != start[i])
            return UI_ICON_LAST;
    *skip = sizeof(start) - 1;
    return UI_ICON_BTN_START;
}

// Parcourt hints : dessine si draw, renvoie la largeur en colonnes.
static u16 footer_pass(u16 x, u16 y, const char *hints, bool draw, u16 limit)
{
    char seg[UI_LINE_BUF];
    u16 n = 0, cols = 0, skip = 0;
    const char *s = hints;
    bool word_start = TRUE;

    for (;;) {
        UiIcon ic = (*s && word_start) ? button_at(s, &skip) : UI_ICON_LAST;
        if (*s == 0 || ic != UI_ICON_LAST) {
            if (n) {
                seg[n] = 0;
                if (draw && cols < limit)
                    ui_text_draw_n(x + cols, y, seg, limit - cols, 0, UI_STYLE_DIM);
                cols += ui_text_len(seg);
                n = 0;
            }
            if (*s == 0)
                return cols;
            if (draw && cols < limit)
                ui_icon_draw(x + cols, y, ic);
            cols += 2;            // icône + espace
            s += skip;
            word_start = FALSE;
            continue;
        }
        word_start = (*s == ' ');
        if (n < sizeof(seg) - 1)
            seg[n++] = *s;
        s++;
    }
}

void ui_footer(u16 x, u16 y, u16 w, const char *hints)
{
    u16 cols = footer_pass(0, y, hints, FALSE, 0xFFFF);

    VDP_clearTileMapRect(BG_A, x, y, w, 1);
    if (cols > w)
        cols = w;   // trop long : tronqué à droite, jamais au-delà de x + w (I7)
    footer_pass(x + (w - cols) / 2, y, hints, TRUE, cols);
}

// --- Jauges et icônes ----------------------------------------------------------------

void ui_gauge_draw(u16 x, u16 y, u16 w, s16 value, s16 max, UiStyle style)
{
    u16 row[UI_COLS];
    u16 fill, i, set = (style >> 1) & 1;

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
        u16 part = (w == 1) ? 1 : (i == 0 ? 0 : (i == w - 1 ? 2 : 1));
        fill -= k;
        row[i] = UI_ATTR(UI_STYLE_PAL(style), ui_tile_gauge + UI_GAUGE_TILE(set, part, k));
    }
    VDP_setTileMapDataRow(BG_A, row, y, x, w, CPU);
}

void ui_icon_draw(u16 x, u16 y, UiIcon icon)
{
    if (icon >= UI_ICON_LAST)
        return;
    VDP_setTileMapXY(BG_A, UI_ATTR(ui_icon_pal[icon], ui_tile_icons + icon), x, y);
}

u16 ui_format_number(char *buf, s32 value, u16 decimals)
{
    StrBuf sb;

    sb_init(&sb, buf, 14);   // pire cas : signe + 10 chiffres + virgule + 0
    sb_num(&sb, value, decimals);
    return sb.len;
}

u16 ui_number_draw(u16 x, u16 y, s32 value, u16 decimals, UiStyle style)
{
    char buf[14];

    ui_format_number(buf, value, decimals);
    return ui_text_draw(x, y, buf, style);
}
