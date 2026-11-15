// Mega Président — point d'entrée.
// Enregistre les écrans du jeu (src/screens/) et lance la boucle principale.
// Ce fichier n'appelle jamais VDP_* / PAL_* : tout passe par src/ui/.

#include <genesis.h>
#include "ui/screen.h"
#include "screens/screens.h"
#include "ui/ui.h"
#include "ui/input.h"
#include "audio/music.h"
#include "audio/sfx.h"
#include "game/game.h"
#include "save/sram.h"
#include "save/settings_apply.h"

// Musique de l'écran (3.14) : thème de l'année pour le tableau de bord, issue de la
// partie pour l'écran de fin (victoire = deux mandats menés à terme sans destitution).
static void enter_music(ScreenId id)
{
    u8 arg = 0;
    if (id == SCREEN_GAME)
        arg = sim_year(&g_game.country);
    else if (id == SCREEN_END)
        arg = (!game_destitue() && game_two_mandates()) ? MUSIC_END_WIN : MUSIC_END_LOSE;
    music_for_screen(id, arg);
}

int main(bool hard)
{
    (void)hard;
    ui_init();
    input_init();
    sfx_init();
    music_init();
    // Réglages de l'écran Options lus AVANT le premier écran (sinon « For sure ! »
    // partirait avant la lecture du réglage Voix).
    sram_settings_load();
    settings_apply_all();
    screen_set_enter_hook(enter_music);
    screen_register(SCREEN_TITLE, &scr_title);
    screen_register(SCREEN_GAME, &scr_game);
    screen_register(SCREEN_REPORT, &scr_report);
    screen_register(SCREEN_EVENT, &scr_event);
    screen_register(SCREEN_ELECTION, &scr_election);
    screen_register(SCREEN_END, &scr_end);
    screen_register(SCREEN_SCORES, &scr_scores);
    screen_register(SCREEN_HELP, &scr_help);
    screen_register(SCREEN_IMPEACH, &scr_impeach);
    screen_register(SCREEN_CAMPAIGN, &scr_campaign);
    screen_register(SCREEN_MANDATE2, &scr_mandate2);
    screen_register(SCREEN_BUDGET, &scr_budget);
    screen_register(SCREEN_REFERENDUM, &scr_referendum);
    screen_register(SCREEN_RESHUFFLE, &scr_reshuffle);
    screen_register(SCREEN_MAP, &scr_map);
    screen_register(SCREEN_OPTIONS, &scr_options);
    screen_register(SCREEN_ELEC, &scr_elec);
    screen_run(SCREEN_TITLE);
    return 0;
}
