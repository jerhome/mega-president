#ifndef SIM_SIM_H
#define SIM_SIM_H

// API principale du cœur de simulation (tâches 2.1, 2.2, 2.4).
// Formules et constantes : docs/SIM.md.

#include "sim/sim_state.h"
#include "sim/effects.h"
#include "sim/laws.h"
#include "sim/parliament.h"
#include "sim/election.h"

#define SIM_TURNS_PER_YEAR_V0 4
#define SIM_MANDATE_YEARS     5

// Initialise un pays : orientation ORIENT_*, graine du hasard.
void sim_init(Country *c, u8 orientation, u32 seed, u8 turns_per_year);

// Popularité globale (dixièmes de %) : 40 % populaires, 40 % moyennes, 20 % aisées.
s16 sim_popularity(const Country *c);

// Nombre total de tours du mandat (20 en V0).
u8 sim_mandate_turns(const Country *c);
// 1 quand le mandat est terminé (présidentielle à organiser).
u8 sim_mandate_over(const Country *c);
// 1 si le tour courant termine un trimestre (toujours vrai en V0).
u8 sim_quarter_tick(const Country *c);

// Propose une loi : paie le CP, fait voter l'Assemblée ; si adoptée, applique
// les effets immédiats et met les effets différés en attente.
// Renvoie LAW_OK (vote effectué, voir out->adopted) ou LAW_ERR_*.
u8 sim_propose_law(Country *c, u8 law_id, VoteResult *out);

// Compte rendu d'une fin de tour (pour le journal).
typedef struct {
    u8  n_matured;
    u8  matured[SIM_MAX_PENDING];   // lois dont les effets se sont appliqués
    u8  cp_gain;
    s16 popularity_before;
    s16 popularity_after;
} TurnReport;

// Fin de tour, dans l'ordre : lois arrivées à maturité, économie, opinion,
// Parlement, capital politique. Les effets d'événements sont appliqués avant
// par l'appelant avec sim_apply_effects(). report peut être NULL.
void sim_end_turn(Country *c, TurnReport *report);

// Étapes séparées (exposées pour les tests).
void sim_economy_update(Country *c);
void sim_opinion_update(Country *c);
s16  sim_opinion_target(const Country *c, u8 electorate);
u8   sim_cp_gain(const Country *c);

// Progression moyenne des piliers depuis le début (dixièmes de point).
s16 sim_pillar_progress(const Country *c);

#endif
