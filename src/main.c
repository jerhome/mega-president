// Mega Président — point d'entrée.
// V0 (tâches 1.1 à 1.5) : écran titre et écran de démonstration des widgets.
// Ce fichier n'appelle jamais VDP_* / PAL_* : tout passe par src/ui/.

#include <genesis.h>
#include "ui/ui.h"
#include "ui/input.h"
#include "ui/screen.h"

// --- Écran titre ------------------------------------------------------------

static bool title_blink_on;

static void title_init(void)
{
    ui_window(3, 4, 34, 9);
    ui_text_center(4, 6, 32, "MEGA PRÉSIDENT", UI_STYLE_HIGHLIGHT);
    ui_text_center(4, 8, 32, "Élysée, cinq ans de pouvoir", UI_STYLE_TEXT);
    ui_text_center(4, 10, 32, "« À vous de gouverner ! »", UI_STYLE_DIM);

    ui_text_draw(1, 18, "Jeu de caractères :", UI_STYLE_DIM);
    ui_text_draw(1, 19, "éèêëàâçîïôùûüœ ÉÈÊÀÇÔ «»°€’…", UI_STYLE_TEXT);
    ui_text_draw(1, 20, "0123456789 ,;:!?.'\"()[]{}%&@#$*+-=/<>", UI_STYLE_TEXT);

    ui_text_draw(1, 25, IS_PAL_SYSTEM ? "PAL 50 Hz" : "NTSC 60 Hz", UI_STYLE_DIM);
    title_blink_on = FALSE;
}

static void title_update(void)
{
    if (input_pressed() & (INPUT_START | INPUT_OK))
        screen_change(SCREEN_DEMO);
}

static void title_draw(void)
{
    // Clignotement toutes les 500 ms, quelle que soit la fréquence.
    bool on = ((screen_time_ms() / 500) & 1) == 0;

    if (on != title_blink_on) {
        title_blink_on = on;
        if (on)
            ui_text_center(0, 15, UI_COLS, "Appuyez sur START", UI_STYLE_TEXT);
        else
            ui_clear_rect(0, 15, UI_COLS, 1);
    }
}

static const Screen screen_title = { title_init, title_update, title_draw, NULL };

// --- Écran de démonstration des widgets ----------------------------------------

static const char demo_text[] =
    "Le Président élu au suffrage universel gère l’économie, l’écologie et la "
    "sécurité. «\u00A0Où ça\u00A0?\u00A0» À Noël, Ève a payé 15\u00A0€ un cœur "
    "d’œuf à 37\u00A0°C\u00A0; naïf, maïs, sûr… ÊTRE ÉLU, c’est être "
    "«\u00A0président des riches\u00A0».";

static const char *const law_names[] = {
    "Hausse du SMIC",
    "Réforme des retraites",
    "Taxe sur les écrans",
    "Plan école numérique",
    "Loi « Sécurité globale »",
    "Prime à l’isolation",
    "Impôt sur la fortune",
    "Gratuité des transports",
};
#define LAW_COUNT (sizeof(law_names) / sizeof(law_names[0]))

static const char *const vote_choices[] = { "Voter la loi", "Amender le texte", "Retirer" };

enum { DIRTY_ALL = 1, DIRTY_LIST = 2, DIRTY_STATUS = 4 };

static UiList demo_list;
static u8 demo_dirty;
static s16 demo_last_choice;
static u8 demo_last_law;

static void demo_init(void)
{
    ui_list_init(&demo_list, 1, 13, 18, 7, law_names, LAW_COUNT);
    demo_list.disabled = (u32)1 << 2;   // « Taxe sur les écrans » grisée
    demo_dirty = DIRTY_ALL;
    demo_last_choice = -1;
    demo_last_law = 0;
}

