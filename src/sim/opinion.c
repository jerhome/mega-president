#include "sim/sim.h"

// Opinion par électorat (tâche 2.4). Constantes : docs/SIM.md.
#define OPN_BASE        330
#define OPN_NOISE       8
#define GROWTH_REF      110
#define UNEMP_REF       750
#define TENSION_REF     20
#define GRACE_PER_YEAR  80   // érosion de l'état de grâce (dixièmes de % par an)

// Affinité de l'électorat avec l'orientation du joueur.
static const s8 orient_affinity[ORIENT_COUNT][ELEC_COUNT] = {
    {  30,  0, -40 },   // gauche
    { -10, 20,  10 },   // centre
    { -40,  0,  40 },   // droite
};

// Sensibilité au chômage et à la croissance (dixièmes de % par point, / 10).
static const u8 k_unemp[ELEC_COUNT]  = { 5, 4, 3 };
static const u8 k_growth[ELEC_COUNT] = { 1, 2, 3 };

// Poids des piliers par électorat (somme = 16).
static const u8 pillar_weight[ELEC_COUNT][PIL_COUNT] = {
    // Soc Éco Tech Écol Séc
    {  6,  3,  1,  2,  4 },   // populaires
    {  3,  4,  2,  3,  4 },   // moyennes
    {  1,  7,  3,  2,  3 },   // aisées
};

s16 sim_opinion_target(const Country *c, u8 e)
{
    s32 t = OPN_BASE + orient_affinity[c->orientation][e];
    s32 pil = 0;
    u8 i;

    t -= ((s32)c->unemployment - UNEMP_REF) * k_unemp[e] / 10;
    t += ((s32)c->growth - GROWTH_REF) * k_growth[e] / 10;

    for (i = 0; i < PIL_COUNT; i++)
        pil += (s32)pillar_weight[e][i] * ((s32)c->pillars[i] - 500);
    t += (pil / 16) * 4 / 10;   // +10 points de pilier pondéré : +4 points d'opinion

    t -= ((s32)c->gauges[JAUGE_TENSION] - TENSION_REF) * 2;
    t += c->grace;
    return sim_clamp(t, OPINION_MIN, OPINION_MAX);
}

void sim_opinion_update(Country *c)
{
    s16 tpy = c->turns_per_year;
    u8 e;

    for (e = 0; e < ELEC_COUNT; e++) {
        s32 o = c->opinion[e];
        o += ((s32)sim_opinion_target(c, e) - o) / tpy;   // inertie : 1/4 par trimestre
        o += rng_range(&c->rng, -OPN_NOISE, OPN_NOISE);
        c->opinion[e] = sim_clamp(o, OPINION_MIN, OPINION_MAX);
    }

    // Érosion de l'état de grâce.
    c->grace = sim_clamp((s32)c->grace - GRACE_PER_YEAR / tpy, 0, 1000);
}
