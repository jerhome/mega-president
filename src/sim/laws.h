#ifndef SIM_LAWS_H
#define SIM_LAWS_H

// Lois décrites comme des données (tâche 2.3).
// Les textes (noms, descriptions) sont ailleurs : ici, seulement les identifiants.

#include "sim/sim_state.h"

enum {
    LOI_SMIC,     // hausse du SMIC
    LOI_IS,       // baisse de l'impôt sur les sociétés
    LOI_RENOV,    // plan de rénovation thermique
    LOI_POLICE,   // recrutement de policiers
    LOI_CIR,      // crédit d'impôt recherche
    LOI_COUNT
};

#define LAW_MAX_EFFECTS    6
#define LAW_MAX_IMMEDIATE  3

typedef struct {
    u8 id;
    u8 cost;          // capital politique
    u8 delay_q;       // délai des effets, en trimestres (= tours en V0)
    s8 orientation;   // -100 (gauche) ... +100 (droite)
    u8 n_effects;
    u8 n_immediate;
    Effect effects[LAW_MAX_EFFECTS];       // différés, appliqués à maturité
    Effect immediate[LAW_MAX_IMMEDIATE];   // à l'adoption (opinion surtout)
} LawDef;

extern const LawDef law_defs[LOI_COUNT];

const LawDef *law_get(u8 id);   // NULL si id invalide

// Délai en tours pour la durée de tour courante (trimestres -> tours).
u8 law_delay_turns(const Country *c, const LawDef *law);

// Résultat de sim_propose_law
enum {
    LAW_OK = 0,         // vote effectué (voir VoteResult.adopted)
    LAW_ERR_ID,         // identifiant inconnu
    LAW_ERR_CP,         // capital politique insuffisant
    LAW_ERR_ADOPTED,    // déjà adoptée
    LAW_ERR_FULL        // file des lois en attente pleine
};

u8 law_is_adopted(const Country *c, u8 id);

// Vérifie qu'une loi peut être proposée (LAW_OK ou code d'erreur).
u8 law_can_propose(const Country *c, u8 id);

// Applique les lois arrivées à maturité ; écrit leurs identifiants dans out
// (taille SIM_MAX_PENDING), renvoie leur nombre.
u8 law_process_pending(Country *c, u8 *out);

#endif