static void demo_update(void)
{
    s16 r;

    if (ui_msgbox_is_open()) {
        r = ui_msgbox_update();
        if (r >= 0) {
            ui_msgbox_close();
            demo_last_choice = r;
            demo_dirty = DIRTY_ALL;   // la boîte recouvrait une partie de l'écran
        }
        return;
    }

    switch (ui_list_update(&demo_list)) {
    case UI_LIST_MOVED:
        demo_dirty |= DIRTY_LIST;
        break;
    case UI_LIST_CHOSEN:
        demo_last_law = demo_list.sel;
        ui_msgbox_open(law_names[demo_list.sel],
                       "Le gouvernement présente ce texte à l’Assemblée nationale. "
                       "Les syndicats menacent d’une grève générale ; la majorité "
                       "hésite. Que décidez-vous ?",
                       vote_choices, 3);
        break;
    case UI_LIST_CANCELLED:
        screen_change(SCREEN_TITLE);
        break;
    default:
        break;
    }
}

static void demo_draw_status(void)
{
    ui_clear_rect(1, 23, UI_TEXT_COLS, 1);
    if (demo_last_choice < 0) {
        ui_text_draw(1, 23, "Aucune décision pour l’instant.", UI_STYLE_DIM);
    } else {
        u16 n = ui_text_draw(1, 23, vote_choices[demo_last_choice], UI_STYLE_HIGHLIGHT);
        n += ui_text_draw(1 + n, 23, " : ", UI_STYLE_TEXT);
        ui_text_draw_n(1 + n, 23, law_names[demo_last_law], UI_TEXT_COLS - n, 0, UI_STYLE_TEXT);
    }
}

static void demo_draw_gauge(u16 y, const char *label, s32 value, u16 decimals,
                            const char *unit, s16 gval, s16 gmax, UiStyle style)
{
    char buf[14];
    u16 n;

    ui_text_draw_n(21, y, label, 11, 11, UI_STYLE_TEXT);
    n = ui_format_number(buf, value, decimals);
    ui_clear_rect(32, y, 6, 1);
    ui_text_draw(38 - n - ui_text_len(unit), y, buf, style);
    ui_text_draw(38 - ui_text_len(unit), y, unit, style);
    ui_gauge_draw(21, y + 1, 17, gval, gmax, style);
}

static void demo_draw(void)
{
    if (demo_dirty & DIRTY_ALL) {
        ui_window(0, 0, 40, 10);
        ui_text_draw(1, 1, "Démonstration de l’interface", UI_STYLE_HIGHLIGHT);
        ui_text_wrap(1, 3, UI_TEXT_COLS, 6, demo_text, UI_STYLE_TEXT);

        ui_window(0, 10, 20, 12);
        ui_text_draw(1, 11, "Lois", UI_STYLE_HIGHLIGHT);

        ui_window(20, 10, 20, 12);
        demo_draw_gauge(11, "Popularité", 425, 1, " %", 425, 1000, UI_GAUGE_POSITIVE);
        demo_draw_gauge(13, "Chômage", 750, 2, " %", 750, 2000, UI_GAUGE_NEUTRAL);
        demo_draw_gauge(15, "Dette", 1100, 1, " %", 1100, 2000, UI_GAUGE_GOLD);

        ui_window(0, 22, 40, 5);
        ui_text_draw(1, 24, "Haut/Bas : choisir  A : voter", UI_STYLE_DIM);
        ui_text_draw(1, 25, "Gauche/Droite : page  B : titre", UI_STYLE_DIM);
        demo_dirty = DIRTY_LIST | DIRTY_STATUS;
    }
    if (demo_dirty & DIRTY_LIST)
        ui_list_draw(&demo_list);
    if (demo_dirty & DIRTY_STATUS)
        demo_draw_status();
    demo_dirty = 0;

    if (!ui_msgbox_is_open()) {
        // Jauge animée : une montée de 0 à 100 en 4 s, identique en PAL et NTSC.
        s16 tension = (s16)((screen_time_ms() % 4000) / 40);
        demo_draw_gauge(17, "Tension", tension, 0, "", tension, 100, UI_GAUGE_NEGATIVE);
    }
}

static const Screen screen_demo = { demo_init, demo_update, demo_draw, NULL };

// -----------------------------------------------------------------------------

int main(bool hard)
{
    (void)hard;
    screen_register(SCREEN_TITLE, &screen_title);
    screen_register(SCREEN_DEMO, &screen_demo);
    screen_run(SCREEN_TITLE);
    return 0;
}
