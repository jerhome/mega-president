#include "sim/budget.h"
#include "sim/sim.h"

// Budget annuel simplifié (V1). Constantes : docs/SIM.md.
// Tables indexées par BUDGET_RIGUEUR, BUDGET_EQUILIBRE, BUDGET_RELANCE.

static const s8  bud_orient[BUDGET_COUNT]  = { 40, 0, -40 };
static const s8  bud_deficit[BUDGET_COUNT] = { -15, 0, 15 };   // dixièmes de pt/an
static const s8  bud_growth[BUDGET_COUNT]  = { -30, 0, 30 };   // centièmes
static const s8  bud_tension[BUDGET_COUNT] = { 6, 0, -5 };     // points
// Opinion (dixièmes de %) : populaires, moyennes, aisées
static const s8  bud_opinion[BUDGET_COUNT][ELEC_COUNT] = {
    { -20,  -5,  15 },
    {   0,   0,   0 },
    {  20,  10, -15 },
};
// Piliers par trimestre (dixièmes) : Soc Éco Tech Écol Séc
static const s8  bud_pillar[BUDGET_COUNT][PIL_TOTAL] = {
    { -5,  2,  0, -1,  0 },
    {  0,  0,  0,  0,  0 },
    {  5, -1,  2,  2,  1 },
};

static u8 bud_ok(u8 b) { return (u8)(b < BUDGET_COUNT); }

u8 sim_budget_due(const Country *c)
{
    if (c->status != STATUS_PLAYING || c->budget_done) return 0;
    if (sim_mandate_over(c)) return 0;
    if (c->cohab) return 0;   // V2 : le budget est décidé par le Premier ministre d'opposition
    return (u8)(sim_quarter(c) == 3);
}

s8  sim_budget_orientation(u8 b) { return bud_ok(b) ? bud_orient[b] : 0; }
s16 sim_budget_deficit(u8 b)     { return bud_ok(b) ? bud_deficit[b] : 0; }
s16 sim_budget_growth(u8 b)      { return bud_ok(b) ? bud_growth[b] : 0; }
s16 sim_budget_tension(u8 b)     { return bud_ok(b) ? bud_tension[b] : 0; }

s16 sim_budget_opinion(u8 b, u8 e)
{
    return (bud_ok(b) && e < ELEC_COUNT) ? bud_opinion[b][e] : 0;
}

s16 sim_budget_pillar(u8 b, u8 p)
{
    return (bud_ok(b) && p < PIL_TOTAL) ? bud_pillar[b][p] : 0;
}

// Le budget porte la couleur du gouvernement : orientation du joueur / 2
// + orientation propre du budget.
static s8 vote_orientation(const Country *c, u8 b)
{
    return (s8)sim_clamp((s32)sim_player_orientation(c) / 2 + sim_budget_orientation(b),
                         -100, 100);
}

void sim_budget_preview(const Country *c, u8 b, VoteResult *out)
{
    parl_vote(c, vote_orientation(c, b), out);
}

static void budget_adopt(Country *c, u8 b)
{
    c->budget = b;
    c->budget_done = 1;
}

u8 sim_budget_vote(Country *c, u8 b, VoteResult *out)
{
    s8 o;
    if (!bud_ok(b)) return BUDGET_ERR_ID;
    if (!sim_budget_due(c)) return BUDGET_ERR_NOT_DUE;
    o = vote_orientation(c, b);
    parl_vote(c, o, out);
    if (out->adopted) {
        budget_adopt(c, b);
    } else {
        sim_apply_effect(c, EFF_DEFIANCE, 3);
    }
    return BUDGET_OK;
}

u8 sim_budget_force(Country *c, u8 b)
{
    u8 g;
    if (!bud_ok(b)) return BUDGET_ERR_ID;
    if (!sim_budget_due(c)) return BUDGET_ERR_NOT_DUE;
    if (c->cp < BUDGET_493_CP) return BUDGET_ERR_CP;
    c->cp = (u8)(c->cp - BUDGET_493_CP);
    budget_adopt(c, b);
    sim_apply_effect(c, EFF_DEFIANCE, 8);
    for (g = 0; g < GRP_COUNT; g++)
        if (g != c->player_group)
            c->relation[g] = (u8)sim_clamp((s32)c->relation[g] - 1, 0, RELATION_MAX);
    return BUDGET_OK;
}

void budget_quarter_update(Country *c)
{
    u8 p;
    for (p = 0; p < PIL_TOTAL; p++)
        c->pillars[p] = sim_clamp((s32)c->pillars[p] + sim_budget_pillar(c->budget, p),
                                  PILLAR_MIN, PILLAR_MAX);
}
