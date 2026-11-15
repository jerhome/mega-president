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
    // V1 (docs/CONTRAT_V1.md §2, ordre imposé)
    LOI_RETRAITES,   // report de l'âge de départ
    LOI_ISF,         // impôt sur la fortune
    LOI_HOPITAL,     // plan pour l'hôpital public
    LOI_ECOLE,       // recrutement d'enseignants
    LOI_ENERGIE,     // nucléaire et renouvelables
    LOI_TRANSPORTS,  // trains du quotidien
    LOI_IMMIGRATION, // durcissement des conditions d'entrée
    LOI_CHOMAGE,     // réforme de l'assurance chômage
    LOI_NUMERIQUE,   // taxe sur les géants du numérique
    LOI_LOGEMENT,    // logements sociaux
    // V2 (docs/contenu/lois_v2.md, 25 lois, ordre du fichier)
    LOI_SURPROFITS,
    LOI_SEMAINE4J,
    LOI_CANTINE,
    LOI_VOLS_COURTS,
    LOI_DESERTS_MED,
    LOI_RIC,
    LOI_PROPORTIONNELLE,
    LOI_AIDE_DEV,
    LOI_POLICE_PROX,
    LOI_ENCADREMENT,
    LOI_BIO_CANTINE,
    LOI_CULTURE,
    LOI_FLAT_TAX,
    LOI_LICENCIEMENT,
    LOI_UNIFORME,
    LOI_VIDEO,
    LOI_PARLEMENTAIRES,
    LOI_LPM,
    LOI_SERVICE_MIL,
    LOI_ALLIANCE,
    LOI_EXPORT_ARMES,
    LOI_AGRI_PRODUCT,
    LOI_CARBONE,
    LOI_CYBER,
    LOI_PEINES_PLANCHERS,
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
    u8 sovereign;     // V2 : loi de souveraineté (autorisée en cohabitation)
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
    LAW_ERR_FULL,       // file des lois en attente pleine
    LAW_ERR_493,        // 49.3 déjà utilisé cette année (hors budget)
    LAW_ERR_STATE,      // partie terminée
    LAW_ERR_COHAB       // V2 : cohabitation, loi réservée au gouvernement
};

// Coût effectif en CP (réduit de 1 avec le PM Réformateur, minimum 1).
u8 law_cost(const Country *c, const LawDef *law);

u8 law_is_adopted(const Country *c, u8 id);

// Vérifie qu'une loi peut être proposée (LAW_OK ou code d'erreur).
u8 law_pending_turns(const Country *c, u8 id);
u8 law_can_propose(const Country *c, u8 id);

// Applique les lois arrivées à maturité ; écrit leurs identifiants dans out
// (taille SIM_MAX_PENDING), renvoie leur nombre.
u8 law_process_pending(Country *c, u8 *out);

#endif
