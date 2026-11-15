#include "sim/cohab.h"
#include "sim/sim.h"

u8 sim_cohabitation(const Country *c)
{
    return c->cohab;
}

u8 sim_law_sovereign(u8 law_id)
{
    return (u8)(law_id < LOI_COUNT && law_defs[law_id].sovereign);
}

u8 sim_cohab_camp(const Country *c)
{
    if (!c->cohab) return ORIENT_CENTRE;
    if (c->cohab_group <= GRP_G2) return ORIENT_GAUCHE;
    if (c->cohab_group == GRP_G3) return ORIENT_CENTRE;
    return ORIENT_DROITE;
}

// Préréglage le plus proche du plan du PM (descriptif ; le plan exact est sim_cohab_plan).
u8 sim_cohab_budget(const Country *c)
{
    switch (sim_cohab_camp(c)) {
    case ORIENT_GAUCHE: return BUDGET_RELANCE;
    case ORIENT_DROITE: return BUDGET_RIGUEUR;
    default:            return BUDGET_EQUILIBRE;
    }
}

void cohab_start(Country *c)
{
    u8 g, best = GRP_COUNT;
    if (c->cohab) return;
    // Le Premier ministre vient du groupe d'opposition le plus nombreux
    // (groupes hors du bloc présidentiel).
    for (g = 0; g < GRP_COUNT; g++)
        if (!sim_in_bloc(c, g) && (best == GRP_COUNT || c->seats[g] > c->seats[best]))
            best = g;
    c->cohab_group = best;
    c->pm_prev = c->pm;
    c->pm = PM_OPPOSITION;
    c->cohab = 1;
    c->cohab_def_adj = 0;
    if (c->cohabitations < 255) c->cohabitations++;
}

void cohab_end(Country *c)
{
    if (!c->cohab) return;
    c->cohab = 0;
    c->pm = c->pm_prev;
    c->pm_prev = PM_NONE;
}

// Loi que le gouvernement propose : la plus proche de son orientation parmi les
// lois non souveraines et non encore adoptées.
u8 sim_cohab_next_law(const Country *c)
{
    s16 best_aff = -1000;
    u8 id, best = LOI_COUNT;
    if (!c->cohab || c->n_pending >= SIM_MAX_PENDING) return LOI_COUNT;
    for (id = 0; id < LOI_COUNT; id++) {
        s16 aff;
        if (sim_law_sovereign(id) || law_is_adopted(c, id)) continue;
        aff = parl_affinity(law_defs[id].orientation, group_orientation[c->cohab_group]);
        if (aff > best_aff) { best_aff = aff; best = id; }
    }
    return best;
}

u8 cohab_government_quarter(Country *c)
{
    u8 id;
    if (!c->cohab || c->status != STATUS_PLAYING) return 0;
    if (rng_below(&c->rng, 100) >= COHAB_LAW_PCT) return 0;
    id = sim_cohab_next_law(c);
    if (id >= LOI_COUNT) return 0;
    sim_law_adopt(c, id, 1);
    if (c->gov_laws < 255) c->gov_laws++;
    return (u8)(id + 1);
}
