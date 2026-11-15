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

// Accroche de l'écran titre : renvoie l'indice suivant (0..n-1) et le mémorise,
// pour changer d'accroche à chaque démarrage. Octet libre entre les scores et la partie.
#define SRAM_TAGLINE_OFFSET 254
u8 sram_tagline_next(u8 n);

// Réglages de l'écran Options (save/settings.h) : 16 octets de 224 à 239.
// Les octets 169-223 et 240-253 restent libres ; l'octet 254 n'est pas touché.
#define SRAM_SETTINGS_OFFSET 224
// Lit les réglages (SRAM vierge ou corrompue : défauts) ; renvoie le code de
// settings_deserialize. N'applique rien (voir settings_apply_all).
u8 sram_settings_load(void);
// Écrit les réglages et efface le drapeau « modifié ».
void sram_settings_save(void);

#endif
