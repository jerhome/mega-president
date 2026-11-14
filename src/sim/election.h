#ifndef SIM_ELECTION_H
#define SIM_ELECTION_H

// Présidentielle à deux tours (tâche 3.2) et score de fin de mandat.

#include "sim/sim_state.h"

// Pool de candidats fictifs (contrat §5) ; CAND_JOUEUR = le joueur.
enum {
    CAND_GAUCHE_RADICALE,   // -80
    CAND_ECOLO,             // -50
    CAND_CENTRE,            //   0
    CAND_DROITE,            // +40
    CAND_NATIONALISTE,      // +80
    CAND_POOL_COUNT,
    CAND_JOUEUR = CAND_POOL_COUNT
};

#define ELECTION_CANDIDATES 5   // le joueur + 4 adversaires

extern const s8 cand_orientation[CAND_POOL_COUNT];

typedef struct {
    u8  cand[ELECTION_CANDIDATES];     // cand[0] = CAND_JOUEUR, puis 4 adversaires
    s16 round1[ELECTION_CANDIDATES];   // dixièmes de %, somme = 1000
    u8  finalist[2];                   // identifiants CAND_* (finalist[0] = 1er)
    s16 round2[2];                     // dixièmes de %, somme = 1000
    u8  elected;                       // 1 si le joueur est élu
} ElectionResult;

// Orientation du joueur sur l'axe -100..+100 (gauche -40, centre 0, droite +40).
s8 sim_player_orientation(const Country *c);

// Les 4 adversaires du joueur (écrit 4 identifiants dans out).
void election_opponents(u8 orientation, u8 *out);

// Organise la présidentielle (consomme du hasard dans c->rng).
void sim_election(Country *c, ElectionResult *out);

// Score de fin de mandat (REGLES §12, sans second mandat). Jamais négatif.
u16 sim_mandate_score(const Country *c, u8 reelected);

#endif
