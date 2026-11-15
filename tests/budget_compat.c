// Compatibilité des tests PC : préréglages de budget et sim_init (hors ROM).
#include "budget_compat.h"

void sim_init(Country *c, u8 orientation, u32 seed, u8 turns_per_year)
{
    SimSetup s;
    s.orientation = orientation;
    s.pm = PM_NONE;
    s.difficulty = DIFF_NORMAL;
    s.turns_per_year = turns_per_year;
    s.seed = seed;
    sim_init_ex(c, &s);
}

// Orientation propre du plan, hors orientation du joueur (pays de centre).
s8 sim_budget_orientation(u8 b)
{
    static Country c;
    BudgetPlan p;
    sim_init(&c, ORIENT_CENTRE, 1, 4);
    sim_budget_preset_plan(b, &p);
    return sim_plan_orientation(&c, &p);
}

s16 sim_budget_opinion(u8 b, u8 e)
{
    BudgetPlan p;
    sim_budget_preset_plan(b, &p);
    return sim_plan_opinion(&p, e);
}

void sim_budget_preview(const Country *c, u8 b, VoteResult *out)
{
    BudgetPlan p;
    sim_budget_preset_plan(b, &p);
    sim_budget_plan_preview(c, &p, out);
}

u8 sim_budget_vote(Country *c, u8 b, VoteResult *out)
{
    BudgetPlan p;
    if (b >= BUDGET_COUNT) return BUDGET_ERR_ID;
    sim_budget_preset_plan(b, &p);
    return sim_budget_plan_vote(c, &p, out);
}

u8 sim_budget_force(Country *c, u8 b)
{
    BudgetPlan p;
    if (b >= BUDGET_COUNT) return BUDGET_ERR_ID;
    sim_budget_preset_plan(b, &p);
    return sim_budget_plan_force(c, &p);
}

u8 sim_cohab_budget(const Country *c)
{
    switch (sim_cohab_camp(c)) {
    case ORIENT_GAUCHE: return BUDGET_RELANCE;
    case ORIENT_DROITE: return BUDGET_RIGUEUR;
    default:            return BUDGET_EQUILIBRE;
    }
}
