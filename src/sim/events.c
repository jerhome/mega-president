#include "sim/events.h"
#include "sim/sim.h"

const EventDef *event_get(u8 id)
{
    return id < EVT_COUNT ? &event_defs[id] : 0;
}

s16 events_var_value(const Country *c, u8 var)
{
    switch (var) {
    case EVV_TENSION:     return c->gauges[JAUGE_TENSION];
    case EVV_SCANDALE:    return c->gauges[JAUGE_SCANDALE];
    case EVV_DEFIANCE:    return c->gauges[JAUGE_DEFIANCE];
    case EVV_CP:          return c->cp;
    case EVV_POPULARITE:  return sim_popularity(c);
    case EVV_CHOMAGE:     return c->unemployment;
    case EVV_DETTE:       return c->debt;
    case EVV_CROISSANCE:  return c->growth;
    case EVV_SOCIAL:      return c->pillars[PIL_SOCIAL];
    case EVV_ECONOMIE:    return c->pillars[PIL_ECONOMIE];
    case EVV_TECHNOLOGIE: return c->pillars[PIL_TECHNOLOGIE];
    case EVV_ECOLOGIE:    return c->pillars[PIL_ECOLOGIE];
    case EVV_SECURITE:    return c->pillars[PIL_SECURITE];
    default:              return 0;
    }
}

u8 events_cond_ok(const Country *c, const EventDef *e)
{
    s16 v;
    if (e->cond.var == EVV_NONE) return 1;
    v = events_var_value(c, e->cond.var);
    switch (e->cond.op) {
    case EVO_GT: return (u8)(v > e->cond.value);
    case EVO_GE: return (u8)(v >= e->cond.value);
    case EVO_LT: return (u8)(v < e->cond.value);
    default:     return (u8)(v <= e->cond.value);
    }
}

u8 events_eligible(const Country *c, u8 id)
{
    const EventDef *e = event_get(id);
    if (!e) return 0;
    if (c->last_event == id + 1) return 0;
    if (e->cond.var == EVV_NONE && (c->events_done & ((u32)1 << id))) return 0;
    return events_cond_ok(c, e);
}

u8 events_chance_pct(const Country *c)
{
    u8 tpy = c->turns_per_year ? c->turns_per_year : SIM_TURNS_PER_YEAR_V0;
    u16 pct = (u16)(100 * SIM_TURNS_PER_YEAR_V0 / tpy);
    return (u8)(pct > 100 ? 100 : pct);
}

s16 events_draw(Country *c)
{
    u16 total = 0, pick;
    u8 i;
    u8 roll;

    if (c->turn == 0) return -1;                 // jamais au tout premier tour
    roll = (u8)rng_below(&c->rng, 100);
    if (roll >= events_chance_pct(c)) return -1;

    for (i = 0; i < EVT_COUNT; i++)
        if (events_eligible(c, i)) total = (u16)(total + event_defs[i].weight);
    if (total == 0) return -1;

    pick = rng_below(&c->rng, total);
    for (i = 0; i < EVT_COUNT; i++) {
        if (!events_eligible(c, i)) continue;
        if (pick < event_defs[i].weight) {
            c->events_done |= (u32)1 << i;
            c->last_event = (u8)(i + 1);
            return i;
        }
        pick = (u16)(pick - event_defs[i].weight);
    }
    return -1;   // inatteignable
}

u8 events_choose(Country *c, u8 event_id, u8 choice)
{
    const EventDef *e = event_get(event_id);
    if (!e || choice >= e->n_choices) return 0;
    sim_apply_effects(c, e->choices[choice].effects, e->choices[choice].n_effects);
    return 1;
}
