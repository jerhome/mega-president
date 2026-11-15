// Mega Président — sauvegarde de partie en SRAM (console seulement).
// Emplacement unique à partir de l'octet SRAM 256 (les high scores occupent 0..168).
#ifndef SAVE_SRAM_GAME_H
#define SAVE_SRAM_GAME_H

#include "game/game.h"

#define SRAM_GAME_OFFSET 256

// Écrit la partie (à appeler en fin de trimestre).
void sram_game_save(const Game *g);
// Charge la partie ; renvoie 1 si une sauvegarde valide a été chargée
// (g n'est modifié que dans ce cas).
u8 sram_game_load(Game *g);
// 1 si une sauvegarde valide existe (compatible avec le jeu actuel).
u8 sram_game_exists(void);
// Efface la sauvegarde (fin de partie).
void sram_game_erase(void);
// 1 si une sauvegarde d'une version précédente est présente (donc ignorée).
u8 sram_game_stale(void);

#endif
