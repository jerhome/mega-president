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
    screen_run(SCREEN_TITLE);
    return 0;
}
