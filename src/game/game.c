#include "game/game.h"
#include "ui/screen.h"
#include "save/sram_game.h"
#include "ui/ui.h"
#include "content/content_texts.h"

#define GAME_START_YEAR 2027

Game g_game;

void game_new(u8 orientation, u8 pm, u8 difficulty, u8 turns_per_year, u32 seed)
{
    SimSetup s;

    memset(&g_game, 0, sizeof(g_game));
    s.orientation = orientation;
    s.pm = pm;
    s.difficulty = difficulty;
    s.turns_per_year = (turns_per_year == 12) ? 12 : 4;   // 20 ou 60 tours
    s.seed = seed;
    sim_init_ex(&g_game.country, &s);
    g_game.event = -1;
    g_game.event_choice = -1;
}

u8 game_propose_law(u8 law_id, VoteResult *out)
{
    u8 r = sim_propose_law(&g_game.country, law_id, out);
    if (r == LAW_OK) {
        law_mask_set(&g_game.laws_tried, law_id);
        if (out->adopted)
            law_mask_set(&g_game.laws_adopted, law_id);
    }
    return r;
}

// Suite normale d'un tour (après rapport, défense et événement).
static void next_after_turn(void)
{
    Country *c = &g_game.country;

    if (sim_game_over(c)) {
        sram_game_erase();
        screen_change(SCREEN_END);
    } else if (sim_election_due(c)) {
        screen_change(SCREEN_CAMPAIGN);
    } else {
        sram_game_save(&g_game);   // reprise possible au début de ce trimestre
        screen_change(SCREEN_GAME);
    }
}

static void event_or_next(void)
{
    if (g_game.event >= 0)
        screen_change(SCREEN_EVENT);
    else
        next_after_turn();
}

void game_end_turn(void)
{
    Country *c = &g_game.country;

    sim_end_turn(c, &g_game.report);
    if (sim_game_over(c) || sim_mandate_over(c))
        g_game.event = -1;
    else
        g_game.event = events_draw(c);
    g_game.event_choice = -1;
    screen_change(SCREEN_REPORT);
}

void game_after_report(void)
{
    // Le journal du trimestre est lu : on repart de zéro pour le suivant.
    law_mask_clear(&g_game.laws_tried);
    law_mask_clear(&g_game.laws_adopted);
    if (game_destitue()) {
        sram_game_erase();
        screen_change(SCREEN_END);
    }
    else if (sim_impeach_pending(&g_game.country))
        screen_change(SCREEN_IMPEACH);
    else
        event_or_next();
}

void game_after_impeach(void)
{
    event_or_next();
}

void game_event_chosen(u8 choice)
{
    events_choose(&g_game.country, (u8)g_game.event, choice);
    g_game.event_choice = (s8)choice;
    g_game.event = -1;
    next_after_turn();
}

void game_campaign_done(u8 theme)
{
    sim_set_theme(&g_game.country, theme);
    screen_change(SCREEN_ELECTION);
}

void game_run_election(void)
{
    sim_election(&g_game.country, &g_game.election);
    g_game.reelected = g_game.election.elected;
    g_game.score = g_game.reelected ? sim_score_first(&g_game.country)
                                    : sim_mandate_score(&g_game.country, 0);
}

void game_after_election(void)
{
    if (g_game.reelected) {
        sim_start_second_mandate(&g_game.country, &g_game.legislatives);
        screen_change(SCREEN_MANDATE2);
    } else {
        sram_game_erase();
        screen_change(SCREEN_END);
    }
}

void game_mandate2_done(void)
{
    g_game.event = -1;
    sram_game_save(&g_game);
    screen_change(SCREEN_GAME);
}

u8 game_continue(void)
{
    if (!sram_game_load(&g_game))
        return 0;
    screen_change(SCREEN_GAME);
    return 1;
}

u8 game_has_save(void)
{
    return sram_game_exists();
}

u8 game_destitue(void)
{
    return g_game.country.status == STATUS_DESTITUE;
}

u8 game_two_mandates(void)
{
    return g_game.reelected;
}

u16 game_score_first(void)
{
    return g_game.reelected ? sim_score_first(&g_game.country)
                            : sim_mandate_score(&g_game.country, 0);
}

u16 game_score_total(void)
{
    return g_game.reelected ? sim_score_total(&g_game.country) : 0;
}

u16 game_end_year(void)
{
    // sim_year compte les années du mandat en cours : on ajoute 5 ans par mandat déjà fait.
    return (u16)(GAME_START_YEAR + 5 * g_game.country.mandate + sim_year(&g_game.country));
}

u8 game_is_monthly(void)
{
    return g_game.country.turns_per_year == 12;
}

const char *game_month_name(u8 turn)
{
    return txt[TXT_MOIS_1 + (turn % 12)];
}

u16 game_calendar_year(u8 turn)
{
    const Country *c = &g_game.country;
    return (u16)(GAME_START_YEAR + 5 * c->mandate + turn / c->turns_per_year);
}

static void cat_n(char *dst, s32 v)
{
    char b[14];
    ui_format_number(b, v, 0);
    strcat(dst, b);
}

void game_turn_label(u8 turn, char *buf)
{
    const Country *c = &g_game.country;
    u8 total = sim_mandate_turns(c);
    u8 t = turn < total ? turn : (u8)(total - 1);

    buf[0] = 0;
    if (game_is_monthly()) {
        strcat(buf, txt[TXT_TOUR_MOIS]);
        strcat(buf, " ");
        cat_n(buf, t + 1);
        strcat(buf, "/60 - ");
        strcat(buf, game_month_name(t));
        strcat(buf, " ");
        cat_n(buf, game_calendar_year(t));
    } else {
        strcat(buf, "Mandat ");
        cat_n(buf, c->mandate + 1);
        strcat(buf, " - Trimestre ");
        cat_n(buf, t + 1);
        strcat(buf, " / 20");
    }
}
