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
    ui_list_init(&choice_list, 1, 20, 38, n, choice_items, n);
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

// Mise en page (charte V3) :
//  0-15  fenêtre à bandeau (titre) : texte à gauche (27 colonnes), vignette 8 x 6
//        à droite (x 30-37), cadre doré posé sur l'image, centrée sur le texte
//  16-27 fenêtre à bandeau : choix, puis décision et journal
#define TEXT_W   27
#define MED_X    29
#define MED_Y    3
#define MED_W    10
#define MED_H    12
#define LOW_Y    16

// Textes coupés à la main pour 38 colonnes : remis à plat pour être recoupés.
static const char *flatten(const char *src)
{
    static char flat[256];
    u16 i;
    for (i = 0; src[i] && i < sizeof(flat) - 1; i++)
        flat[i] = (src[i] == '\n') ? ' ' : src[i];
    flat[i] = 0;
    return flat;
}

static void draw_lower(const char *title)
{
    ui_window_title(0, LOW_Y, 40, UI_ROWS - LOW_Y, title, TRUE);
}

// Haut de l'écran : titre, texte et, à droite, la vignette (choix) ou le ministre
// concerné dans un médaillon 10 x 12 (décision).
static void draw_upper(bool minister)
{
    ArtId art = evt_art(evt);
    const char *t = flatten(evt_text[evt]);
    u16 w, h, lines = ui_text_count_lines(TEXT_W, t), ax, ay;
    art_reset();
    ui_window_title(0, 0, 40, LOW_Y, evt_title[evt], FALSE);
    if (lines > 10)
        lines = 10;
    ui_text_wrap(1, 4, TEXT_W, 10, t, UI_STYLE_TEXT);
    if (minister) {
        // Un seul visage animé : la gueule du ministre prend la place de la vignette.
        ui_medallion(MED_X, MED_Y, MED_W, MED_H);
        ui_gueule_draw(evt_minister(evt), evt_mood(evt, chosen), MED_X, MED_Y, PAL3);
        ui_gueule_talk(ui_gueule_talk_ms(flatten(evt_journal[evt][chosen])));
        return;
    }
    // Le cadre épouse la vignette (8 x 6) : posé sur l'image elle-même,
    // centré sur la colonne de droite et sur le bloc de texte.
    ui_art_size(art, &w, &h);
    ax = MED_X + (MED_W - w) / 2;
    ay = (lines > h) ? 4 + (lines - h) / 2 : 4;
    if (ay + h > MED_Y + MED_H)
        ay = MED_Y + MED_H - h;
    ui_medallion(ax, ay, w, h);
    ui_art_draw_pal(art, ax, ay, PAL3);
}

static void event_draw(void)
{
    if (!dirty) return;
    dirty = FALSE;

    if (phase == P_CHOOSE) {
        draw_upper(FALSE);
        draw_lower("Que décidez-vous ?");
        ui_list_draw(&choice_list);
        ui_footer(1, 26, 38, "Haut/Bas : choisir   A : valider");
    } else {
        // Le ministre concerné réagit en haut ; le bas montre la décision et l'écho dans la presse.
        draw_upper(TRUE);
        draw_lower("Votre décision");
        ui_icon_draw(1, 20, UI_ICON_CHECK);
        ui_text_draw_n(3, 20, evt_choice[evt][chosen], 36, 0, UI_STYLE_HIGHLIGHT);
        ui_separator(1, 21, 38);
        ui_icon_draw(1, 22, UI_ICON_NEWS);
        ui_text_draw(3, 22, "Le Quotidien du Matin", UI_STYLE_DIM);
        ui_text_wrap(3, 23, 36, 3, flatten(evt_journal[evt][chosen]), UI_STYLE_TEXT);
        ui_footer(1, 26, 38, "A : continuer");
    }
}

static void event_exit(void)
{
    art_reset();
}

const Screen scr_event = { event_init, event_update, event_draw, event_exit };
