// Mega Président — accès à la SRAM (console seulement, utilise SGDK).
// La SRAM est déclarée dans src/boot/rom_head.c (octets impairs, 0x200001+).
#ifndef SAVE_SRAM_H
#define SAVE_SRAM_H

#include <genesis.h>
#include "save/scores.h"

// Lit les classements ; SRAM vide ou corrompue : tableaux réinitialisés
// (jamais de plantage). Renvoie le nombre de tableaux réinitialisés.
u8 sram_scores_load(ScoreBoard *b);
// Écrit les classements.
void sram_scores_save(const ScoreBoard *b);

#endif
