#include "game/game.h"
#include "ui/screen.h"

Game g_game;

void game_new(u8 orientation, u32 seed)
{
    memset(&g_game, 0, sizeof(g_game));
    sim_init(&g_game.country, orientation, seed, 4);   // V0 : mode 20 tours
    g_game.event = -1;
    g_game.event_choice = -1;
}

u8 game_propose_law(u8 law_id, VoteResult *out)
{
    u8 r = sim_propose_law(&g_game.country, law_id, out);
    if (r == LAW_OK) {
        g_game.laws_tried |= (u8)(1 << law_id);
        if (out->adopted)
            g_game.laws_adopted |= (u8)(1 << law_id);
    }
    return r;
}

static void next_after_turn(void)
{
    screen_change(sim_mandate_over(&g_game.country) ? SCREEN_ELECTION : SCREEN_GAME);
}

void game_end_turn(void)
{
    sim_end_turn(&g_game.country, &g_game.report);
    g_game.event = sim_mandate_over(&g_game.country) ? -1 : events_draw(&g_game.country);
    g_game.event_choice = -1;
    screen_change(SCREEN_REPORT);
}

void game_after_report(void)
{
    // Le journal du trimestre est lu : on repart de zéro pour le suivant.
    g_game.laws_tried = 0;
    g_game.laws_adopted = 0;
    if (g_game.event >= 0)
        screen_change(SCREEN_EVENT);
    else
        next_after_turn();
}

void game_event_chosen(u8 choice)
{
    events_choose(&g_game.country, (u8)g_game.event, choice);
    g_game.event_choice = (s8)choice;
    g_game.event = -1;
    next_after_turn();
}

void game_run_election(void)
{
    sim_election(&g_game.country, &g_game.election);
    g_game.score = sim_mandate_score(&g_game.country, g_game.election.elected);
}
