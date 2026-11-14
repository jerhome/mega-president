// Mega Président — écran titre et choix de l'orientation (tâche 6.3).
#include <genesis.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_world.h"

enum { PH_PRESS = 0, PH_CHOOSE };

enum { D_ALL = 1, D_LIST = 2, D_DESC = 4 };

#define LIST_Y 16

static const char *const orient_items[ORIENT_COUNT] = {
    "Gauche — RSV", "Centre — MJM", "Droite — UTL"
};

static u8 phase;
static u8 dirty;
static bool blink_on;
static u32 seed_acc;
static u32 press_ms;
static UiList list;

// Groupe du parti du joueur selon l'orientation (contrat §4).
static u8 group_of(u8 orient)
{
    return (u8)(GRP_G2 + orient);
}

static void title_init(void)
{
    phase = PH_PRESS;
    dirty = D_ALL;
    blink_on = FALSE;
    seed_acc = 0;
    press_ms = 0;
    ui_list_init(&list, 4, LIST_Y, 20, 3, orient_items, ORIENT_COUNT);
    list.sel = ORIENT_CENTRE;
}

static void title_update(void)
{
    u16 p = input_pressed();

    // Entropie : compteur d'images et durée d'attente.
    seed_acc = seed_acc * 1103515245UL + 12345UL + input_frame_ticks() + input_held();

    if (phase == PH_PRESS) {
        if (p & (INPUT_START | INPUT_OK)) {
            press_ms = screen_time_ms();
            phase = PH_CHOOSE;
            dirty = D_ALL;
        }
        return;
    }

    switch (ui_list_update(&list)) {
    case UI_LIST_MOVED:
        dirty |= D_LIST | D_DESC;
        break;
    case UI_LIST_CHOSEN:
        game_new(list.sel, seed_acc ^ (press_ms << 8) ^ (screen_time_ms() * 2654435761UL));
        screen_change(SCREEN_GAME);
        break;
    case UI_LIST_CANCELLED:
        phase = PH_PRESS;
        blink_on = FALSE;
        dirty = D_ALL;
        break;
    default:
        if (p & INPUT_START) {
            game_new(list.sel, seed_acc ^ (press_ms << 8) ^ (screen_time_ms() * 2654435761UL));
            screen_change(SCREEN_GAME);
        }
        break;
    }
}

static void draw_frame(void)
{
    ui_clear();
    ui_window(3, 4, 34, 9);
    ui_text_center(4, 6, 32, "MEGA PRÉSIDENT", UI_STYLE_HIGHLIGHT);
    ui_text_center(4, 8, 32, "Élysée, cinq ans de pouvoir", UI_STYLE_TEXT);
    ui_text_center(4, 10, 32, "« À vous de gouverner ! »", UI_STYLE_DIM);
}

static void title_draw(void)
{
    if (dirty & D_ALL) {
        draw_frame();
        if (phase == PH_CHOOSE) {
            ui_text_center(0, 14, UI_COLS, "Votre orientation politique", UI_STYLE_HIGHLIGHT);
            ui_text_draw(1, 26, "Haut/Bas : choisir  A : commencer", UI_STYLE_DIM);
            dirty |= D_LIST | D_DESC;
        }
        blink_on = FALSE;
        dirty &= (u8)~D_ALL;
    }

    if (phase == PH_PRESS) {
        // Clignotement toutes les 500 ms, quelle que soit la fréquence.
        bool on = ((screen_time_ms() / 500) & 1) == 0;
        if (on != blink_on) {
            blink_on = on;
            if (on)
                ui_text_center(0, 16, UI_COLS, "Appuyez sur START", UI_STYLE_TEXT);
            else
                ui_clear_rect(0, 16, UI_COLS, 1);
        }
        return;
    }

    if (dirty & D_LIST) {
        ui_list_draw(&list);
        dirty &= (u8)~D_LIST;
    }
    if (dirty & D_DESC) {
        u8 g = group_of(list.sel);
        ui_clear_rect(1, 20, UI_TEXT_COLS, 5);
        ui_text_draw_n(1, 20, grp_name[g], UI_TEXT_COLS, 0, UI_STYLE_HIGHLIGHT);
        ui_text_wrap(1, 22, UI_TEXT_COLS, 3, grp_desc[g], UI_STYLE_TEXT);
        dirty &= (u8)~D_DESC;
    }
}

const Screen scr_title = { title_init, title_update, title_draw, NULL };
