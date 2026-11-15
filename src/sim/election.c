#include "sim/election.h"
#include "sim/sim.h"

const s8 cand_orientation[CAND_POOL_COUNT] = { -80, -50, 0, 40, 80 };

// Poids de base des adversaires au 1er tour (avant le score du joueur).
static const s16 cand_weight[CAND_POOL_COUNT] = { 110, 90, 150, 160, 150 };

// Constantes d'équilibrage (docs/SIM.md)
#define R1_BASE        60     // socle du joueur au 1er tour
#define R1_POP_NUM     1      // + popularité / 2
#define R1_POP_DEN     2
#define R1_MIN         40
#define R1_MAX         600
#define R1_NOISE       25
#define R2_POP_REF     510    // popularité « neutre » au 2nd tour
#define R2_SLOPE       8      // +0,8 point au 2nd tour par point de popularité
#define R2_NOISE       80
#define R2_OPP_REF     200    // score de 1er tour « neutre » de l'adversaire
#define R2_MIN         150
#define R2_MAX         850
#define FRONT_REP_NAT  20     // bonus contre le candidat nationaliste
#define FRONT_REP_GR   20     // bonus contre la gauche radicale

s8 sim_player_orientation(const Country *c)
{
    return (s8)((s16)c->orientation * 40 - 40);
}

void election_opponents(u8 orientation, u8 *out)
{
    u8 excluded = (orientation == ORIENT_DROITE) ? CAND_DROITE : CAND_CENTRE;
    u8 i, n = 0;
    for (i = 0; i < CAND_POOL_COUNT; i++)
        if (i != excluded) out[n++] = i;
}

// Indicateur de bilan commun aux deux tours (dixièmes de %, centré sur 0).
static s16 record_bonus(const Country *c)
{
    s32 b = ((s32)c->unemployment_start - c->unemployment) / 8   // -1 pt de chômage : +12,5
          + sim_pillar_progress(c) / 3;                           // +10 pts de pilier : +33
    return sim_clamp(b, -150, 150);
}

static s16 front_bonus(u8 opponent)
{
    if (opponent == CAND_NATIONALISTE) return FRONT_REP_NAT;
    if (opponent == CAND_GAUCHE_RADICALE) return FRONT_REP_GR;
    return 0;
}

s16 sim_campaign_popularity(const Country *c)
{
    s32 o[ELEC_COUNT];
    u8 e;
    for (e = 0; e < ELEC_COUNT; e++)
        o[e] = (s32)c->opinion[e] + sim_theme_bonus(c, c->theme, e);
    {
        s32 tot = 0;
        for (e = 0; e < ELEC_COUNT; e++) tot += (s32)elec_weight[e] * o[e];
        return sim_clamp((tot + 50) / 100, 0, 1000);
    }
}

void sim_election(Country *c, ElectionResult *out)
{
    s8 porient = sim_player_orientation(c);
    s16 pop = sim_campaign_popularity(c);
    s16 rec = record_bonus(c);
    s16 w[ELECTION_CANDIDATES];
    s32 wsum = 0, rest, given = 0;
    u8 i, best, second;

    // --- 1er tour ---
    out->cand[0] = CAND_JOUEUR;
    election_opponents(c->orientation, &out->cand[1]);

    out->round1[0] = sim_clamp((s32)R1_BASE + (s32)pop * R1_POP_NUM / R1_POP_DEN + rec
                               + rng_range(&c->rng, -R1_NOISE, R1_NOISE), R1_MIN, R1_MAX);

    for (i = 1; i < ELECTION_CANDIDATES; i++) {
        u8 k = out->cand[i];
        s16 dist = sim_abs16((s16)((s16)cand_orientation[k] - porient));
        s32 wi;
        if (dist > 80) dist = 80;
        // Un candidat proche du joueur est affaibli : poids × (dist + 40) / 120.
        wi = (s32)cand_weight[k] * (dist + 40) / 120;
        wi += rng_range(&c->rng, -20, 20);
        if (wi < 10) wi = 10;
        w[i] = (s16)wi;
        wsum += wi;
    }
    rest = 1000 - out->round1[0];
    best = 1;
    for (i = 1; i < ELECTION_CANDIDATES; i++) {
        out->round1[i] = (s16)(rest * w[i] / wsum);
        given += out->round1[i];
        if (w[i] > w[best]) best = i;
    }
    out->round1[best] = (s16)(out->round1[best] + (rest - given));   // somme exacte

    // Deux premiers (à égalité, le premier dans l'ordre l'emporte : le joueur).
    best = 0;
    for (i = 1; i < ELECTION_CANDIDATES; i++)
        if (out->round1[i] > out->round1[best]) best = i;
    second = (best == 0) ? 1 : 0;
    for (i = 0; i < ELECTION_CANDIDATES; i++)
        if (i != best && out->round1[i] > out->round1[second]) second = i;
    out->finalist[0] = out->cand[best];
    out->finalist[1] = out->cand[second];

    // --- 2nd tour ---
    if (best == 0 || second == 0) {
        u8 oi = (u8)(best == 0 ? second : best);
        u8 opp = out->cand[oi];
        // Un adversaire fort au 1er tour l'est aussi au 2nd.
        s16 p = sim_clamp(500 + ((s32)pop - R2_POP_REF) * R2_SLOPE / 10 + rec / 2
                          - ((s32)out->round1[oi] - R2_OPP_REF) / 2
                          + front_bonus(opp) + rng_range(&c->rng, -R2_NOISE, R2_NOISE),
                          R2_MIN, R2_MAX);
        if (best == 0) { out->round2[0] = p; out->round2[1] = (s16)(1000 - p); }
        else           { out->round2[0] = (s16)(1000 - p); out->round2[1] = p; }
        out->elected = (u8)(p > 500);
    } else {
        // Joueur éliminé : duel entre les deux adversaires.
        s16 a = (s16)(500 + (out->round1[best] - out->round1[second]) / 2
                      + front_bonus(out->cand[second]) - front_bonus(out->cand[best])
                      + rng_range(&c->rng, -R2_NOISE, R2_NOISE));
        a = sim_clamp(a, R2_MIN, R2_MAX);
        out->round2[0] = a;
        out->round2[1] = (s16)(1000 - a);
        out->elected = 0;
    }
}

u16 sim_mandate_score(const Country *c, u8 reelected)
{
    s32 s = sim_popularity(c);                 // popularité (%) × 10
    u8 i;
    for (i = 0; i < PIL_TOTAL; i++)            // progression (points) × 5
        s += ((s32)c->pillars[i] - c->pillars_start[i]) / 2;
    // Bonus économiques : 50 par point de chômage en moins, 5 par point de dette évité.
    s += ((s32)c->unemployment_start - c->unemployment) / 2;
    s += ((s32)c->debt_start - c->debt) / 2;
    if (reelected) s += 500;
    if (s < 0) s = 0;
    if (s > 65535) s = 65535;
    return (u16)s;
}
