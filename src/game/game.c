#include "game/game.h"
#include "ui/screen.h"
#include "save/sram_game.h"
#include "ui/ui.h"
#include "content/content_texts.h"
#include "screens/screens.h"

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
#ifdef MP_DEBUG_SKIP_YEARS
    // ROM de test seulement : avance jusqu'au dernier tour de l'année 3.
    while (g_game.country.status == STATUS_PLAYING &&
           g_game.country.turn + 1 < 3 * g_game.country.turns_per_year)
        sim_end_turn(&g_game.country, &g_game.report);
#endif
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

void game_save_and_quit(void)
{
    sram_game_save(&g_game);   // état exact du trimestre en cours (CP dépensés compris)
    screen_change(SCREEN_TITLE);
}

u8 game_save_stale(void)
{
    return sram_game_stale();
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

#ifdef MP_DEBUG_SCREEN
// ROM de démonstration seulement (captures d'écran, docs/CHARTE_V3.md § 9) :
// prépare un état de partie et saute directement à l'écran voulu.
//   1 campagne puis élection gagnée, 2 campagne puis élection perdue,
//   3 budget, 4 référendum, 5 remaniement, 6 destitution,
//   7 fin : destitution, 8 fin : défaite, 9 fin : victoire (deux mandats),
//   10 élection perdue au 1er tour, 11 titre, 12 tableau de bord,
//   13 / 15 événement (texte long / court), 14 meilleurs scores remplis,
//   16 budget en cohabitation, 17 budget amendé par le Sénat, 18 électorats,
//   19 titre avec sauvegarde d'une ancienne version,
//   20 / 21 événements V4 EHPAD / marche pour le climat.
static void dbg_opinion(Country *c, s16 v)
{
    for (u8 e = 0; e < ELEC_COUNT; e++)
        c->opinion[e] = v;
}

void game_debug_jump(void)
{
    Country *c;
    u8 k = MP_DEBUG_SCREEN;

    game_new(k == 9 ? ORIENT_DROITE : (k == 6 ? ORIENT_CENTRE : ORIENT_GAUCHE),
             PM_RASSEMBLEUR, 0, 4, 1234);
    c = &g_game.country;
    c->cp = 12;
    switch (k) {
    case 1: case 2: case 8: case 10:
        c->turn = sim_mandate_turns(c);
        dbg_opinion(c, k == 1 ? 620 : (k == 10 ? 120 : (k == 2 ? 470 : 380)));
        if (k == 8) {
            c->unemployment += 120;
            c->debt += 85;
            c->pillars[PIL_SOCIAL] -= 60;
            c->pillars[PIL_ECOLOGIE] += 45;
            game_run_election();
            screen_change(SCREEN_END);
        } else {
            screen_change(SCREEN_CAMPAIGN);
        }
        break;
    case 11:   // écran titre normal (choix du PM au script)
        screen_change(SCREEN_TITLE);
        break;
    case 12:   // tableau de bord (Assemblée, carte, intervention télévisée au script)
        c->turn = 2;
        screen_change(SCREEN_GAME);
        break;
    case 13: case 15:   // événement au texte long (13) ou court (15)
        g_game.event = (k == 13) ? 1 : 2;
        screen_change(SCREEN_EVENT);
        break;
    case 20: case 21:   // événements V4 (vignette 4.1 R1) : EHPAD (20), marche pour le climat (21)
        g_game.event = (k == 20) ? EVT_EHPAD : EVT_MARCHE_CLIMAT;
        screen_change(SCREEN_EVENT);
        break;
    case 14:   // meilleurs scores, 10 noms (remplis par scr_scores.c)
        screen_change(SCREEN_SCORES);
        break;
    case 3:
        c->turn = 3;   // le budget se vote au 4e trimestre
        screen_change(SCREEN_BUDGET);
        break;
    case 16:   // budget en cohabitation (plan du PM de gauche, défense réglable)
        c->turn = 6;
        cohab_start(c);
        c->cohab_group = GRP_G1;
        screen_change(SCREEN_BUDGET);
        break;
    case 17:   // budget adopté puis amendé par le Sénat (lecture définitive à 1 CP)
        c->turn = 3;
        c->budget_lvl[BUD_SOCIAL] = 1;
        c->budget_lvl[BUD_EDUCATION] = 1;
        c->budget_lvl[BUD_IMPOT_ENTREPRISES] = 1;
        c->budget_done = 1;
        c->bud_amend_line = BUD_SOCIAL + 1;
        c->bud_amend_from = 2;
        screen_change(SCREEN_BUDGET);
        break;
    case 18:   // détail des électorats
        c->turn = 5;
        g_opinion_prev_ok = 0;
        c->opinion[0] = 380; c->opinion[1] = 470; c->opinion[2] = 610;
        c->opinion[3] = 540; c->opinion[4] = 320;
        screen_change(SCREEN_ELEC);
        break;
    case 19:   // écran titre avec une sauvegarde d'une ancienne version (version 1 du format)
        sram_game_save(&g_game);
        SRAM_enable();
        SRAM_writeByte(SRAM_GAME_OFFSET + 4, 1);
        SRAM_disable();
        screen_change(SCREEN_TITLE);
        break;
    case 22:   // carte de la popularité par région
        c->turn = 6;
        screen_change(SCREEN_MAP);
        break;
    case 23:   // aide en pages (script : droite pour changer de page)
        screen_change(SCREEN_HELP);
        break;
    case 24:   // remaniement avec 1 CP (profils grisés, message d'erreur)
        c->turn = 6;
        c->cp = 1;
        screen_change(SCREEN_RESHUFFLE);
        break;
    case 4:
        screen_change(SCREEN_REFERENDUM);
        break;
    case 5:
        c->turn = 6;
        screen_change(SCREEN_RESHUFFLE);
        break;
    case 6: case 7:
        c->turn = 9;
        c->gauges[JAUGE_SCANDALE] = 88;
        c->gauges[JAUGE_DEFIANCE] = 81;
        dbg_opinion(c, 230);
        for (u8 g = 0; g < GRP_COUNT; g++)
            if (g != c->player_group)
                c->relation[g] = 20;
        c->impeach = IMPEACH_DEFENSE;
        if (k == 7) {
            c->status = STATUS_DESTITUE;
            screen_change(SCREEN_END);
        } else {
            screen_change(SCREEN_IMPEACH);
        }
        break;
    default:   // 9 : réélu, puis fin du 2nd mandat
        c->turn = sim_mandate_turns(c);
        dbg_opinion(c, 650);
        game_run_election();
        sim_start_second_mandate(c, &g_game.legislatives);
        c->turn = sim_mandate_turns(c);
        c->unemployment -= 150;
        c->pillars[PIL_TECHNOLOGIE] += 120;
        screen_change(SCREEN_END);
        break;
    }
}
#endif
