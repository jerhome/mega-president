// Mega Président — écran Options (tâche 3.19).
#ifndef SCR_OPTIONS_H
#define SCR_OPTIONS_H

#include "ui/screen.h"

// Ouvre l'écran Options ; B ou START ramène à l'écran « return_to ». Depuis une
// partie (return_to != SCREEN_TITLE), « Effacer la partie » est grisée.
void options_open(ScreenId return_to);
// TRUE une seule fois après un retour de l'écran Options : l'appelant replace sa
// sélection sur sa ligne « Options ».
bool options_take_return(void);

#endif
