#ifndef SIM_PARLIAMENT_H
#define SIM_PARLIAMENT_H

// Assemblée nationale (tâche 3.1).

#include "sim/sim_state.h"

extern const s8  group_orientation[GRP_COUNT];   // -80 -40 0 +40 +80
extern const u16 group_seats_start[GRP_COUNT];   // 75 140 160 120 82

#define VOTE_THRESHOLD 75   // relation + affinité >= seuil => pour

typedef struct {
    u8  pour[GRP_COUNT];      // 1 si le groupe vote pour
    s16 score[GRP_COUNT];     // relation + affinité (diagnostic)
    u16 votes_pour;
    u16 votes_contre;
    u8  adopted;
} VoteResult;

// Affinité d'orientation loi/groupe : 50 - |écart| / 2 (de +50 à -50).
s16 parl_affinity(s8 law_orientation, s8 group_orientation);

// Vote déterministe (sans modifier l'état) : utile pour l'aperçu à l'écran.
void parl_vote(const Country *c, s8 law_orientation, VoteResult *out);

// Évolution des relations après un vote.
void parl_after_vote(Country *c, s8 law_orientation, const VoteResult *v);

// Phase « Parlement » de fin de tour : dérive des relations.
void parl_end_turn(Country *c);

// Sièges des groupes dont la relation >= 50 (soutien).
u16 parl_support_seats(const Country *c);

#endif
