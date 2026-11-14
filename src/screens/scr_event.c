// Mega Président — événement du trimestre : texte, choix, ligne de journal.

#include "audio/sfx.h"
#include <genesis.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_events.h"
#include "screens/evt_art.h"
#include "ui/ui_art.h"

#define VIGNETTE_X 30   // vignette 8 x 6 tuiles, en bas à droite (sous la liste)
#define VIGNETTE_Y 20

typedef enum { P_CHOOSE, P_JOURNAL } Phase;

static Phase phase;
static bool dirty;
static u8 evt;        // événement affiché (g_game.event est remis à -1 par game_event_chosen)
static u8 chosen;
static UiList choice_list;
static const char *choice_items[EVENT_MAX_CHOICES];

static void event_init(void)
{
    sfx_play(SFX_EVENT);
    const EventDef *def;
    u16 n;

    evt = g_game.event >= 0 ? (u8)g_game.event : 0;
    def = event_get(evt);
    n = def ? def->n_choices : 2;
    for (u16 i = 0; i < n; i++)
        choice_items[i] = evt_choice[evt][i];
    ui_list_init(&choice_list, 1, 15, 38, n, choice_items, n);
    phase = P_CHOOSE;
    dirty = TRUE;
}

static void event_update(void)
{
    if (dirty) return;
    if (phase == P_CHOOSE) {
        UiListResult r = ui_list_update(&choice_list);
        if (r == UI_LIST_MOVED) {
            ui_list_draw(&choice_list);
        } else if (r == UI_LIST_CHOSEN) {
            chosen = choice_list.sel;
            phase = P_JOURNAL;
            dirty = TRUE;
        }
    } else if (input_pressed() & (INPUT_OK | INPUT_START)) {
        game_event_chosen(chosen);
    }
}

static void event_draw(void)
{
    if (!dirty) return;
    dirty = FALSE;

    if (phase == P_CHOOSE) {
        art_reset();
        ui_window(0, 0, 40, 3);
        ui_text_center(1, 1, 38, evt_title[evt], UI_STYLE_HIGHLIGHT);
        ui_window(0, 3, 40, 10);
        ui_text_wrap(1, 4, 38, 8, evt_text[evt], UI_STYLE_TEXT);
        ui_window(0, 13, 40, 15);
        ui_text_draw(1, 14, "Que décidez-vous ?", UI_STYLE_TEXT);
        ui_list_draw(&choice_list);
        ui_text_draw(1, 26, "Haut/Bas : choisir   A : valider", UI_STYLE_TEXT);
        ui_art_draw(evt_art(evt), VIGNETTE_X, VIGNETTE_Y);   // PAL2 : pas de style DIM ici
    } else {
        // Le texte reste affiché ; on remplace la liste par la décision et le journal.
        ui_clear_rect(1, 14, 38, 13);
        ui_text_draw(1, 14, "Votre décision :", UI_STYLE_TEXT);
        ui_text_draw_n(1, 15, evt_choice[evt][chosen], 38, 0, UI_STYLE_HIGHLIGHT);
        ui_text_draw(1, 17, "Journal", UI_STYLE_HIGHLIGHT);
        ui_text_wrap(1, 18, 28, 7, evt_journal[evt][chosen], UI_STYLE_TEXT);
        ui_text_draw(1, 26, "A : continuer", UI_STYLE_TEXT);
    }
}

static void event_exit(void)
{
    art_reset();
}

const Screen scr_event = { event_init, event_update, event_draw, event_exit };
