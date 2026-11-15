// Mega Président — kit d'écran : bandeau titre + CP, ligne d'erreur d'action,
// sièges par groupe. Évite la copie de ces blocs dans chaque écran (audit V4.1).
#ifndef SCR_KIT_H
#define SCR_KIT_H

#include <genesis.h>
#include "sim/laws.h"

// Bandeau du haut : titre (ou date du tour si NULL) à gauche, CP à droite.
void kit_header(const char *title);

// Page complète sans apparition douce : art_reset_quiet() puis kit_header(title).
void kit_page(const char *title);

// Cause d'un refus d'action.
typedef enum {
    KIT_ERR_NONE,      // aucune erreur : rien n'est dessiné
    KIT_ERR_CP,        // capital politique insuffisant
    KIT_ERR_ADOPTED,   // loi déjà adoptée
    KIT_ERR_QUEUE,     // file des lois en attente pleine
    KIT_ERR_493,       // 49.3 déjà utilisé cette année
    KIT_ERR_COHAB,     // cohabitation : loi réservée au gouvernement
    KIT_ERR_OTHER      // impossible pour une autre raison
} KitErr;

// Convertit un LAW_ERR_* de la simulation (LAW_OK -> KIT_ERR_NONE).
KitErr kit_err_from_law(u8 law_err);

// Message de l'erreur sur la ligne y (38 colonnes ; COHAB sur 2 lignes).
void kit_err_line(u16 y, KitErr err);

// Sièges par groupe sur deux lignes (y et y + 1) : coche ou croix et sigle, puis
// sièges (« avant→après » si before). pour, before et seats peuvent être NULL.
void kit_group_seats(u16 y, const u16 *seats, const u16 *before, const u8 *pour);

#endif
