// Mega Président — choix du thème de campagne (fin du 1er mandat).
#include <genesis.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_texts.h"
#include "screens/evt_art.h"
#include "ui/ui_art.h"

static UiList list;
static bool dirty;
static bool effects_dirty;
static const char *items[THEME_COUNT];

static const char *const elec_names[ELEC_COUNT] = { "Populaires", "Moyennes", "Aisées" };

static void campaign_init(void)
{
    items[THEME_EMPLOI] = txt[TXT_THEME_EMPLOI];
    items[THEME_SECURITE] = txt[TXT_THEME_SECURITE];
    items[THEME_POUVOIR_ACHAT] = txt[TXT_THEME_POUVOIR_ACHAT];
    items[THEME_ECOLOGIE] = txt[TXT_THEME_ECOLOGIE];
    ui_list_init(&list, 1, 10, 27, 4, items, THEME_COUNT);
    dirty = TRUE;
    effects_dirty = TRUE;
}

static void campaign_update(void)
{
    switch (ui_list_update(&list)) {
    case UI_LIST_MOVED:
        ui_list_draw(&list);
        effects_dirty = TRUE;
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
    return "0";
}

static void campaign_draw(void)
{
    static const UiIcon icons[ELEC_COUNT] = { UI_ICON_SOCIAL, UI_ICON_PERSON, UI_ICON_DEBT };
    const Country *c = &g_game.country;
    u8 e;

    if (dirty) {
        dirty = FALSE;
        art_reset();
        ui_window_title(0, 0, 40, 28, "Fin du mandat · campagne", FALSE);
        ui_medallion(29, 3, UI_PORTRAIT_W, UI_PORTRAIT_H);
        ui_gueule_draw(GUEULE_PRES(c->orientation), EXPR_CONTENT, 29, 3, PAL2);
        ui_text_wrap(1, 4, 27, 4,
                     "La présidentielle approche. Choisissez le thème de votre campagne.",
                     UI_STYLE_TEXT);
        ui_text_draw(1, 8, "Thème de campagne", UI_STYLE_DIM);
        ui_separator(1, 9, 27);
        ui_list_draw(&list);
        ui_text_draw(1, 16, "Effet sur l’électorat", UI_STYLE_HIGHLIGHT);
        ui_separator(23, 16, 16);
        ui_text_center(1, 24, 38, "++ fort   + favorable   - défavorable", UI_STYLE_DIM);
        ui_footer(1, 26, 38, "Haut/Bas : choisir   A : valider");
    }
    if (effects_dirty) {
        effects_dirty = FALSE;
        for (e = 0; e < ELEC_COUNT; e++) {
            UiStyle st;
            s16 b = sim_theme_bonus(c, list.sel, e);
            const char *m = mark(b, &st);
            u16 y = 18 + e * 2;
            ui_icon_draw(1, y, icons[e]);
            ui_text_draw(3, y, elec_names[e], UI_STYLE_DIM);
            ui_text_draw_n(18, y, m, 3, 3, st);
            ui_gauge_draw(22, y, 16, (s16)(b < 0 ? -b : b), 80,
                          st == UI_STYLE_ALERT ? UI_GAUGE_NEGATIVE : UI_GAUGE_GOLD);
        }
    }
}

static void campaign_exit(void)
{
    art_reset();
}

const Screen scr_campaign = { campaign_init, campaign_update, campaign_draw, campaign_exit };
