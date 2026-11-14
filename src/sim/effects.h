#ifndef SIM_EFFECTS_H
#define SIM_EFFECTS_H

// Clés d'effets communes aux lois et aux événements (contrat §3).

#include "sim/sim_state.h"

typedef enum {
    EFF_NONE = 0,
    EFF_POP_POPULAIRES,  // points de % d'opinion
    EFF_POP_MOYENNES,
    EFF_POP_AISEES,
    EFF_POP_TOUS,
    EFF_CROISSANCE,      // dixièmes de point
    EFF_CHOMAGE,         // dixièmes de point
    EFF_DETTE,           // points de PIB
    EFF_SOCIAL,          // points de pilier (0-100)
    EFF_ECONOMIE,
    EFF_TECHNOLOGIE,
    EFF_ECOLOGIE,
    EFF_SECURITE,
    EFF_TENSION,         // points de jauge
    EFF_SCANDALE,
    EFF_DEFIANCE,
    EFF_CP,              // capital politique
    EFF_REL_G1,          // points de relation
    EFF_REL_G2,
    EFF_REL_G3,
    EFF_REL_G4,
    EFF_REL_G5,
    EFF_COUNT
} EffectKey;

// Applique un effet (unités lisibles) : conversion en unités internes et bornage.
void sim_apply_effect(Country *c, u8 key, s8 value);

// Applique une liste d'effets (lois, événements : point d'accroche réutilisable).
void sim_apply_effects(Country *c, const Effect *effects, u8 count);

// Facteur de conversion unités lisibles -> internes pour une clé (0 si inconnue).
s16 sim_effect_scale(u8 key);

#endif
