// Mega Président — écran « Les électorats » (V4, tâche 4.07), ouvert par C depuis le
// tableau de bord : 5 jauges, variation du tour, cible de fond et phrase de contexte.
// Dessin uniquement via src/ui.

#include <genesis.h>
#include <string.h>
#include "audio/sfx.h"
#include "screens.h"
#include "scr_txt.h"
#include "scr_help.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_texts.h"

// Opinion de chaque électorat au début du dernier trimestre joué (renseignée par
// scr_game.c quand le joueur termine le tour). Non sauvegardée : après un rechargement,
// la tendance se déduit de la cible de fond.
s16 g_opinion_prev[ELEC_COUNT];
u8  g_opinion_prev_ok;

#define SEUIL_VARIATION 3    // dixièmes de point : en dessous, on dit « stable »
#define SEUIL_CIBLE     5

static u8 sel;
static bool dirty;        // redessin complet
static bool detail_dirty; // bloc du bas

s8 elec_trend(const Country *c, u8 e)
{
    s16 d;
    if (e >= ELEC_COUNT) return 0;
    if (g_opinion_prev_ok) {
        d = (s16)(c->opinion[e] - g_opinion_prev[e]);
        return (s8)(d >= SEUIL_VARIATION ? 1 : (d <= -SEUIL_VARIATION ? -1 : 0));
    }
    d = (s16)(sim_opinion_target(c, e) - c->opinion[e]);
    return (s8)(d >= SEUIL_CIBLE ? 1 : (d <= -SEUIL_CIBLE ? -1 : 0));
}

void elec_trend_icon(u16 x, u16 y, s8 t)
{
    if (t > 0) ui_icon_draw(x, y, UI_ICON_UP_GOOD);
    else if (t < 0) ui_icon_draw(x, y, UI_ICON_DOWN_BAD);
    else ui_text_draw(x, y, "=", UI_STYLE_DIM);
}

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
    char buf[16];
    u16 n;
    ui_banner(0, 0, 40, "Les électorats", TRUE);
    buf[0] = 0;
    cat_num(buf, g_game.country.cp, 0);
    strcat(buf, "/");
    cat_num(buf, CP_MAX, 0);
    n = ui_text_len(buf);
    ui_text_right(39, 1, " CP", UI_STYLE_DIM);
    ui_text_right(36, 1, buf, UI_STYLE_TEXT);
    ui_icon_draw(34 - n, 1, UI_ICON_CP);
}

static void draw_row(u8 e)
{
    const Country *c = &g_game.country;
    char buf[16];
    u16 y = 5 + e * 2;
    s16 o = c->opinion[e];
    s8 t = elec_trend(c, e);
    UiStyle vs = o >= 500 ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT;

    ui_icon_draw(1, y, elec_icon(e));
    ui_text_draw_n(3, y, elec_txt(e, ET_LONG), 18, 18, e == sel ? UI_STYLE_HIGHLIGHT : UI_STYLE_DIM);
    ui_gauge_draw(21, y, 9, o, OPINION_MAX, o >= 500 ? UI_GAUGE_GOLD : UI_GAUGE_NEGATIVE);
    buf[0] = 0;
    cat_num(buf, o, 1);
    strcat(buf, " %");
    ui_clear_rect(30, y, 8, 1);
    ui_text_right(37, y, buf, vs);
    elec_trend_icon(38, y, t);
}

static void draw_rows(void)
{
    for (u8 e = 0; e < ELEC_COUNT; e++) {
        ui_select_bar_clear(1, 5 + e * 2, 38);
        if (e == sel) ui_select_bar(1, 5 + e * 2, 38);
        draw_row(e);
    }
}

static void draw_detail(void)
{
    const Country *c = &g_game.country;
    char buf[UI_LINE_BUF];
    s16 o = c->opinion[sel];
    s16 target = sim_opinion_target(c, sel);
    s8 t = elec_trend(c, sel);
    s16 pop = sim_popularity(c);

    ui_clear_rect(1, 16, 38, 10);
    ui_text_draw_n(1, 16, elec_txt(sel, ET_LONG), 24, 0, UI_STYLE_HIGHLIGHT);
    buf[0] = 0;
    cat_num(buf, elec_weight[sel], 0);
    strcat(buf, " % des voix");
    ui_text_right(39, 16, buf, UI_STYLE_DIM);
    ui_text_wrap(1, 17, 38, 2, elec_txt(sel, ET_QUI), UI_STYLE_DIM);
    // Phrase de contexte : la tendance du tour, sinon le niveau.
    {
        u8 up = t > 0 || (t == 0 && o >= 500);
        ui_text_wrap(1, 19, 38, 2, elec_txt(sel, up ? ET_HAUT : ET_BAS),
                     up ? UI_STYLE_TEXT : UI_STYLE_ALERT);
    }
    if (g_opinion_prev_ok) {
        buf[0] = 0;
        strcat(buf, "Ce trimestre : ");
        cat_signed(buf, c->opinion[sel] - g_opinion_prev[sel], 1);
        strcat(buf, " pt");
        ui_text_draw(1, 21, buf, UI_STYLE_TEXT);
    }
    buf[0] = 0;
    strcat(buf, "Cap : ");
    cat_num(buf, target, 1);
    strcat(buf, " % (tendance de fond)");
    ui_text_draw(1, 22, buf, target >= o ? UI_STYLE_TEXT : UI_STYLE_ALERT);

    ui_icon_draw(1, 24, UI_ICON_POPULARITY);
    ui_text_draw(3, 24, "Popularité", UI_STYLE_DIM);
    ui_gauge_draw(16, 24, 14, pop, 1000, pop >= 500 ? UI_GAUGE_GOLD : UI_GAUGE_NEGATIVE);
    buf[0] = 0;
    cat_num(buf, pop, 1);
    strcat(buf, " %");
    ui_text_right(37, 24, buf, pop >= 500 ? UI_STYLE_HIGHLIGHT : UI_STYLE_ALERT);
}

static void elec_init(void)
{
    sel = 0;
    dirty = TRUE;
}

static void elec_update(void)
{
    u16 r = input_repeat();
    u16 p = input_pressed();

    if (dirty) return;
    if (p & (INPUT_B | INPUT_START)) {
        screen_change(SCREEN_GAME);
        return;
    }
    if (p & INPUT_C) {
        help_open(SCREEN_ELEC);
        return;
    }
    if (r & INPUT_DOWN) {
        sel = (u8)((sel + 1) % ELEC_COUNT);
        sfx_play(SFX_MOVE);
        dirty = TRUE;
    } else if (r & INPUT_UP) {
        sel = (u8)((sel + ELEC_COUNT - 1) % ELEC_COUNT);
        sfx_play(SFX_MOVE);
        dirty = TRUE;
    }
}

static void elec_draw(void)
{
    if (dirty) {
        dirty = FALSE;
        draw_header();
        ui_window(0, 3, 40, 25);
        ui_separator(1, 15, 38);
        draw_rows();
        detail_dirty = TRUE;
        ui_footer(1, 26, 38, "B : retour   C : aide");
    }
    if (detail_dirty) {
        detail_dirty = FALSE;
        draw_detail();
    }
}

const Screen scr_elec = { elec_init, elec_update, elec_draw, NULL };
