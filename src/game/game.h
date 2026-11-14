#ifndef GAME_H
#define GAME_H

// État partagé d'une partie et enchaînement des écrans (V0).
//
// Flux d'un tour (docs/CONTRAT_V0.md §7) :
//   SCREEN_GAME (tableau de bord : proposer des lois, finir le trimestre)
//     -> game_end_turn() : sim_end_turn() puis tirage d'un événement
//     -> SCREEN_REPORT (journal du trimestre)
//     -> SCREEN_EVENT si g_game.event >= 0
//     -> SCREEN_GAME, ou SCREEN_ELECTION si le mandat est terminé
//   SCREEN_ELECTION -> SCREEN_END -> SCREEN_TITLE

#include <genesis.h>
#include "sim/sim.h"
#include "sim/events.h"

#define GAME_JOURNAL_MAX 6   // lignes de journal mémorisées pour le rapport

typedef struct {
    Country    country;
    TurnReport report;        // compte rendu du dernier trimestre
    s16        event;         // événement à présenter (EVT_*), -1 si aucun
    s8         event_choice;  // choix fait pour l'événement du tour, -1 si aucun
    u8         laws_tried;    // lois proposées ce trimestre (bit = id), pour le journal
    u8         laws_adopted;  // lois adoptées ce trimestre (bit = id)
    ElectionResult election;  // rempli par game_run_election()
    u16        score;         // score final du mandat
} Game;

extern Game g_game;

// Nouvelle partie (orientation ORIENT_*, graine de hasard).
void game_new(u8 orientation, u32 seed);
// Proposer une loi : vote immédiat. Met à jour laws_tried/laws_adopted.
u8   game_propose_law(u8 law_id, VoteResult *out);
// Fin du trimestre : sim_end_turn, tirage d'événement, puis SCREEN_REPORT.
void game_end_turn(void);
// Après le rapport : SCREEN_EVENT, SCREEN_GAME ou SCREEN_ELECTION.
void game_after_report(void);
// Après le choix d'un événement (applique ses effets) : SCREEN_GAME ou SCREEN_ELECTION.
void game_event_chosen(u8 choice);
// Calcule l'élection et le score (appelé à l'entrée de SCREEN_ELECTION).
void game_run_election(void);

#endif
