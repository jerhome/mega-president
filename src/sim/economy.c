#include "sim/sim.h"

// Dynamique économique (tâche 2.2). Constantes : docs/SIM.md.
#define TREND_BASE     110   // tendance de croissance hors piliers (1,10 %)
#define GROWTH_POT     110   // croissance potentielle (Okun)
#define GROWTH_NOISE   25    // aléa ±0,25 point par tour
#define UNEMP_STRUCT   750   // chômage structurel (rappel faible)
#define DEFICIT_BASE   45    // déficit primaire de base : 4,5 points de PIB par an
#define INFLATION      200   // 2,00 % (croissance nominale = réelle + inflation)
#define TENSION_BASE   20

// Tendance de croissance : base + effet des piliers Économie et Technologie.
static s16 growth_trend(const Country *c)
{
    return (s16)(TREND_BASE + (c->pillars[PIL_ECONOMIE] - 500) / 5
                            + (c->pillars[PIL_TECHNOLOGIE] - 500) / 10);
}

void sim_economy_update(Country *c)
{
    s16 tpy = c->turns_per_year;
    s32 v;

    // Croissance : retour vers la tendance (1/4 de l'écart par trimestre) + aléa.
    v = (s32)c->growth + ((s32)growth_trend(c) - c->growth) / tpy
      + rng_range(&c->rng, -GROWTH_NOISE, GROWTH_NOISE);
    c->growth = sim_clamp(v, GROWTH_MIN, GROWTH_MAX);

    // Chômage (loi d'Okun) : -0,5 point par point de croissance au-dessus du
    // potentiel et par an, plus un léger rappel vers le chômage structurel.
    v = (s32)c->unemployment - ((s32)c->growth - GROWTH_POT) / (2 * tpy)
      + ((s32)UNEMP_STRUCT - c->unemployment) / (10 * tpy);
    c->unemployment = sim_clamp(v, UNEMP_MIN, UNEMP_MAX);

    // Dette : variation annuelle = déficit - dette × croissance nominale.
    // Calcul en centièmes de point, avec report du reste d'un tour à l'autre.
    {
        s32 deficit = DEFICIT_BASE + ((s32)c->unemployment - 750) / 10;   // dixièmes/an
        s32 annual = deficit * 10 - (s32)c->debt * ((s32)c->growth + INFLATION) / 1000;
        s32 acc = (s32)c->debt_acc + annual;
        s32 div = 10 * (s32)tpy;
        s32 delta = acc / div;
        c->debt_acc = (s16)(acc - delta * div);
        c->debt = sim_clamp((s32)c->debt + delta, DEBT_MIN, DEBT_MAX);
    }

    // Tension sociale : cible selon le chômage et le pilier Social.
    {
        s32 target = TENSION_BASE + ((s32)c->unemployment - 750) / 25
                   - ((s32)c->pillars[PIL_SOCIAL] - 500) / 25;
        s32 t = c->gauges[JAUGE_TENSION];
        target = sim_clamp(target, 0, GAUGE_MAX);
        t += (target - t) / tpy;
        c->gauges[JAUGE_TENSION] = (u8)sim_clamp(t, 0, GAUGE_MAX);
    }
}
