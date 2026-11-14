#ifndef SIM_COHAB_H
#define SIM_COHAB_H

// Cohabitation (V2, tâche 2.04, REGLES §8). Elle commence quand le bloc
// présidentiel perd des législatives (dissolution ou début du 2nd mandat) sans
// les gagner : un Premier ministre d'opposition (PM_OPPOSITION) est nommé.
// Pendant la cohabitation :
//  - le joueur ne peut proposer que des lois de souveraineté (law_can_propose
//    renvoie LAW_ERR_COHAB pour les autres) ; le référendum reste possible ;
//  - les autres lois sont proposées par le gouvernement (sim_end_turn, une
//    chance sur deux par trimestre, sans CP, effets immédiats d'opinion divisés
//    par deux) ;
//  - le budget est décidé par le Premier ministre (sim_cohab_budget) ;
//  - pas de remaniement, pas de motion de censure ;
//  - la popularité du président est moins exposée : les cibles d'opinion sous
//    COHAB_OPINION_REF perdent un tiers de leur écart.
// Elle prend fin à la première législative gagnée (majorité, ou sièges gagnés).

#include "sim/sim_state.h"
#include "sim/laws.h"

// Lois de souveraineté (masque de bits LOI_*) : police, immigration, énergie,
// recherche (CIR, défense et industrie).
#define COHAB_SOVEREIGN_MASK  ((u16)((1u << LOI_POLICE) | (1u << LOI_IMMIGRATION) \
                                     | (1u << LOI_ENERGIE) | (1u << LOI_CIR)))
#define COHAB_LAW_PCT         50    // % de chance par trimestre d'une loi du gouvernement
#define COHAB_OPINION_REF     420   // cible d'opinion sous laquelle la baisse est amortie

// 1 pendant une cohabitation.
u8  sim_cohabitation(const Country *c);
// 1 si la loi est une loi de souveraineté (réservée au président en cohabitation).
u8  sim_law_sovereign(u8 law_id);
// Camp du Premier ministre d'opposition (ORIENT_GAUCHE, CENTRE ou DROITE) ; ORIENT_CENTRE hors cohabitation.
u8  sim_cohab_camp(const Country *c);
// Budget que le Premier ministre d'opposition impose en fin d'année (aperçu).
u8  sim_cohab_budget(const Country *c);
// Prochaine loi que le gouvernement proposerait (aperçu) : LOI_* ou LOI_COUNT si aucune.
u8  sim_cohab_next_law(const Country *c);

// Interne.
void cohab_start(Country *c);
void cohab_end(Country *c);
// Proposition du gouvernement (par trimestre) : renvoie id + 1 de la loi adoptée, 0 sinon.
u8  cohab_government_quarter(Country *c);

#endif
