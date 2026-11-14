#ifndef SIM_BUDGET_H
#define SIM_BUDGET_H

// Budget annuel (V1 simplifiée, tâche 1.06) : une décision par an parmi
// BUDGET_RIGUEUR, BUDGET_EQUILIBRE, BUDGET_RELANCE (sim/ids_v1.h).
// Présenté au 4e trimestre ; voté par l'Assemblée ou passé en 49.3.
// Sans budget adopté à la fin de l'année : reconduction (= BUDGET_EQUILIBRE).
// Le budget adopté s'applique pendant toute l'année suivante.

#include "sim/sim_state.h"
#include "sim/parliament.h"

#define BUDGET_493_CP 4

enum { BUDGET_OK = 0, BUDGET_ERR_NOT_DUE, BUDGET_ERR_ID, BUDGET_ERR_CP };

// 1 si le budget de l'année doit être présenté maintenant (4e trimestre,
// budget de l'année pas encore adopté, partie en cours).
u8 sim_budget_due(const Country *c);

// --- Aperçus (aucun effet de bord) ---
// Orientation politique d'un budget (-100 gauche ... +100 droite) : sert au vote.
s8  sim_budget_orientation(u8 budget);
// Déficit par rapport à l'équilibre (dixièmes de point de PIB par an).
s16 sim_budget_deficit(u8 budget);
// Effet sur la tendance de croissance (centièmes de point).
s16 sim_budget_growth(u8 budget);
// Effet sur la cible d'opinion d'un électorat (dixièmes de %).
s16 sim_budget_opinion(u8 budget, u8 electorate);
// Effet sur un pilier par trimestre (dixièmes de point).
s16 sim_budget_pillar(u8 budget, u8 pillar);
// Effet sur la cible de tension (points).
s16 sim_budget_tension(u8 budget);
// Aperçu du vote de l'Assemblée.
void sim_budget_preview(const Country *c, u8 budget, VoteResult *out);

// --- Actions ---
// Vote du budget. BUDGET_OK : vote effectué (voir out->adopted) ; adopté, il
// entre en vigueur. Rejeté : défiance +3 ; on peut retenter, passer en 49.3,
// ou laisser la reconduction s'appliquer en fin d'année.
u8 sim_budget_vote(Country *c, u8 budget, VoteResult *out);
// 49.3 sur le budget : 4 CP, défiance +8, illimité.
u8 sim_budget_force(Country *c, u8 budget);

// Interne (fin de tour) : dérive des piliers, une fois par trimestre.
void budget_quarter_update(Country *c);

#endif
