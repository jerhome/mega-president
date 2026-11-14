// Mega Président — choix du thème de campagne (fin du 1er mandat).
#include <genesis.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_texts.h"

static UiList list;
static bool dirty;
static const char *items[THEME_COUNT];

static const char *const elec_names[ELEC_COUNT] = { "Populaires", "Moyennes", "Aisées" };

static void campaign_init(void)
{
    items[THEME_EMPLOI] = txt[TXT_THEME_EMPLOI];
    items[THEME_SECURITE] = txt[TXT_THEME_SECURITE];
    items[THEME_POUVOIR_ACHAT] = txt[TXT_THEME_POUVOIR_ACHAT];
    items[THEME_ECOLOGIE] = txt[TXT_THEME_ECOLOGIE];
    ui_list_init(&list, 3, 8, 30, 4, items, THEME_COUNT);
    dirty = TRUE;
}

static void campaign_update(void)
{
    switch (ui_list_update(&list)) {
    case UI_LIST_MOVED:
        dirty = TRUE;
        break;
    case UI_LIST_CHOSEN:
        game_campaign_done(list.sel);
        break;
    default:
        if (input_pressed() & INPUT_START)
            game_campaign_done(list.sel);
        break;
    }
}

// Aperçu : « ++ » fort, « + » favorable, « - » défavorable, « 0 » neutre.
static const char *mark(s16 b, UiStyle *st)
{
    *st = UI_STYLE_TEXT;
    if (b >= 40) { *st = UI_STYLE_HIGHLIGHT; return "++"; }
    if (b >= 15) { *st = UI_STYLE_HIGHLIGHT; return "+"; }
    if (b <= -15) { *st = UI_STYLE_ALERT; return "-"; }
    *st = UI_STYLE_DIM;
    return "0";
}

static void campaign_draw(void)
{
    u8 e;

    if (dirty) {
        const Country *c = &g_game.country;
        dirty = FALSE;
        ui_clear();
        ui_window(0, 0, 40, 28);
        ui_text_center(0, 1, UI_COLS, "FIN DU MANDAT", UI_STYLE_HIGHLIGHT);
        ui_text_wrap(2, 3, 36, 3,
                     "La présidentielle approche. Choisissez le thème de votre campagne.",
                     UI_STYLE_TEXT);
        ui_list_draw(&list);
        ui_text_draw(1, 14, "Effet sur l'électorat :", UI_STYLE_HIGHLIGHT);
        for (e = 0; e < ELEC_COUNT; e++) {
            UiStyle st;
            s16 b = sim_theme_bonus(c, list.sel, e);
            const char *m = mark(b, &st);
            ui_text_draw(3, 16 + e * 2, elec_names[e], UI_STYLE_TEXT);
            ui_text_draw_n(20, 16 + e * 2, m, 3, 3, st);
            ui_gauge_draw(24, 16 + e * 2, 12, (s16)(b < 0 ? 0 : b), 80, st == UI_STYLE_ALERT ? UI_GAUGE_NEGATIVE : UI_GAUGE_GOLD);
        }
        ui_text_draw(1, 23, "++ fort  + favorable  - défavorable", UI_STYLE_DIM);
        ui_text_center(0, 26, UI_COLS, "Haut/Bas : choisir  A : valider", UI_STYLE_DIM);
    }
}

const Screen scr_campaign = { campaign_init, campaign_update, campaign_draw, NULL };
