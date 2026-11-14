#include "sim/sim.h"

// Opinion de départ par orientation (lendemain d'élection).
static const s16 opinion_start[ORIENT_COUNT][ELEC_COUNT] = {
    { 580, 530, 420 },   // gauche
    { 520, 560, 520 },   // centre
    { 450, 540, 600 },   // droite
};

// Relation de départ selon la distance au parti du joueur (contrat §4).
static const u8 relation_by_distance[4] = { 80, 55, 30, 15 };

#define CP_START      5
#define GRACE_START   120   // +12 points d'opinion cible au début du mandat

void sim_init(Country *c, u8 orientation, u32 seed, u8 turns_per_year)
{
    u8 i;
    u16 k;
    u8 *raw = (u8 *)c;
    for (k = 0; k < sizeof(Country); k++) raw[k] = 0;   // pas de memset (pas de libc)

    if (orientation >= ORIENT_COUNT) orientation = ORIENT_CENTRE;
    if (turns_per_year != 12) turns_per_year = SIM_TURNS_PER_YEAR_V0;

    rng_seed(&c->rng, seed);
    c->orientation = orientation;
    c->turns_per_year = turns_per_year;
    c->player_group = (u8)(GRP_G2 + orientation);

    for (i = 0; i < ELEC_COUNT; i++)
        c->opinion[i] = (s16)(opinion_start[orientation][i] + rng_range(&c->rng, -15, 15));

    c->growth = (s16)(120 + rng_range(&c->rng, -20, 20));
    c->unemployment = (s16)(750 + rng_range(&c->rng, -30, 30));
    c->debt = 1100;
    for (i = 0; i < PIL_COUNT; i++)
        c->pillars[i] = (s16)(500 + rng_range(&c->rng, -20, 20));

    for (i = 0; i < PIL_COUNT; i++) c->pillars_start[i] = c->pillars[i];
    c->unemployment_start = c->unemployment;
    c->debt_start = c->debt;

    c->gauges[JAUGE_TENSION] = 20;
    c->gauges[JAUGE_SCANDALE] = 0;
    c->gauges[JAUGE_DEFIANCE] = 10;
    c->cp = CP_START;
    c->grace = GRACE_START;

    for (i = 0; i < GRP_COUNT; i++) {
        s16 dist = sim_abs16((s16)((s16)i - c->player_group));
        c->relation[i] = relation_by_distance[dist];
        c->relation_base[i] = c->relation[i];
        c->seats[i] = group_seats_start[i];
    }
}

s16 sim_popularity(const Country *c)
{
    s32 p = 4 * (s32)c->opinion[ELEC_POPULAIRES] + 4 * (s32)c->opinion[ELEC_MOYENNES]
          + 2 * (s32)c->opinion[ELEC_AISEES];
    return (s16)((p + 5) / 10);
}

u8 sim_mandate_turns(const Country *c)
{
    return (u8)(c->turns_per_year * SIM_MANDATE_YEARS);
}

u8 sim_mandate_over(const Country *c)
{
    return (u8)(c->turn >= sim_mandate_turns(c));
}

u8 sim_quarter_tick(const Country *c)
{
    // En mode mensuel, un trimestre se termine tous les 3 tours.
    if (c->turns_per_year <= 4) return 1;
    return (u8)((c->turn % 3) == 0);
}

s16 sim_pillar_progress(const Country *c)
{
    s16 sum = 0;
    u8 i;
    for (i = 0; i < PIL_COUNT; i++) sum += (s16)(c->pillars[i] - c->pillars_start[i]);
    return (s16)(sum / PIL_COUNT);
}

u8 sim_propose_law(Country *c, u8 law_id, VoteResult *out)
{
    const LawDef *law;
    u8 err = law_can_propose(c, law_id);
    if (err != LAW_OK) return err;
    law = law_get(law_id);

    c->cp = (u8)(c->cp - law->cost);
    parl_vote(c, law->orientation, out);
    parl_after_vote(c, law->orientation, out);

    if (out->adopted) {
        c->laws_adopted |= (u16)(1u << law_id);
        sim_apply_effects(c, law->immediate, law->n_immediate);
        c->pending[c->n_pending].law = law_id;
        c->pending[c->n_pending].turns_left = law_delay_turns(c, law);
        c->n_pending++;
    } else {
        // Échec : CP perdu, défiance en hausse, petit coût d'image.
        sim_apply_effect(c, EFF_DEFIANCE, 5);
        sim_apply_effect(c, EFF_POP_TOUS, -1);
    }
    return LAW_OK;
}

// Gain de CP par trimestre (REGLES §5).
#define CP_BASE 3

u8 sim_cp_gain(const Country *c)
{
    s16 pop = sim_popularity(c);
    u8 g = CP_BASE;
    if (pop >= 450) g++;
    if (pop >= 550) g++;
    if (parl_support_seats(c) >= SEATS_MAJORITY) g++;
    return g;
}

void sim_end_turn(Country *c, TurnReport *report)
{
    TurnReport tmp;
    TurnReport *r = report ? report : &tmp;

    r->popularity_before = sim_popularity(c);
    c->turn++;

    // 1. Lois arrivées à maturité
    r->n_matured = law_process_pending(c, r->matured);
    // 2. Économie
    sim_economy_update(c);
    // 3. Opinion
    sim_opinion_update(c);
    // 4. Parlement
    parl_end_turn(c);
    // 5. Capital politique (par trimestre)
    r->cp_gain = 0;
    if (sim_quarter_tick(c)) {
        u8 before = c->cp;
        sim_apply_effect(c, EFF_CP, (s8)sim_cp_gain(c));
        r->cp_gain = (u8)(c->cp - before);
    }
    r->popularity_after = sim_popularity(c);
}
