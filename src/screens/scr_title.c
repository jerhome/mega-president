// Mega Président — écran titre et choix de l'orientation (tâche 6.3).
#include "audio/music.h"
#include <genesis.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_world.h"
#include "content/content_texts.h"
#include "ui/screen.h"
#include "ui/ui_art.h"
#include "screens/evt_art.h"

enum { PH_PRESS = 0, PH_ORIENT, PH_PM, PH_DIFF, PH_DUREE };

enum { D_ALL = 1, D_LIST = 2, D_DESC = 4 };

#define LIST_Y 19       // sous le titre de la phase (ligne 18), dans la fenêtre du bas (17-27)
#define WIN_Y  17
#define PORTRAIT_X 31   // portrait 7 x 7 dans un cadre 9 x 9 sur l'illustration du titre
#define PORTRAIT_Y 4

static const char *const orient_items[ORIENT_COUNT] = {
    "Gauche — RSV", "Centre — MJM", "Droite — UTL"
};

static const char *const diff_items[DIFF_COUNT] = { "Facile", "Normal", "Difficile" };
static const char *pm_items[PM_COUNT];
static const char *duree_items[2];
static u8 duree_sel = 0;   // 0 : 20 tours, 1 : 60 tours
static u8 orient_sel = ORIENT_CENTRE;
static u8 pm_sel = PM_TECHNOCRATE;
static u8 diff_sel = DIFF_NORMAL;

static u8 phase;
static u8 dirty;
static bool blink_on;
static u32 seed_acc;
static u32 press_ms;
static u8 has_save;
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
    has_save = game_has_save();
    music_play(MUS_TITLE);
    duree_items[0] = txt[TXT_DUREE_20];
    duree_items[1] = txt[TXT_DUREE_60];
    pm_items[PM_TECHNOCRATE] = txt[TXT_PM_TECHNOCRATE_NOM];
    pm_items[PM_RASSEMBLEUR] = txt[TXT_PM_RASSEMBLEUR_NOM];
    pm_items[PM_REFORMATEUR] = txt[TXT_PM_REFORMATEUR_NOM];
}

// Prépare la liste de la phase de choix courante.
static void list_setup(void)
{
    switch (phase) {
    case PH_ORIENT:
        ui_list_init(&list, 4, LIST_Y, 24, 3, orient_items, ORIENT_COUNT);
        list.sel = orient_sel;
        break;
    case PH_PM:
        ui_list_init(&list, 4, LIST_Y, 30, 3, pm_items, PM_COUNT);
        list.sel = pm_sel;
        break;
    case PH_DUREE:
        ui_list_init(&list, 2, LIST_Y, 36, 2, duree_items, 2);
        list.sel = duree_sel;
        break;
    default:
        ui_list_init(&list, 4, LIST_Y, 20, 3, diff_items, DIFF_COUNT);
        list.sel = diff_sel;
        break;
    }
}

static void start_game(void)
{
    game_new(orient_sel, pm_sel, diff_sel, duree_sel ? 12 : 4,
             seed_acc ^ (press_ms << 8) ^ (screen_time_ms() * 2654435761UL));
    screen_change(SCREEN_GAME);
}

static void go_phase(u8 ph)
{
    phase = ph;
    blink_on = FALSE;
    dirty = D_ALL;
    if (ph != PH_PRESS)
        list_setup();
}

// Valide la sélection de la phase courante.
static void choose(void)
{
    if (phase == PH_ORIENT) {
        orient_sel = list.sel;
        go_phase(PH_PM);
    } else if (phase == PH_PM) {
        pm_sel = list.sel;
        go_phase(PH_DIFF);
    } else if (phase == PH_DIFF) {
        diff_sel = list.sel;
        go_phase(PH_DUREE);
    } else {
        duree_sel = list.sel;
        start_game();
    }
}

static void title_update(void)
{
    u16 p = input_pressed();

    // Entropie : compteur d'images et durée d'attente.
    seed_acc = seed_acc * 1103515245UL + 12345UL + input_frame_ticks() + input_held();

    if (phase == PH_PRESS) {
        if (p & INPUT_C) {
            screen_change(SCREEN_SCORES);
        } else if ((p & INPUT_B) && has_save) {
            if (!game_continue())
                has_save = 0;
        } else if (p & (INPUT_START | INPUT_OK)) {
            press_ms = screen_time_ms();
            go_phase(PH_ORIENT);
        }
        return;
    }

    switch (ui_list_update(&list)) {
    case UI_LIST_MOVED:
        dirty |= D_LIST | D_DESC;
        break;
    case UI_LIST_CHOSEN:
        choose();
        break;
    case UI_LIST_CANCELLED:
        go_phase((u8)(phase - 1));
        break;
    default:
        if (p & INPUT_START)
            choose();
        break;
    }
}

