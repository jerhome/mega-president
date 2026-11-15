// Mega Président — validation et empreinte d'une partie sauvegardée (portable).
#include <stddef.h>
#include "game/game_state.h"
#include "content/content_texts.h"

u16 game_fingerprint(void)
{
    u32 h = 0x1021;
    u32 v[] = {
        EVT_COUNT, LOI_COUNT, TXT_COUNT, ELEC_COUNT, PIL_TOTAL, REG_COUNT, BUD_LINES,
        sizeof(Game), sizeof(Country), sizeof(TurnReport),
        offsetof(Country, gauges), offsetof(Country, laws_adopted),
        offsetof(Country, pm), offsetof(Country, events_done_hi),
        offsetof(Country, budget_lvl), offsetof(Country, bud_amend_line),
        offsetof(Country, rng),
        offsetof(Game, report), offsetof(Game, event), offsetof(Game, election),
        offsetof(Game, legislatives), offsetof(Game, score)
    };
    u16 i;
    for (i = 0; i < (u16)(sizeof v / sizeof v[0]); i++)
        h = (h * 31u) ^ (v[i] + (h >> 13));
    return (u16)(h ^ (h >> 16));
}

static u8 s8_in(s8 v, s8 lo, s8 hi) { return (u8)(v >= lo && v <= hi); }

u8 game_sanitize(const Game *g)
{
    const Country *c = &g->country;
    u8 i;

    if (c->turns_per_year != 4 && c->turns_per_year != 12) return 0;
    if (c->orientation >= ORIENT_COUNT) return 0;
    if (c->player_group >= GRP_COUNT) return 0;
    if (c->difficulty >= DIFF_COUNT) return 0;
    if (c->pm >= PM_COUNT && c->pm != PM_NONE && c->pm != PM_OPPOSITION) return 0;
    if (c->pm_prev >= PM_COUNT && c->pm_prev != PM_NONE && c->pm_prev != PM_OPPOSITION) return 0;
    if (c->mandate > 1 || c->cohab > 1 || g->reelected > 1) return 0;
    if (c->turn > (u8)(c->turns_per_year * 5)) return 0;
    if (c->theme > THEME_NONE) return 0;
    if (c->status >= STATUS_COUNT || c->impeach > IMPEACH_DEFENSE) return 0;
    if (c->cp > CP_MAX) return 0;
    if (c->n_pending > SIM_MAX_PENDING) return 0;
    for (i = 0; i < c->n_pending; i++)
        if (c->pending[i].law >= LOI_COUNT) return 0;
    if (c->cohab_group >= GRP_COUNT) return 0;
    if (c->last_event > EVT_COUNT) return 0;
    if (c->budget > BUDGET_PERSO) return 0;
    if (c->bud_amend_line > BUD_LINES) return 0;
    if (!s8_in(c->bud_amend_from, BUD_LVL_MIN, BUD_LVL_MAX)) return 0;
    if (!s8_in(c->cohab_def_adj, -1, 1)) return 0;
    for (i = 0; i < BUD_LINES; i++)
        if (!s8_in(c->budget_lvl[i], BUD_LVL_MIN, BUD_LVL_MAX)) return 0;
    for (i = 0; i < JAUGE_COUNT; i++)
        if (c->gauges[i] > GAUGE_MAX) return 0;
    if (g->event < -1 || g->event >= (s16)EVT_COUNT) return 0;
    if (g->event_choice < -1 || g->event_choice >= EVENT_MAX_CHOICES) return 0;
    if (g->report.senate_amend > BUD_LINES) return 0;
    return 1;
}
