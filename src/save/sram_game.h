// Mega Président — sauvegarde de partie en SRAM (console seulement).
// Deux emplacements alternés à partir de l'octet SRAM 256 (les high scores occupent
// 0..168, les réglages 224..239, l'accroche 254) : une coupure pendant l'écriture
// laisse l'emplacement précédent intact (voir gamesave.h).
#ifndef SAVE_SRAM_GAME_H
#define SAVE_SRAM_GAME_H

#include "game/game.h"
#include "save/gamesave.h"

#define SRAM_GAME_OFFSET 256
#define SRAM_GAME_SLOT   ((u16)(GAMESAVE_HEADER + sizeof(Game)))
// SRAM de 64 Ko à octets impairs : 32768 octets utiles ; vérifié à la compilation.
typedef char sram_game_fits[(SRAM_GAME_OFFSET + 2UL * (GAMESAVE_HEADER + sizeof(Game)) <= 32768UL) ? 1 : -1];

// Écrit la partie dans l'emplacement alterné (à appeler en fin de trimestre).
void sram_game_save(const Game *g);
// Charge la partie la plus récente valide ; renvoie 1 si une sauvegarde valide
// (somme, empreinte, game_sanitize) a été chargée (g n'est modifié que dans ce cas).
u8 sram_game_load(Game *g);
// 1 si une sauvegarde valide existe (résultat mis en cache).
u8 sram_game_exists(void);
// Efface la sauvegarde (fin de partie), les deux emplacements.
void sram_game_erase(void);
// 1 si une sauvegarde d'une version précédente ou refusée est présente (donc ignorée).
u8 sram_game_stale(void);

#endif
