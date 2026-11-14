// Mega Président — journal du trimestre écoulé (tâche 6.2).

#include <genesis.h>
#include <string.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_laws.h"

#define START_YEAR 2027
#define LAST_ROW   25   // dernière ligne utilisable pour le contenu

// Instantané pris par scr_game.c au moment de la fin du trimestre.
extern s16 g_report_snap[9];

static bool dirty;

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

// Ligne d'indicateur : flèche, libellé, « avant -> après ».
static u16 indicator(u16 y, const char *label, s16 before, s16 after, u16 dec, const char *unit)
{
    char buf[40];
    if (before == after || y > LAST_ROW) return y;
    ui_icon_draw(1, y, after > before ? UI_ICON_ARROW_UP : UI_ICON_ARROW_DOWN);
    ui_text_draw(3, y, label, UI_STYLE_TEXT);
    buf[0] = 0;
    cat_num(buf, before, dec);
    strcat(buf, " -> ");
    cat_num(buf, after, dec);
    strcat(buf, unit);
    ui_text_draw(38 - ui_text_len(buf), y, buf, UI_STYLE_HIGHLIGHT);
    return y + 1;
}

static void report_init(void)
{
    dirty = TRUE;
}

static void report_update(void)
{
    if (input_pressed() & (INPUT_OK | INPUT_START))
        game_after_report();
}

static void report_draw(void)
{
    static const char *const pillar_names[PIL_COUNT] = {
        "Social", "Économie", "Technologie", "Écologie", "Sécurité"
    };
    const Country *c = &g_game.country;
    const TurnReport *r = &g_game.report;
    char buf[40];
    u16 y, n;
    u8 t;

    if (!dirty) return;
    dirty = FALSE;

    ui_clear();
    ui_window(0, 0, 40, 28);
    ui_text_center(1, 1, 38, "LE QUOTIDIEN DU MATIN", UI_STYLE_HIGHLIGHT);

    t = c->turn > 0 ? c->turn - 1 : 0;   // trimestre qui vient de s'écouler
    buf[0] = 0;
    strcat(buf, "Bilan du trimestre T");
    cat_num(buf, (t & 3) + 1, 0);
    strcat(buf, " ");
    cat_num(buf, START_YEAR + (t >> 2), 0);
    ui_text_center(1, 2, 38, buf, UI_STYLE_DIM);

    // Popularité et capital politique
    y = 4;
    buf[0] = 0;
    strcat(buf, "Popularité : ");
    cat_num(buf, r->popularity_before, 1);
    strcat(buf, " % -> ");
    cat_num(buf, r->popularity_after, 1);
    strcat(buf, " % (");
    cat_signed(buf, r->popularity_after - r->popularity_before, 1);
    strcat(buf, ")");
    ui_text_draw(1, y++, buf, r->popularity_after >= r->popularity_before ? UI_STYLE_TEXT : UI_STYLE_ALERT);
    buf[0] = 0;
    strcat(buf, "Capital politique : +");
    cat_num(buf, r->cp_gain, 0);
    strcat(buf, " (total ");
    cat_num(buf, c->cp, 0);
    strcat(buf, ")");
    ui_text_draw(1, y++, buf, UI_STYLE_TEXT);
    y++;

    // À la une : lois votées ce trimestre
    if (g_game.laws_tried) {
        ui_text_draw(1, y++, "À la une", UI_STYLE_HIGHLIGHT);
        for (u16 i = 0; i < LOI_COUNT && y <= LAST_ROW; i++) {
            if (!(g_game.laws_tried & (1 << i))) continue;
            ui_icon_draw(1, y, UI_ICON_BULLET);
            n = ui_text_wrap(3, y, 35, LAST_ROW + 1 - y > 2 ? 2 : LAST_ROW + 1 - y,
                             (g_game.laws_adopted & (1 << i)) ? law_journal_ok[i] : law_journal_ko[i],
                             (g_game.laws_adopted & (1 << i)) ? UI_STYLE_TEXT : UI_STYLE_DIM);
            y += n ? n : 1;
        }
        y++;
    }

    // Lois dont les effets arrivent
    if (r->n_matured && y <= LAST_ROW) {
        ui_text_draw(1, y++, "Entrent en vigueur", UI_STYLE_HIGHLIGHT);
        for (u16 i = 0; i < r->n_matured && y <= LAST_ROW; i++) {
            ui_icon_draw(1, y, UI_ICON_BULLET);
            ui_text_draw_n(3, y++, law_name[r->matured[i]], 35, 0, UI_STYLE_TEXT);
        }
        y++;
    }

    // Indicateurs qui ont bougé
    if (y <= LAST_ROW) {
        u16 y0 = y;
        ui_text_draw(1, y++, "Indicateurs", UI_STYLE_HIGHLIGHT);
        y = indicator(y, "Croissance", g_report_snap[0], c->growth, 2, " %");
        y = indicator(y, "Chômage", g_report_snap[1], c->unemployment, 2, " %");
        y = indicator(y, "Dette", g_report_snap[2], c->debt, 1, " %");
        for (u16 i = 0; i < PIL_COUNT; i++)
            y = indicator(y, pillar_names[i], g_report_snap[3 + i], c->pillars[i], 1, "");
        y = indicator(y, "Tension", g_report_snap[8], c->gauges[JAUGE_TENSION], 0, " %");
        if (y == y0 + 1)
            ui_text_draw(1, y, "Rien de notable.", UI_STYLE_DIM);
    }

    ui_text_draw(1, 26, "A : continuer", UI_STYLE_DIM);
}

const Screen scr_report = { report_init, report_update, report_draw, NULL };
