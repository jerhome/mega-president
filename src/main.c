// Mega Président — point d'entrée.
// Enregistre les écrans du jeu (src/screens/) et lance la boucle principale.
// Ce fichier n'appelle jamais VDP_* / PAL_* : tout passe par src/ui/.

#include <genesis.h>
#include "ui/screen.h"
#include "screens/screens.h"

int main(bool hard)
{
    (void)hard;
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
