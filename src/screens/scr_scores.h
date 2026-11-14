// Mega Président — écran des high scores (tâches 1.15, 1.16).
#ifndef SCR_SCORES_H
#define SCR_SCORES_H

#include <genesis.h>

// À appeler à la fin d'une partie (hors destitution), AVANT screen_change(SCREEN_SCORES).
//   score_mandat1 : score du 1er mandat (tableau « 1er mandat »)
//   score_cumul   : score cumulé sur 10 ans (tableau « cumul »), pris en compte
//                   seulement si two_mandates est non nul
//   orientation   : ORIENT_*, year : année de fin de partie
// Si aucun score n'entre dans un classement, l'écran affiche simplement les tableaux.
// Sinon l'écran demande les 3 initiales, enregistre en SRAM puis affiche le classement.
void scores_offer(u16 score_mandat1, u16 score_cumul, u8 two_mandates, u8 orientation, u16 year);

#endif
