#ifndef SIM_STATE_H
#define SIM_STATE_H

// État du pays (tâche 2.1). Unités : voir docs/CONTRAT_V0.md §2 et docs/SIM.md.
// Tout est entier ; aucune allocation dynamique.

#include "sim/sim_types.h"
#include "sim/rng.h"

// Électorats
enum { ELEC_POPULAIRES, ELEC_MOYENNES, ELEC_AISEES, ELEC_COUNT };
// Piliers (dixièmes, 0-1000)
enum { PIL_SOCIAL, PIL_ECONOMIE, PIL_TECHNOLOGIE, PIL_ECOLOGIE, PIL_SECURITE, PIL_COUNT };
// Jauges (0-100)
enum { JAUGE_TENSION, JAUGE_SCANDALE, JAUGE_DEFIANCE, JAUGE_COUNT };
// Groupes parlementaires (contrat §4) : G1 = GRP_G1 ...
enum { GRP_G1, GRP_G2, GRP_G3, GRP_G4, GRP_G5, GRP_COUNT };
// Orientation du joueur
enum { ORIENT_GAUCHE, ORIENT_CENTRE, ORIENT_DROITE, ORIENT_COUNT };

// Bornes des grandeurs (unités internes)
#define OPINION_MIN      0
#define OPINION_MAX      1000
#define GROWTH_MIN       (-800)   // -8,00 %
#define GROWTH_MAX       800      // +8,00 %
#define UNEMP_MIN        200      // 2,00 %
#define UNEMP_MAX        2500     // 25,00 %
#define DEBT_MIN         0
#define DEBT_MAX         3000     // 300,0 % du PIB
#define PILLAR_MIN       0
#define PILLAR_MAX       1000
#define GAUGE_MAX        100
#define RELATION_MAX     100
#define CP_MAX           15

#define SEATS_TOTAL      577
#define SEATS_MAJORITY   289

#define SIM_MAX_PENDING  8   // lois adoptées en attente d'effet

// Effet (clé, valeur) en unités lisibles du contrat §3.
typedef struct {
    u8 key;      // EffectKey (sim/effects.h)
    s8 value;
} Effect;

typedef struct {
    u8 law;          // identifiant LOI_*
    u8 turns_left;   // tours restants avant application
} PendingLaw;

typedef struct {
    // Opinion et économie
    s16 opinion[ELEC_COUNT];   // dixièmes de %
    s16 growth;                // centièmes de point (annualisée)
    s16 unemployment;          // centièmes de point
    s16 debt;                  // dixièmes de point de PIB
    s16 pillars[PIL_COUNT];    // dixièmes (0-1000)

    // Valeurs de départ (progression, score, présidentielle)
    s16 pillars_start[PIL_COUNT];
    s16 unemployment_start;
    s16 debt_start;

    s16 debt_acc;              // reste de calcul de la dette (centièmes de point)
    s16 grace;                 // bonus d'état de grâce restant (dixièmes de %)

    u8 gauges[JAUGE_COUNT];    // 0-100
    u8 cp;                     // capital politique (0-15)
    u8 turn;                   // tours joués (0 = début du mandat)
    u8 turns_per_year;         // 4 (V0) ou 12 (V1)
    u8 orientation;            // ORIENT_*
    u8 player_group;           // GRP_* du parti du joueur

    u8 relation[GRP_COUNT];    // 0-100
    u8 relation_base[GRP_COUNT]; // relation « naturelle » vers laquelle on dérive
    u16 seats[GRP_COUNT];

    u16 laws_adopted;          // bit i = loi i adoptée
    u32 events_done;           // bit i = événement i déjà tiré ce mandat (sim/events.h)
    u8  last_event;            // dernier événement tiré + 1 (0 = aucun)
    u8 n_pending;
    PendingLaw pending[SIM_MAX_PENDING];

    Rng rng;
} Country;

// Bornage générique, calcul en s32 pour éviter les débordements.
static inline s16 sim_clamp(s32 v, s16 lo, s16 hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return (s16)v;
}

static inline s16 sim_abs16(s16 v) { return v < 0 ? (s16)-v : v; }

#endif
