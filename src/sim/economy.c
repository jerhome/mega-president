#include "sim/sim.h"

// Dynamique économique (tâche 2.2, V1). Constantes : docs/SIM.md.
#define TREND_BASE     110   // tendance de croissance hors piliers (1,10 %)
#define GROWTH_POT     110   // croissance potentielle (Okun)
#define GROWTH_NOISE   25    // aléa ±0,25 point par tour (4 tours/an)
#define GROWTH_NOISE_M 14    // aléa par tour à 12 tours/an (≈ 25 / √3)
#define UNEMP_STRUCT   750   // chômage structurel (rappel faible)
#define DEFICIT_BASE   45    // déficit primaire de base : 4,5 points de PIB par an
#define DEBT_REF       1100  // au-delà, consolidation budgétaire spontanée
#define DEBT_CONSOL    120   // -1 point de déficit par an pour 20 points de dette en plus
#define INFLATION      200   // 2,00 % (croissance nominale = réelle + inflation)
#define TENSION_BASE   20
#define TECHNO_TREND   30    // PM Technocrate : +0,30 point de tendance
#define RISK_DEBT      1200  // V4 : prime de risque au-delà de 120 % de dette ...
#define RISK_DEFICIT   10    // ... pour le déficit du plan au-delà de 1 point de PIB
#define RISK_PER_TENTH 5     // -0,05 point de croissance par dixième de déficit en trop

// Tendance de croissance : base + piliers Économie et Technologie + budget + PM.
static s16 growth_trend(const Country *c)
{
    BudgetPlan plan;
    s32 t;
    sim_budget_current_plan(c, &plan);
    t = TREND_BASE + (c->pillars[PIL_ECONOMIE] - 500) / 5
          + (c->pillars[PIL_TECHNOLOGIE] - 500) / 10
          + sim_plan_growth(&plan);
    if (c->debt > RISK_DEBT && sim_plan_deficit(&plan) > RISK_DEFICIT)
        t -= (s32)RISK_PER_TENTH * (sim_plan_deficit(&plan) - RISK_DEFICIT);
    if (c->pm == PM_TECHNOCRATE) t += TECHNO_TREND;
    return (s16)t;
}

void sim_economy_update(Country *c)
{
    s16 tpy = c->turns_per_year;
    s16 noise = tpy > 4 ? GROWTH_NOISE_M : GROWTH_NOISE;
    s32 v;
    BudgetPlan plan;
    sim_budget_current_plan(c, &plan);

    // Croissance : retour vers la tendance (1/4 de l'écart par trimestre) + aléa.
    v = (s32)c->growth + sim_step((s32)growth_trend(c) - c->growth, &c->acc_growth, tpy)
      + rng_range(&c->rng, (s16)-noise, noise);
    c->growth = sim_clamp(v, GROWTH_MIN, GROWTH_MAX);

    // Chômage (loi d'Okun) : -0,5 point par point de croissance au-dessus du
    // potentiel et par an, plus un léger rappel vers le chômage structurel.
    v = -((s32)c->growth - GROWTH_POT) / 2 + ((s32)UNEMP_STRUCT - c->unemployment) / 10;
    v = (s32)c->unemployment + sim_step(v, &c->acc_unemp, tpy);
    c->unemployment = sim_clamp(v, UNEMP_MIN, UNEMP_MAX);

    // Dette : variation annuelle = déficit - dette × croissance nominale.
    // Calcul en centièmes de point, avec report du reste d'un tour à l'autre.
    {
        s32 deficit = DEFICIT_BASE + ((s32)c->unemployment - 750) / 10   // dixièmes/an
                    + sim_plan_deficit(&plan)
                    - ((s32)c->debt - DEBT_REF) / DEBT_CONSOL;
        s32 annual = deficit * 10 - (s32)c->debt * ((s32)c->growth + INFLATION) / 1000;
        s32 acc = (s32)c->debt_acc + annual;
        s32 div = 10 * (s32)tpy;
        s32 delta = acc / div;
        c->debt_acc = (s16)(acc - delta * div);
        c->debt = sim_clamp((s32)c->debt + delta, DEBT_MIN, DEBT_MAX);
    }

    // Tension sociale : cible selon le chômage, le pilier Social et le budget.
    {
        s32 target = TENSION_BASE + ((s32)c->unemployment - 750) / 25
                   - ((s32)c->pillars[PIL_SOCIAL] - 500) / 25
                   + sim_plan_tension(&plan);
        s32 t = c->gauges[JAUGE_TENSION];
        target = sim_clamp(target, 0, GAUGE_MAX);
        t += sim_step(target - t, &c->acc_tension, tpy);
        c->gauges[JAUGE_TENSION] = (u8)sim_clamp(t, 0, GAUGE_MAX);
    }
}