// Fond : illustration du titre (lignes 0-15, palette PAL2 : pas de style DIM
// tant qu'elle est affichée), titre dans le ciel, fenêtre du bas (17-27).
static void draw_frame(void)
{
    art_reset();
    ui_art_draw(ART_TITLE, 0, 0);
    ui_text_center(0, 1, UI_COLS, "MEGA PRÉSIDENT", UI_STYLE_HIGHLIGHT);
    ui_window(0, WIN_Y, UI_COLS, 11);
}

// Portrait en surbrillance (cadre + palette PAL3, celle de l'illustration du titre est PAL2).
static void draw_portrait(void)
{
    if (phase != PH_ORIENT && phase != PH_PM)
        return;
    ui_art_draw_pal(phase == PH_ORIENT ? ART_PRES(list.sel) : ART_PM(list.sel),
                    PORTRAIT_X, PORTRAIT_Y, PAL3);
}

static void title_draw(void)
{
    if (dirty & D_ALL) {
        draw_frame();
        if (phase == PH_PRESS) {
            ui_text_center(0, 19, UI_COLS, "Élysée, cinq ans de pouvoir", UI_STYLE_HIGHLIGHT);
            ui_text_center(0, 20, UI_COLS, "« À vous de gouverner ! »", UI_STYLE_TEXT);
            if (has_save)
                ui_text_center(0, 24, UI_COLS, "B : continuer la partie", UI_STYLE_TEXT);
            ui_text_center(0, 25, UI_COLS, "C : meilleurs scores", UI_STYLE_TEXT);
        } else {
            if (phase == PH_ORIENT || phase == PH_PM)
                ui_window(PORTRAIT_X - 1, PORTRAIT_Y - 1, 9, 9);
            static const char *const titles[4] = {
                "Votre orientation politique", "Votre Premier ministre", "Difficulté", NULL
            };
            ui_text_center(0, 18, UI_COLS, phase == PH_DUREE ? txt[TXT_DUREE_TITRE] : titles[phase - PH_ORIENT],
                           UI_STYLE_HIGHLIGHT);
            ui_text_draw(1, 26, phase == PH_DUREE ? "Haut/Bas  A : jouer  B : retour"
                                                 : "Haut/Bas  A : suite  B : retour",
                         UI_STYLE_TEXT);
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
                ui_text_center(0, 22, UI_COLS, "Appuyez sur START", UI_STYLE_TEXT);
            else
                ui_clear_rect(1, 22, UI_TEXT_COLS, 1);
        }
        return;
    }

    if (dirty & D_LIST) {
        ui_list_draw(&list);
        dirty &= (u8)~D_LIST;
    }
    if (dirty & D_DESC) {
        ui_clear_rect(1, 22, UI_TEXT_COLS, 4);
        draw_portrait();
        if (phase == PH_ORIENT) {
            u8 g = group_of(list.sel);
            ui_text_draw_n(1, 23, grp_name[g], UI_TEXT_COLS, 0, UI_STYLE_HIGHLIGHT);
            ui_text_wrap(1, 24, UI_TEXT_COLS, 2, grp_desc[g], UI_STYLE_TEXT);
        } else if (phase == PH_PM) {
            ui_text_wrap(1, 23, UI_TEXT_COLS, 3, txt[TXT_PM_TECHNOCRATE_DESC + 2 * list.sel], UI_STYLE_TEXT);
        } else if (phase == PH_DUREE) {
            ui_text_wrap(1, 23, UI_TEXT_COLS, 3, txt[TXT_DUREE_20_DESC + list.sel], UI_STYLE_TEXT);
        } else {
            ui_text_wrap(1, 23, UI_TEXT_COLS, 3, txt[TXT_DIFF_FACILE + list.sel], UI_STYLE_TEXT);
        }
        dirty &= (u8)~D_DESC;
    }
}

static void title_exit(void)
{
    art_reset();
}

const Screen scr_title = { title_init, title_update, title_draw, title_exit };
