#ifndef SIM_STATE_H
#define SIM_STATE_H

// État du pays (tâche 2.1). Unités : voir docs/CONTRAT_V0.md §2 et docs/SIM.md.
// Tout est entier ; aucune allocation dynamique.

#include "sim/sim_types.h"
#include "sim/rng.h"
#include "sim/ids_v1.h"

// Masque de lois (64 bits en deux u32 : évite les u64 sur 68000).
typedef struct { u32 w[2]; } LawMask;
#define LAW_MASK_BITS 64
static inline u8 law_mask_get(const LawMask *m, u8 id)
{
    return (u8)((id < LAW_MASK_BITS) ? ((m->w[id >> 5] >> (id & 31)) & 1u) : 0);
}
static inline void law_mask_set(LawMask *m, u8 id)
{
    if (id < LAW_MASK_BITS) m->w[id >> 5] |= (u32)1 << (id & 31);
}
static inline void law_mask_clear(LawMask *m) { m->w[0] = 0; m->w[1] = 0; }
static inline u8 law_mask_any(const LawMask *m) { return (u8)((m->w[0] | m->w[1]) != 0); }

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

// V2 : Sénat (348 sièges, mêmes groupes G1-G5 que l'Assemblée)
#define SENATE_SEATS     348

#define SIM_MAX_PENDING  8   // lois adoptées en attente d'effet

// Absence de choix (PM, thème de campagne)
#define PM_NONE      PM_COUNT
// V2 : Premier ministre d'opposition pendant une cohabitation (jamais un choix du joueur).
#define PM_OPPOSITION (PM_COUNT + 1)
#define THEME_NONE   THEME_COUNT

// État de la partie
enum { STATUS_PLAYING, STATUS_DESTITUE, STATUS_COUNT };

// Procédure de destitution (REGLES §10)
enum { IMPEACH_NONE, IMPEACH_DEFENSE };
// Défenses possibles pendant la procédure (bits de Country.impeach_def)
enum { DEFENSE_TV, DEFENSE_SACRIFICE, DEFENSE_NEGOCIER, DEFENSE_COUNT };

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
    // Restes de calcul (mode 60 tours : pas de perte par troncature)
    s16 acc_growth;
    s16 acc_unemp;
    s16 acc_tension;
    s16 acc_opinion[ELEC_COUNT];

    u8 gauges[JAUGE_COUNT];    // 0-100
    u8 cp;                     // capital politique (0-15)
    u8 turn;                   // tours joués (0 = début du mandat)
    u8 turns_per_year;         // 4 (V0) ou 12 (V1)
    u8 orientation;            // ORIENT_*
    u8 player_group;           // GRP_* du parti du joueur

    u8 relation[GRP_COUNT];    // 0-100
    u8 relation_base[GRP_COUNT]; // relation « naturelle » vers laquelle on dérive
    u16 seats[GRP_COUNT];

    LawMask laws_adopted;      // bit i = loi i adoptée (64 lois max)
    u32 events_done;           // bit i = événement i déjà tiré ce mandat (ids 0-31, sim/events.h)
    u8  last_event;            // dernier événement tiré + 1 (0 = aucun)
    u8 n_pending;
    PendingLaw pending[SIM_MAX_PENDING];

    // --- V1 ---
    u8  pm;              // PM_* (PM_NONE : aucun profil)
    u8  difficulty;      // DIFF_*
    u8  mandate;         // 0 = 1er mandat, 1 = 2nd mandat
    u8  theme;           // THEME_* de campagne (THEME_NONE si non choisi)
    u8  budget;          // BUDGET_* en vigueur (reconduction : BUDGET_EQUILIBRE)
    u8  budget_done;     // 1 si le budget de l'année est adopté (vote ou 49.3)
    u8  art49_year;      // année (1..) du dernier 49.3 hors budget, 0 = jamais
    u8  tv_turn;         // tour + 1 de la dernière intervention télévisée
    u8  tv_fatigue;      // usure des interventions télévisées
    u8  dissolve_turn;   // tour + 1 des dernières législatives, 0 = aucune ce mandat
    u8  impeach;         // IMPEACH_*
    u8  impeach_def;     // bits DEFENSE_* déjà utilisés pendant la procédure
    u8  impeach_cool;    // trimestres avant qu'une nouvelle procédure soit possible
    u8  status;          // STATUS_*
    u8  censures;        // gouvernements renversés (statistique)
    u16 score_first;     // score du 1er mandat (fixé au début du 2nd)

    // --- V2 ---
    u32 events_done_hi;  // bit i = événement 32 + i déjà tiré ce mandat (capacité 64)
    u16 senate[GRP_COUNT]; // sièges du Sénat par groupe (somme = 348)
    u8  cohab;           // 1 pendant une cohabitation
    u8  cohab_group;     // GRP_* du groupe d'opposition le plus fort (donne le camp du PM)
    u8  pm_prev;         // PM du joueur avant la cohabitation (rétabli à sa fin)
    u8  referendum_done; // 1 si le référendum de ce mandat est utilisé
    u8  reshuffle_year;  // année (1..) du dernier remaniement, 0 = jamais
    u8  grace_pm;        // état de grâce du remaniement (dixièmes de %, décroît)
    u8  gov_laws;        // lois adoptées par le gouvernement de cohabitation (statistique)
    u8  cohabitations;   // cohabitations commencées (statistique)

    Rng rng;
} Country;

// Bornage générique, calcul en s32 pour éviter les débordements.
static inline s16 sim_clamp(s32 v, s16 lo, s16 hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return (s16)v;
}

// Ajoute d points (non convertis) à une jauge, bornée à 0-100.
static inline void sim_gauge_add(u8 *gauge, s16 d)
{
    *gauge = (u8)sim_clamp((s32)*gauge + d, 0, GAUGE_MAX);
}

static inline s16 sim_abs16(s16 v) { return v < 0 ? (s16)-v : v; }

#endif
