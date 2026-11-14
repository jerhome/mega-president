#ifndef SIM_SENATE_H
#define SIM_SENATE_H

// Sénat (V2, tâche 2.05) : 348 sièges, mêmes groupes G1-G5 que l'Assemblée,
// composition plus stable. Renouvelé par moitié tous les 3 ans (années 3, 6 et 9
// de la partie) selon la popularité. Il compte dans la destitution : vote aux
// deux tiers de l'Assemblée ET du Sénat (REGLES §10).

#include "sim/sim_state.h"

#define SENATE_IMPEACH_MAJORITY  232   // deux tiers de 348
#define SENATE_RENEW_YEARS       3     // renouvellement par moitié tous les 3 ans

// Compose le Sénat de départ (difficulté et orientation via les sièges de l'Assemblée).
// Appelé par sim_init_ex, après l'initialisation de c->seats.
void sim_senate_init(Country *c);

// Sièges du bloc présidentiel au Sénat (parti du joueur + groupes voisins).
u16 sim_senate_bloc_seats(const Country *c);
// 1 si le bloc présidentiel a la majorité absolue au Sénat (175 sièges).
u8  sim_senate_has_majority(const Country *c);
// Sièges des groupes dont la relation >= 50 (soutien).
u16 sim_senate_support_seats(const Country *c);

// Voix pour la destitution au Sénat si le vote avait lieu maintenant (aperçu).
u16 sim_impeach_senate_votes(const Country *c);
// 1 si la destitution serait votée maintenant (Assemblée >= 385 ET Sénat >= 232).
u8  sim_impeach_would_pass(const Country *c);

// 1 si le Sénat est renouvelé au début du tour courant (année de renouvellement).
u8  sim_senate_renewal_due(const Country *c);
// Tours restants avant le prochain renouvellement dans ce mandat (0 : aucun).
u8  sim_senate_renewal_in(const Country *c);

// Renouvelle la moitié du Sénat selon la popularité (inertie forte). before
// (peut être NULL) reçoit les sièges d'avant. Appelé par sim_end_turn.
void sim_senate_renew(Country *c, u16 *before);

#endif
