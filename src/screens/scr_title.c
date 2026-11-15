// Mega Président — écran titre et choix de l'orientation (tâche 6.3).
#include "audio/music.h"
#include "audio/voice.h"
#include <genesis.h>
#include "screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "game/game.h"
#include "content/content_world.h"
#include "content/content_texts.h"
#include "ui/screen.h"
#include "version.h"
#include "save/sram.h"
#include "ui/ui_art.h"
#include "screens/evt_art.h"

enum { PH_PRESS = 0, PH_ORIENT, PH_PM, PH_DIFF, PH_DUREE };

enum { D_ALL = 1, D_LIST = 2, D_DESC = 4 };

#define LIST_Y 18       // sous le titre de la phase (ligne 17), dans la fenêtre du bas (16-27)
#define WIN_Y  16       // l illustration occupe les lignes 0 à 15
#define PORTRAIT_X 29   // gueule 10 x 12 sur l illustration (PAL3 ; le titre est en PAL2)
#define PORTRAIT_Y 2

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
static bool music_wait;   // musique différée jusqu'à la fin de la voix

// Groupe du parti du joueur selon l'orientation (contrat §4).
static u8 group_of(u8 orient)
{
    return (u8)(GRP_G2 + orient);
}

// Accroches en rotation (une par démarrage ou retour au titre), 38 colonnes au plus.
static const char *const taglines[][2] = {
    { "Élu avec 51 %.",                      "Les 49 autres vous attendent dehors." },
    { "La République vous fait confiance.",  "Enfin, 51 % d’entre elle." },
    { "Cinq ans pour changer la vie.",       "Ou au moins la couleur des rideaux." },
    { "Promettre, c’est gratuit.",           "Tenir, ça coûte des CP." },
    { "Le pouvoir est à vous.",              "Le SMIC, lui, attend toujours." },
    { "Ici, c’est vous le monarque.",        "Les syndicats ne sont pas d’accord." },
    { "Jupiter descend de l’Olympe.",        "Il a raté la marche." },
    { "Gouverner, c’est prévoir.",           "Surtout les manifs." },
    { "Cinq ans, 577 députés, une manette.", "Bonne chance, Monsieur le Président." },
    { "L’Élysée vous attend.",               "Le peuple aussi. Il a des pancartes." },
};
#define TAGLINE_COUNT ((u8)(sizeof(taglines) / sizeof(taglines[0])))
static u8 tagline;

static void title_init(void)
{
    tagline = sram_tagline_next(TAGLINE_COUNT);
#ifdef MP_DEBUG_SKIP_YEARS
    {   // ROM de test seulement : partie automatique jusqu'au rapport de fin d'année 3.
        static bool done;
        if (!done) {
            done = TRUE;
            game_new(0, 0, 0, 4, 1234);
            game_end_turn();
            return;
        }
    }
#endif
#ifdef MP_DEBUG_SCREEN
    {   // ROM de démonstration seulement : saut direct à l'écran à capturer.
        static bool done;
        if (!done) {
            done = TRUE;
            game_debug_jump();
            return;
        }
    }
#endif
    phase = PH_PRESS;
    dirty = D_ALL;
    blink_on = FALSE;
    seed_acc = 0;
    press_ms = 0;
    has_save = game_has_save();
    // Voix « For sure ! » une seule fois par allumage ; la musique suit juste après.
    music_wait = voice_play_intro_once();
    if (!music_wait)
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

    if (music_wait && !voice_busy()) {
        music_wait = FALSE;
        music_play(MUS_TITLE);
    }

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

// Fond : illustration du titre (lignes 0-15, PAL2), plaque du titre dans le ciel,
// fenêtre du bas (16-27).
static void draw_frame(void)
{
    art_reset();
    ui_art_draw(ART_TITLE, 0, 0);
    ui_banner(10, 0, 20, "MEGA PRÉSIDENT", FALSE);
    ui_window(0, WIN_Y, UI_COLS, UI_ROWS - WIN_Y);
}

// Gueule du président ou du PM sélectionné, sur l'illustration (PAL3).
static void draw_portrait(void)
{
    if (phase != PH_ORIENT && phase != PH_PM)
        return;
    ui_medallion(PORTRAIT_X, PORTRAIT_Y, UI_PORTRAIT_W, UI_PORTRAIT_H);
    ui_gueule_draw(phase == PH_ORIENT ? GUEULE_PRES(list.sel) : GUEULE_PM(list.sel), EXPR_CONTENT,
                   PORTRAIT_X, PORTRAIT_Y, PAL3);
}


static void title_draw(void)
{
    if (dirty & D_ALL) {
        draw_frame();
        if (phase == PH_PRESS) {
            ui_text_center(0, 17, UI_COLS, taglines[tagline][0], UI_STYLE_HIGHLIGHT);
            ui_text_center(0, 18, UI_COLS, taglines[tagline][1], UI_STYLE_TEXT);
            ui_footer(1, 22, UI_TEXT_COLS, has_save ? "B : continuer la partie   C : meilleurs scores"
                                                    : "C : meilleurs scores");
            // Pied de page, séparé des actions par une ligne vide et un filet.
            ui_separator(1, 24, UI_TEXT_COLS);
            ui_text_center(0, 25, UI_COLS, GAME_AUTHOR " - " GAME_VERSION, UI_STYLE_DIM);
            ui_text_center(0, 26, UI_COLS, "Développé avec Claude", UI_STYLE_DIM);
        } else {
            static const char *const titles[4] = {
                "Votre orientation politique", "Votre Premier ministre", "Difficulté", NULL
            };
            ui_text_center(0, 17, UI_COLS, phase == PH_DUREE ? txt[TXT_DUREE_TITRE] : titles[phase - PH_ORIENT],
                           UI_STYLE_HIGHLIGHT);
            ui_footer(1, 26, UI_TEXT_COLS, phase == PH_DUREE ? "A : jouer   B : retour"
                                                            : "A : suite   B : retour");
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
                ui_footer(1, 20, UI_TEXT_COLS, "START : nouvelle partie");
            else
                ui_clear_rect(1, 20, UI_TEXT_COLS, 1);
        }
        return;
    }

    if (dirty & D_LIST) {
        ui_list_draw(&list);
        dirty &= (u8)~D_LIST;
    }
    if (dirty & D_DESC) {
        ui_clear_rect(1, 22, UI_TEXT_COLS, 3);
        draw_portrait();
        if (phase == PH_ORIENT) {
            u8 g = group_of(list.sel);
            ui_text_draw_n(1, 22, grp_name[g], UI_TEXT_COLS, 0, UI_STYLE_HIGHLIGHT);
            ui_text_wrap(1, 23, UI_TEXT_COLS, 2, grp_desc[g], UI_STYLE_TEXT);
        } else if (phase == PH_PM) {
            ui_text_wrap(1, 22, UI_TEXT_COLS, 3, txt[TXT_PM_TECHNOCRATE_DESC + 2 * list.sel], UI_STYLE_TEXT);
        } else if (phase == PH_DUREE) {
            ui_text_wrap(1, 22, UI_TEXT_COLS, 3, txt[TXT_DUREE_20_DESC + list.sel], UI_STYLE_TEXT);
        } else {
            ui_text_wrap(1, 22, UI_TEXT_COLS, 3, txt[TXT_DIFF_FACILE + list.sel], UI_STYLE_TEXT);
        }
        dirty &= (u8)~D_DESC;
    }
}


static void title_exit(void)
{
    art_reset();
}

const Screen scr_title = { title_init, title_update, title_draw, title_exit };
