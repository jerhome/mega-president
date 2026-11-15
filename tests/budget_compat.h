#ifndef BUDGET_COMPAT_H
#define BUDGET_COMPAT_H

// Anciennes fonctions à identifiant de préréglage, conservées pour les tests PC
// seulement (jamais compilées dans la ROM). Le jeu utilise sim_budget_plan_* et sim_init_ex.
#include "sim/sim.h"
#include "sim/budget.h"
#include "sim/cohab.h"

void sim_init(Country *c, u8 orientation, u32 seed, u8 turns_per_year);
s8  sim_budget_orientation(u8 budget);
s16 sim_budget_opinion(u8 budget, u8 electorate);
void sim_budget_preview(const Country *c, u8 budget, VoteResult *out);
u8  sim_budget_vote(Country *c, u8 budget, VoteResult *out);
u8  sim_budget_force(Country *c, u8 budget);
u8  sim_cohab_budget(const Country *c);

#endif
