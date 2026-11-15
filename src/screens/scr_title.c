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
#include "screens/scr_options.h"

enum { PH_PRESS = 0, PH_ORIENT, PH_PM, PH_DIFF, PH_DUREE };

enum { D_ALL = 1, D_LIST = 2, D_DESC = 4 };

#define LIST_Y 18       // sous le titre de la phase (ligne 17), dans la fenêtre du bas (16-27)
#define WIN_Y  16       // l illustration occupe les lignes 0 à 15
#define PORTRAIT_X 29   // gueule 10 x 12 sur l illustration (PAL3 ; le titre est en PAL2)
#define PORTRAIT_Y 4   // rangées 4-15 : le bas du médaillon touche la fenêtre (rangée 16)

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
static u8 stale_box;     // 1 : message à ouvrir, 2 : ouvert (sauvegarde d'une ancienne version)
static bool stale_seen;  // annoncée une seule fois par allumage
static UiList list;

// Menu d'accueil (3.19) : les raccourcis START, B et C restent valables sur toutes les lignes.
enum { HOME_NEW = 0, HOME_CONTINUE, HOME_SCORES, HOME_OPTIONS, HOME_COUNT };
static const char *const home_items[HOME_COUNT] = {
    "Nouvelle partie", "Continuer la partie", "Meilleurs scores", "Options"
};
#define HOME_X 9        // liste x9-30, barre x9-29, pastilles des raccourcis en x30
#define HOME_Y 20
#define HOME_W 22
static UiList home;
static u8 home_sel;

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
    stale_box = (u8)(!has_save && !stale_seen && game_save_stale());
    // Retour des options : barre sur « Options » ; sinon « Nouvelle partie » (A garde son effet).
    home_sel = options_take_return() ? HOME_OPTIONS : HOME_NEW;
    // Voix « For sure ! » une seule fois par allumage ; la musique suit juste après.
    voice_play_intro_once();   // la musique du titre (screen.c) attend la fin de la voix
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

    if (stale_box == 2) {   // « Partie d'une ancienne version... Une nouvelle partie ? »
        s16 r = ui_msgbox_update();
        if (r < 0 && (p & INPUT_B)) r = 1;
        if (r < 0) return;
        ui_msgbox_close();
        stale_box = 0;
        stale_seen = TRUE;
        if (r == 0) {
            press_ms = screen_time_ms();
            home_sel = HOME_NEW;
            go_phase(PH_ORIENT);
        } else {
            dirty = D_ALL;
        }
        return;
    }

    if (phase == PH_PRESS) {
        u8 act = 0xFF;
        if (p & INPUT_C)
            act = HOME_SCORES;
        else if (p & INPUT_B)
            act = has_save ? HOME_CONTINUE : 0xFF;
        else if (p & INPUT_START)
            act = HOME_NEW;
        else {
            switch (ui_list_update(&home)) {
            case UI_LIST_MOVED:
                home_sel = home.sel;
                dirty |= D_LIST;
                break;
            case UI_LIST_CHOSEN:
                act = home.sel;
                break;
            default:
                break;
            }
        }
        if (act == HOME_SCORES) {
            screen_change(SCREEN_SCORES);
        } else if (act == HOME_CONTINUE) {
            if (!game_continue()) {
                has_save = 0;
                dirty = D_ALL;
            }
        } else if (act == HOME_OPTIONS) {
            options_open(SCREEN_TITLE);
        } else if (act == HOME_NEW) {
            press_ms = screen_time_ms();
            home_sel = HOME_NEW;
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
            ui_list_init(&home, HOME_X, HOME_Y, HOME_W, HOME_COUNT, home_items, HOME_COUNT);
            ui_list_set_disabled(&home, HOME_CONTINUE, !has_save);
            home.sel = home_sel;
            dirty |= D_LIST;
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

    if (stale_box == 1 && phase == PH_PRESS) {
        static const char *const ch[2] = { "Nouvelle partie", "Plus tard" };
        ui_msgbox_open(NULL, txt[TXT_SAVE_ANCIENNE], ch, 2);
        stale_box = 2;
    }
    if (phase == PH_PRESS) {
        if (stale_box == 2) return;   // la boîte de message couvre la liste : rien à animer dessous
        // Clignotement toutes les 500 ms, quelle que soit la fréquence : pastille START seule.
        bool on = ((screen_time_ms() / 500) & 1) == 0;
        if (dirty & D_LIST) {
            ui_list_draw(&home);   // efface la colonne des pastilles : on les repose
            ui_icon_draw(HOME_X + HOME_W - 1, HOME_Y + HOME_CONTINUE, UI_ICON_BTN_B);
            ui_icon_draw(HOME_X + HOME_W - 1, HOME_Y + HOME_SCORES, UI_ICON_BTN_C);
            blink_on = !on;
            dirty &= (u8)~D_LIST;
        }
        if (on != blink_on) {
            blink_on = on;
            if (on)
                ui_icon_draw(HOME_X + HOME_W - 1, HOME_Y + HOME_NEW, UI_ICON_BTN_START);
            else
                ui_clear_rect(HOME_X + HOME_W - 1, HOME_Y + HOME_NEW, 1, 1);
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
