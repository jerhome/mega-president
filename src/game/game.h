#ifndef GAME_H
#define GAME_H

// État partagé d'une partie et enchaînement des écrans (V1).
//
// Flux (docs/CONTRAT_V1.md §8) :
//   TITRE (orientation, PM, difficulté) --game_new--> JEU
//   JEU --game_end_turn--> RAPPORT
//   RAPPORT --game_after_report--> FIN (destitué) | DESTITUTION (procédure)
//                                  | ÉVÉNEMENT | suite
//   DESTITUTION (défense) --game_after_impeach--> ÉVÉNEMENT | suite
//   ÉVÉNEMENT --game_event_chosen--> suite
//   suite = FIN (partie terminée) | CAMPAGNE (fin du 1er mandat) | JEU
//   CAMPAGNE (thème) --game_campaign_done--> ÉLECTION
//   ÉLECTION --game_after_election--> MANDAT2 (réélu) | FIN (battu)
//   MANDAT2 (législatives) --game_mandate2_done--> JEU
//   FIN (bilan) --> SCORES (si high score) | TITRE

#include <genesis.h>
#include "sim/sim.h"
#include "sim/events.h"
#include "sim/ids_v1.h"

typedef struct {
    Country    country;
    TurnReport report;        // compte rendu du dernier trimestre
    s16        event;         // événement à présenter (EVT_*), -1 si aucun
    s8         event_choice;  // choix fait pour l'événement du tour, -1 si aucun
    LawMask    laws_tried;    // lois proposées ce trimestre (bit = id), pour le journal
    LawMask    laws_adopted;  // lois adoptées ce trimestre (bit = id)
    ElectionResult election;  // dernière présidentielle
    LegislativeResult legislatives;  // dernières législatives (dissolution ou 2nd mandat)
    u8         reelected;     // 1 si réélu à la fin du 1er mandat
    u16        score;         // score affiché en fin de partie (mandat ou cumul)
} Game;

extern Game g_game;

// Nouvelle partie : orientation ORIENT_*, Premier ministre PM_*, difficulté DIFF_*,
// turns_per_year : 4 (20 tours) ou 12 (60 tours, 1 tour = 1 mois).
void game_new(u8 orientation, u8 pm, u8 difficulty, u8 turns_per_year, u32 seed);
// Temps du jeu (mode 60 tours : 1 tour = 1 mois).
u8   game_is_monthly(void);                 // 1 en mode 60 tours
// Nom du mois (TXT_MOIS_*) du tour donné, et année civile (2027 + 5 x mandat + année).
const char *game_month_name(u8 turn);
u16  game_calendar_year(u8 turn);
// En-tête « Mois 5/60 - Mai 2027 » (60 tours) ou « Mandat 1 - Trimestre 5 / 20 » (20 tours).
void game_turn_label(u8 turn, char *buf);   // buf : au moins 40 octets

// Proposer une loi : vote immédiat. Met à jour laws_tried/laws_adopted.
u8   game_propose_law(u8 law_id, VoteResult *out);
// Fin du trimestre : sim_end_turn, tirage d'événement, puis SCREEN_REPORT.
void game_end_turn(void);
// Après le rapport (voir le flux ci-dessus).
void game_after_report(void);
// Après le choix de la défense (écran de destitution).
void game_after_impeach(void);
// Après le choix d'un événement (applique ses effets).
void game_event_chosen(u8 choice);
// Thème de campagne choisi (THEME_*) : passe à l'élection.
void game_campaign_done(u8 theme);
// Calcule l'élection (appelé à l'entrée de SCREEN_ELECTION).
void game_run_election(void);
// Après l'écran d'élection : 2nd mandat (législatives) ou fin.
void game_after_election(void);
// Après l'écran du 2nd mandat : retour au jeu.
void game_mandate2_done(void);

// Partie sauvegardée (SRAM) : sauvegarde automatique au début de chaque trimestre.
u8   game_has_save(void);      // 1 si une partie en cours est sauvegardée
void game_save_and_quit(void);  // sauvegarde l’état courant et revient à l’écran d’accueil
u8   game_continue(void);      // recharge la partie et va au tableau de bord (0 si échec)

// Fin de partie
u8   game_destitue(void);        // 1 si la partie s'est terminée par une destitution
u8   game_two_mandates(void);    // 1 si le joueur a fait deux mandats
u16  game_score_first(void);     // score du 1er mandat (tableau 1)
u16  game_score_total(void);     // score cumulé sur 10 ans (tableau 2), 0 si un seul mandat
u16  game_end_year(void);        // année civile de fin de partie

#endif
