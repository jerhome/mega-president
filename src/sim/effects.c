#include "sim/effects.h"

s16 sim_effect_scale(u8 key)
{
    switch (key) {
    case EFF_POP_POPULAIRES: case EFF_POP_MOYENNES:
    case EFF_POP_AISEES: case EFF_POP_TOUS:
        return 10;   // points de % -> dixièmes de %
    case EFF_CROISSANCE: case EFF_CHOMAGE:
        return 10;   // dixièmes de point -> centièmes
    case EFF_DETTE:
        return 5;    // 1 unité = 0,5 point de PIB (V1, équilibrage 1.20)
    case EFF_SOCIAL: case EFF_ECONOMIE: case EFF_TECHNOLOGIE:
    case EFF_ECOLOGIE: case EFF_SECURITE:
        return 10;   // points de pilier -> dixièmes
    case EFF_SCANDALE:
        return 2;    // 1 unité = 2 points de jauge (V1, équilibrage 1.20)
    case EFF_TENSION: case EFF_DEFIANCE:
    case EFF_CP:
    case EFF_REL_G1: case EFF_REL_G2: case EFF_REL_G3:
    case EFF_REL_G4: case EFF_REL_G5: case EFF_REL_PARTI:
        return 1;
    default:
        return 0;
    }
}

static void add16(s16 *p, s32 d, s16 lo, s16 hi)
{
    *p = sim_clamp((s32)*p + d, lo, hi);
}

static void add8(u8 *p, s32 d, s16 hi)
{
    *p = (u8)sim_clamp((s32)*p + d, 0, hi);
}

void sim_apply_effect(Country *c, u8 key, s8 value)
{
    s32 d = (s32)value * sim_effect_scale(key);
    u8 i;

    switch (key) {
    case EFF_POP_POPULAIRES: case EFF_POP_MOYENNES: case EFF_POP_AISEES:
        add16(&c->opinion[key - EFF_POP_POPULAIRES], d, OPINION_MIN, OPINION_MAX);
        break;
    case EFF_POP_TOUS:
        for (i = 0; i < ELEC_COUNT; i++)
            add16(&c->opinion[i], d, OPINION_MIN, OPINION_MAX);
        break;
    case EFF_CROISSANCE:
        add16(&c->growth, d, GROWTH_MIN, GROWTH_MAX);
        break;
    case EFF_CHOMAGE:
        add16(&c->unemployment, d, UNEMP_MIN, UNEMP_MAX);
        break;
    case EFF_DETTE:
        add16(&c->debt, d, DEBT_MIN, DEBT_MAX);
        break;
    case EFF_SOCIAL: case EFF_ECONOMIE: case EFF_TECHNOLOGIE:
    case EFF_ECOLOGIE: case EFF_SECURITE:
        add16(&c->pillars[key - EFF_SOCIAL], d, PILLAR_MIN, PILLAR_MAX);
        break;
    case EFF_TENSION: case EFF_SCANDALE: case EFF_DEFIANCE:
        add8(&c->gauges[key - EFF_TENSION], d, GAUGE_MAX);
        break;
    case EFF_CP:
        add8(&c->cp, d, CP_MAX);
        break;
    case EFF_REL_G1: case EFF_REL_G2: case EFF_REL_G3:
    case EFF_REL_G4: case EFF_REL_G5:
        add8(&c->relation[key - EFF_REL_G1], d, RELATION_MAX);
        break;
    case EFF_REL_PARTI:
        if (c->player_group < GRP_COUNT)
            add8(&c->relation[c->player_group], d, RELATION_MAX);
        break;
    default:
        break;   // clé inconnue : ignorée
    }
}

void sim_apply_effects(Country *c, const Effect *effects, u8 count)
{
    u8 i;
    for (i = 0; i < count; i++)
        sim_apply_effect(c, effects[i].key, effects[i].value);
}
