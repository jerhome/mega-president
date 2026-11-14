#ifndef SIM_INSTITUTIONS_H
#define SIM_INSTITUTIONS_H

// Institutions V1 : législatives et dissolution (1.08), majorité relative
// (1.09), motion de censure (1.07), destitution (1.10).

#include "sim/sim_state.h"
#include "sim/parliament.h"

typedef struct {
    u16 seats_before[GRP_COUNT];
    u16 seats_after[GRP_COUNT];
    u16 bloc_before;     // sièges du bloc présidentiel avant
    u16 bloc_after;
    u8  majority;        // 1 : majorité absolue du bloc présidentiel
    u8  won;             // 1 : le bloc gagne des sièges (ou garde la majorité)
    u8  cohab_started;   // V2 : 1 si la cohabitation commence (législatives perdues)
    u8  cohab_ended;     // V2 : 1 si une cohabitation en cours prend fin (législatives gagnées)
} LegislativeResult;

// Bloc présidentiel : parti du joueur et groupes voisins (distance 1).
u8  sim_in_bloc(const Country *c, u8 group);
u16 sim_bloc_seats(const Country *c);
// 1 si le bloc présidentiel a la majorité absolue (sinon : majorité relative).
u8  sim_has_majority(const Country *c);

// Législatives : redistribution des sièges selon la popularité et l'opinion
// de l'électorat du joueur (consomme du hasard). out peut être NULL.
void sim_legislatives(Country *c, LegislativeResult *out);

// Dissolution : au plus une fois par an (et pas l'année qui suit des législatives).
u8 sim_can_dissolve(const Country *c);
// Dissout et organise les législatives. Renvoie 0 si impossible.
u8 sim_dissolve(Country *c, LegislativeResult *out);

// --- Motion de censure ---
#define CENSURE_DEFIANCE   60   // défiance minimale pour une motion
// Sièges de l'opposition (groupes hors parti du joueur avec relation < 50).
u16 sim_opposition_seats(const Country *c);
// 1 si une motion serait déposée (aperçu, sans effet).
u8  sim_censure_threat(const Country *c);
// Voix pour la motion (aperçu, sans effet).
u16 sim_censure_votes(const Country *c);

// --- Destitution (REGLES §10) ---
#define IMPEACH_SCANDALE   70
#define IMPEACH_DEFIANCE   60
#define IMPEACH_MAJORITY   385   // deux tiers de 577
#define DEFENSE_TV_CP      2
#define DEFENSE_NEGOCIER_CP 3
// 1 si une procédure est en cours (le joueur peut se défendre).
u8  sim_impeach_pending(const Country *c);
// Voix pour la destitution si le vote avait lieu maintenant (aperçu).
u16 sim_impeach_votes(const Country *c);
// 1 si la défense est disponible (procédure en cours, pas encore utilisée, CP).
u8  sim_defense_available(const Country *c, u8 defense);
// Défense : DEFENSE_TV (discours, 2 CP), DEFENSE_SACRIFICE (un ministre
// démissionne), DEFENSE_NEGOCIER (3 CP, relation + avec le groupe donné).
// Renvoie 1 si appliquée.
u8  sim_defend(Country *c, u8 defense, u8 group);

// Interne (fin de tour).
void inst_end_turn_quarter(Country *c, u8 *censure, u16 *censure_votes, u8 *new_pm,
                           u8 *impeach, u16 *impeach_votes, u16 *impeach_senate_votes);

#endif
