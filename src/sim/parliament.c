#include "sim/parliament.h"
#include "sim/sim.h"

const s8  group_orientation[GRP_COUNT] = { -80, -40, 0, 40, 80 };
const u16 group_seats_start[GRP_COUNT] = { 75, 140, 160, 120, 82 };

s16 parl_affinity(s8 law_orientation, s8 group_orient)
{
    s16 diff = sim_abs16((s16)((s16)law_orientation - group_orient));
    return (s16)(50 - (diff >> 1));
}

// Score de vote d'un groupe : relation + affinité, l'affinité comptant triple
// au-delà de 30 (loi très marquée dans le sens du groupe), moins la défiance
// et le scandale pour les groupes autres que le parti du joueur.
s16 parl_group_score(const Country *c, u8 g, s8 law_orientation)
{
    s16 aff = parl_affinity(law_orientation, group_orientation[g]);
    s32 s = (s32)c->relation[g] + aff;
    if (aff > VOTE_STRONG_AFF) s += (s32)(aff - VOTE_STRONG_AFF) * 2;
    if (g != c->player_group) {
        if (c->gauges[JAUGE_DEFIANCE] > 20) s -= (c->gauges[JAUGE_DEFIANCE] - 20) / 4;
        s -= c->gauges[JAUGE_SCANDALE] / 10;
    }
    return (s16)s;
}

s16 parl_threshold(const Country *c, u8 g)
{
    if (g != c->player_group && !sim_has_majority(c)) return VOTE_THRESHOLD + VOTE_RELATIVE_MALUS;
    return VOTE_THRESHOLD;
}

void parl_vote(const Country *c, s8 law_orientation, VoteResult *out)
{
    u8 g;
    out->votes_pour = 0;
    out->votes_contre = 0;
    for (g = 0; g < GRP_COUNT; g++) {
        s16 s = parl_group_score(c, g, law_orientation);
        out->score[g] = s;
        out->pour[g] = (u8)(s >= parl_threshold(c, g));
        if (out->pour[g]) out->votes_pour += c->seats[g];
        else out->votes_contre += c->seats[g];
    }
    out->adopted = (u8)(out->votes_pour >= SEATS_MAJORITY);
}

void parl_after_vote(Country *c, s8 law_orientation, const VoteResult *v)
{
    u8 g;
    for (g = 0; g < GRP_COUNT; g++) {
        s16 d;
        if (v->adopted) {
            // Les groupes proches de la loi apprécient, les éloignés s'irritent.
            d = (s16)(parl_affinity(law_orientation, group_orientation[g]) / 10);
        } else {
            // Rejet : les soutiens sont déçus, l'opposition ne bouge pas.
            d = v->pour[g] ? -2 : 0;
        }
        c->relation[g] = (u8)sim_clamp((s32)c->relation[g] + d, 0, RELATION_MAX);
    }
}

void parl_end_turn(Country *c)
{
    // Dérive de 1 point par trimestre vers la relation de base,
    // décalée par la popularité (± ~6 points).
    s16 popmod = (s16)((sim_popularity(c) - 450) / 25);
    u8 g;
    if (!sim_quarter_tick(c)) return;
    for (g = 0; g < GRP_COUNT; g++) {
        s16 target = sim_clamp((s32)c->relation_base[g] + popmod
                               + (c->pm == PM_RASSEMBLEUR ? RASSEMBLEUR_REL : 0), 0, RELATION_MAX);
        if (c->relation[g] < target) c->relation[g]++;
        else if (c->relation[g] > target) c->relation[g]--;
    }
}

u16 parl_support_seats(const Country *c)
{
    u16 s = 0;
    u8 g;
    for (g = 0; g < GRP_COUNT; g++)
        if (c->relation[g] >= 50) s += c->seats[g];
    return s;
}
