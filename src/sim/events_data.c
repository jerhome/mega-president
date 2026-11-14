// FICHIER GÉNÉRÉ par tools/content/gen_content.py à partir de docs/contenu/ :
// ne pas modifier à la main (relancer le générateur).

#include "sim/events.h"

// Données des événements : identifiants, conditions, poids, effets. Aucun texte.
const EventDef event_defs[EVT_COUNT] = {
    { EVT_CANICULE, 2, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 3, { { EFF_SOCIAL, 3 }, { EFF_DETTE, 2 }, { EFF_POP_POPULAIRES, 2 } } },
        { 3, { { EFF_ECOLOGIE, 3 }, { EFF_POP_MOYENNES, -2 }, { EFF_POP_POPULAIRES, -2 } } },
        { 3, { { EFF_ECONOMIE, 2 }, { EFF_ECOLOGIE, -3 }, { EFF_POP_MOYENNES, 2 } } },
    } },
    { EVT_GREVE_GENERALE, 4, { EVV_TENSION, EVO_GT, 70 }, 3, {
        { 4, { { EFF_SOCIAL, 4 }, { EFF_TENSION, -10 }, { EFF_DETTE, 2 }, { EFF_POP_AISEES, -3 } } },
        { 3, { { EFF_TENSION, 5 }, { EFF_ECONOMIE, 2 }, { EFF_POP_POPULAIRES, -4 } } },
        { 4, { { EFF_TENSION, 3 }, { EFF_SECURITE, 2 }, { EFF_POP_MOYENNES, 2 }, { EFF_POP_POPULAIRES, -3 } } },
    } },
    { EVT_CRISE_FINANCIERE, 1, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 4, { { EFF_ECONOMIE, 3 }, { EFF_DETTE, 4 }, { EFF_POP_AISEES, 2 }, { EFF_POP_POPULAIRES, -2 } } },
        { 3, { { EFF_ECONOMIE, -3 }, { EFF_CHOMAGE, 3 }, { EFF_POP_AISEES, -3 } } },
        { 4, { { EFF_DETTE, -2 }, { EFF_POP_POPULAIRES, 3 }, { EFF_POP_AISEES, -4 }, { EFF_CROISSANCE, -1 } } },
    } },
    { EVT_SOMMET, 2, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 3, { { EFF_POP_MOYENNES, 3 }, { EFF_ECONOMIE, -2 }, { EFF_CP, -1 } } },
        { 3, { { EFF_ECONOMIE, 3 }, { EFF_POP_MOYENNES, -2 }, { EFF_CP, 1 } } },
        { 3, { { EFF_ECOLOGIE, 3 }, { EFF_TECHNOLOGIE, 2 }, { EFF_DETTE, 2 } } },
    } },
    { EVT_EMEUTES, 2, { EVV_TENSION, EVO_GT, 50 }, 3, {
        { 4, { { EFF_SECURITE, 3 }, { EFF_TENSION, -5 }, { EFF_POP_POPULAIRES, -3 }, { EFF_REL_G5, 4 } } },
        { 4, { { EFF_SOCIAL, 2 }, { EFF_TENSION, -3 }, { EFF_SECURITE, -2 }, { EFF_POP_MOYENNES, -3 } } },
        { 4, { { EFF_SOCIAL, 3 }, { EFF_DETTE, 3 }, { EFF_POP_POPULAIRES, 2 }, { EFF_POP_MOYENNES, -2 } } },
    } },
    { EVT_USINE_FERMEE, 4, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 4, { { EFF_CHOMAGE, -2 }, { EFF_DETTE, 3 }, { EFF_POP_POPULAIRES, 3 }, { EFF_POP_AISEES, -3 } } },
        { 4, { { EFF_SOCIAL, 2 }, { EFF_DETTE, 1 }, { EFF_TENSION, -2 }, { EFF_POP_AISEES, -1 } } },
        { 4, { { EFF_CHOMAGE, 2 }, { EFF_ECONOMIE, 2 }, { EFF_POP_POPULAIRES, -3 }, { EFF_TENSION, 3 } } },
    } },
    { EVT_DECOUVERTE, 1, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 3, { { EFF_TECHNOLOGIE, 4 }, { EFF_DETTE, 2 }, { EFF_POP_MOYENNES, 2 } } },
        { 4, { { EFF_ECONOMIE, 3 }, { EFF_CROISSANCE, 1 }, { EFF_POP_AISEES, 2 }, { EFF_POP_POPULAIRES, -2 } } },
        { 2, { { EFF_TECHNOLOGIE, 2 }, { EFF_ECOLOGIE, 1 } } },
    } },
    { EVT_POLLUTION, 2, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 4, { { EFF_ECOLOGIE, 4 }, { EFF_DETTE, 1 }, { EFF_ECONOMIE, -2 }, { EFF_POP_AISEES, -2 } } },
        { 3, { { EFF_ECONOMIE, 2 }, { EFF_ECOLOGIE, -3 }, { EFF_POP_MOYENNES, -3 } } },
        { 3, { { EFF_ECOLOGIE, 2 }, { EFF_DETTE, 2 }, { EFF_SCANDALE, -2 } } },
    } },
    { EVT_DINER_LOBBY, 2, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 3, { { EFF_SCANDALE, 5 }, { EFF_POP_MOYENNES, -2 }, { EFF_REL_PARTI, 3 } } },
        { 4, { { EFF_SCANDALE, -3 }, { EFF_CP, -1 }, { EFF_POP_POPULAIRES, 2 }, { EFF_POP_AISEES, -2 } } },
        { 3, { { EFF_SCANDALE, -1 }, { EFF_CP, -1 }, { EFF_POP_TOUS, 1 } } },
    } },
    { EVT_COMMUNICATION, 4, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 3, { { EFF_POP_TOUS, 1 }, { EFF_CP, -1 }, { EFF_SCANDALE, -1 } } },
        { 3, { { EFF_SCANDALE, 4 }, { EFF_POP_MOYENNES, -2 }, { EFF_POP_POPULAIRES, -2 } } },
        { 3, { { EFF_POP_MOYENNES, 2 }, { EFF_POP_AISEES, -2 }, { EFF_SCANDALE, 2 } } },
    } },
    { EVT_EPIDEMIE, 2, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 4, { { EFF_SOCIAL, 3 }, { EFF_DETTE, 3 }, { EFF_POP_POPULAIRES, 2 }, { EFF_ECONOMIE, -1 } } },
        { 4, { { EFF_SOCIAL, 1 }, { EFF_DETTE, 1 }, { EFF_POP_MOYENNES, -1 }, { EFF_CP, -1 } } },
        { 3, { { EFF_SOCIAL, -3 }, { EFF_POP_POPULAIRES, -4 }, { EFF_TENSION, 3 } } },
    } },
    { EVT_GREVE_ENSEIGNANTS, 2, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 4, { { EFF_SOCIAL, 3 }, { EFF_DETTE, 3 }, { EFF_TENSION, -4 }, { EFF_POP_AISEES, -2 } } },
        { 3, { { EFF_TENSION, 3 }, { EFF_CP, -1 }, { EFF_POP_MOYENNES, -2 } } },
        { 4, { { EFF_TENSION, 5 }, { EFF_SECURITE, 1 }, { EFF_POP_MOYENNES, 2 }, { EFF_SOCIAL, -2 } } },
    } },
    { EVT_CRISE_LAIT, 2, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 4, { { EFF_ECONOMIE, -2 }, { EFF_DETTE, 2 }, { EFF_POP_MOYENNES, 3 }, { EFF_POP_POPULAIRES, -2 } } },
        { 3, { { EFF_DETTE, 3 }, { EFF_TENSION, -3 }, { EFF_ECOLOGIE, -1 } } },
        { 4, { { EFF_ECONOMIE, 1 }, { EFF_TENSION, 4 }, { EFF_POP_MOYENNES, -4 }, { EFF_ECOLOGIE, -1 } } },
    } },
    { EVT_CYBERATTAQUE, 2, { EVV_TECHNOLOGIE, EVO_LT, 400 }, 3, {
        { 4, { { EFF_TECHNOLOGIE, 3 }, { EFF_SECURITE, 2 }, { EFF_DETTE, 3 }, { EFF_POP_MOYENNES, -1 } } },
        { 3, { { EFF_SCANDALE, 2 }, { EFF_SOCIAL, -2 }, { EFF_POP_POPULAIRES, -3 } } },
        { 4, { { EFF_SCANDALE, 5 }, { EFF_DETTE, 1 }, { EFF_POP_AISEES, -1 }, { EFF_CP, -1 } } },
    } },
    { EVT_GEANT_NUMERIQUE, 1, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 4, { { EFF_DETTE, -3 }, { EFF_POP_POPULAIRES, 3 }, { EFF_TECHNOLOGIE, -2 }, { EFF_CROISSANCE, -1 } } },
        { 4, { { EFF_ECONOMIE, 1 }, { EFF_SCANDALE, 2 }, { EFF_POP_POPULAIRES, -3 }, { EFF_CP, -1 } } },
        { 4, { { EFF_TECHNOLOGIE, 2 }, { EFF_POP_AISEES, 2 }, { EFF_POP_POPULAIRES, -4 }, { EFF_SCANDALE, 2 } } },
    } },
    { EVT_LOGEMENT, 4, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 4, { { EFF_SOCIAL, 3 }, { EFF_ECONOMIE, -2 }, { EFF_POP_POPULAIRES, 3 }, { EFF_POP_AISEES, -3 } } },
        { 4, { { EFF_SOCIAL, 2 }, { EFF_DETTE, 4 }, { EFF_CHOMAGE, -1 }, { EFF_ECOLOGIE, -2 } } },
        { 4, { { EFF_ECONOMIE, 2 }, { EFF_SOCIAL, -3 }, { EFF_POP_AISEES, 2 }, { EFF_POP_POPULAIRES, -4 } } },
    } },
    { EVT_GREVE_RAIL, 2, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 4, { { EFF_SOCIAL, 2 }, { EFF_DETTE, 3 }, { EFF_TENSION, -4 }, { EFF_ECONOMIE, -1 } } },
        { 3, { { EFF_ECONOMIE, 2 }, { EFF_TENSION, 5 }, { EFF_POP_POPULAIRES, -3 } } },
        { 4, { { EFF_SECURITE, 1 }, { EFF_TENSION, 6 }, { EFF_POP_POPULAIRES, -3 }, { EFF_CP, -1 } } },
    } },
    { EVT_AFFAIRE_JUDICIAIRE, 4, { EVV_SCANDALE, EVO_GT, 40 }, 3, {
        { 4, { { EFF_SCANDALE, -4 }, { EFF_CP, -2 }, { EFF_POP_POPULAIRES, 2 }, { EFF_REL_PARTI, -2 } } },
        { 3, { { EFF_SCANDALE, 6 }, { EFF_POP_TOUS, -2 }, { EFF_REL_PARTI, 2 } } },
        { 4, { { EFF_SCANDALE, -2 }, { EFF_CP, -2 }, { EFF_DEFIANCE, 2 }, { EFF_POP_MOYENNES, 1 } } },
    } },
    { EVT_CRISE_DIPLO, 2, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 4, { { EFF_SECURITE, 2 }, { EFF_ECONOMIE, -2 }, { EFF_POP_MOYENNES, 2 }, { EFF_REL_G5, 3 } } },
        { 4, { { EFF_ECONOMIE, 1 }, { EFF_CP, -1 }, { EFF_POP_MOYENNES, -3 }, { EFF_REL_G5, -3 } } },
        { 4, { { EFF_CP, -1 }, { EFF_POP_MOYENNES, -1 }, { EFF_ECOLOGIE, 1 }, { EFF_DEFIANCE, 1 } } },
    } },
    { EVT_PRIX_ELECTRICITE, 2, { EVV_TENSION, EVO_GT, 40 }, 3, {
        { 4, { { EFF_POP_POPULAIRES, 3 }, { EFF_POP_MOYENNES, 2 }, { EFF_DETTE, 4 }, { EFF_ECOLOGIE, -1 } } },
        { 4, { { EFF_SOCIAL, 2 }, { EFF_DETTE, 2 }, { EFF_POP_MOYENNES, -2 }, { EFF_POP_AISEES, -1 } } },
        { 4, { { EFF_ECONOMIE, 1 }, { EFF_ECOLOGIE, 1 }, { EFF_TENSION, 4 }, { EFF_POP_POPULAIRES, -5 } } },
    } },
    { EVT_SECHERESSE, 2, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 3, { { EFF_ECOLOGIE, 3 }, { EFF_TENSION, 3 }, { EFF_POP_MOYENNES, -2 } } },
        { 4, { { EFF_ECONOMIE, 2 }, { EFF_DETTE, 2 }, { EFF_POP_MOYENNES, 2 }, { EFF_ECOLOGIE, -2 } } },
        { 3, { { EFF_ECOLOGIE, 3 }, { EFF_DETTE, 2 }, { EFF_POP_POPULAIRES, 1 } } },
    } },
    { EVT_CLASSEMENT_EDU, 1, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 3, { { EFF_SOCIAL, 3 }, { EFF_DETTE, 2 }, { EFF_POP_POPULAIRES, 2 } } },
        { 3, { { EFF_TECHNOLOGIE, 2 }, { EFF_POP_MOYENNES, 2 }, { EFF_POP_POPULAIRES, -2 } } },
        { 2, { { EFF_POP_MOYENNES, -2 }, { EFF_SCANDALE, 3 } } },
    } },
    { EVT_EVENEMENT_SPORTIF, 1, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 3, { { EFF_ECONOMIE, 3 }, { EFF_DETTE, 3 }, { EFF_POP_TOUS, 1 } } },
        { 3, { { EFF_ECOLOGIE, 2 }, { EFF_DETTE, -1 }, { EFF_POP_MOYENNES, -1 } } },
        { 3, { { EFF_SOCIAL, 2 }, { EFF_POP_POPULAIRES, 3 }, { EFF_DETTE, 1 } } },
    } },
    { EVT_JUSTICE_MOYENS, 2, { EVV_SCANDALE, EVO_GT, 25 }, 3, {
        { 4, { { EFF_SECURITE, 2 }, { EFF_DETTE, 2 }, { EFF_SCANDALE, -4 }, { EFF_POP_MOYENNES, 1 } } },
        { 3, { { EFF_ECONOMIE, 1 }, { EFF_SCANDALE, 3 }, { EFF_POP_POPULAIRES, -2 } } },
        { 3, { { EFF_SCANDALE, -2 }, { EFF_POP_POPULAIRES, 1 }, { EFF_POP_AISEES, -1 } } },
    } },
    { EVT_OUTREMER, 2, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 4, { { EFF_SOCIAL, 2 }, { EFF_DETTE, 2 }, { EFF_TENSION, -4 }, { EFF_POP_POPULAIRES, 2 } } },
        { 3, { { EFF_ECONOMIE, 2 }, { EFF_CHOMAGE, -2 }, { EFF_DETTE, 2 } } },
        { 3, { { EFF_TENSION, 3 }, { EFF_SCANDALE, 2 }, { EFF_POP_AISEES, 1 } } },
    } },
    { EVT_INFLATION_ALIM, 4, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 3, { { EFF_POP_POPULAIRES, 3 }, { EFF_DETTE, 2 }, { EFF_SOCIAL, 1 } } },
        { 4, { { EFF_POP_POPULAIRES, 2 }, { EFF_POP_AISEES, -2 }, { EFF_ECONOMIE, -2 }, { EFF_TENSION, -2 } } },
        { 3, { { EFF_ECONOMIE, 2 }, { EFF_POP_POPULAIRES, -3 }, { EFF_TENSION, 3 } } },
    } },
    { EVT_LAICITE_ECOLE, 1, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 4, { { EFF_SECURITE, 2 }, { EFF_TENSION, 2 }, { EFF_POP_MOYENNES, 2 }, { EFF_POP_POPULAIRES, -2 } } },
        { 4, { { EFF_SOCIAL, 2 }, { EFF_TENSION, -3 }, { EFF_DETTE, 1 }, { EFF_POP_MOYENNES, -1 } } },
        { 3, { { EFF_SCANDALE, -1 }, { EFF_TENSION, -1 }, { EFF_POP_MOYENNES, 1 } } },
    } },
    { EVT_DESINFORMATION, 2, { EVV_DEFIANCE, EVO_GT, 40 }, 3, {
        { 3, { { EFF_DEFIANCE, -4 }, { EFF_POP_MOYENNES, 1 }, { EFF_TENSION, -2 } } },
        { 4, { { EFF_DEFIANCE, -2 }, { EFF_TECHNOLOGIE, -2 }, { EFF_POP_AISEES, -2 }, { EFF_SECURITE, 1 } } },
        { 2, { { EFF_DEFIANCE, 4 }, { EFF_SCANDALE, 2 } } },
    } },
    { EVT_ACCIDENT_INDUSTRIEL, 1, { EVV_NONE, EVO_GT, 0 }, 3, {
        { 4, { { EFF_ECOLOGIE, 3 }, { EFF_SECURITE, 1 }, { EFF_ECONOMIE, -2 }, { EFF_POP_AISEES, -2 } } },
        { 3, { { EFF_SECURITE, 2 }, { EFF_ECOLOGIE, 2 }, { EFF_DETTE, 2 } } },
        { 4, { { EFF_POP_POPULAIRES, 2 }, { EFF_DETTE, 1 }, { EFF_SCANDALE, 2 }, { EFF_ECOLOGIE, -2 } } },
    } },
    { EVT_VOTE_JEUNES, 2, { EVV_POPULARITE, EVO_LT, 450 }, 3, {
        { 3, { { EFF_POP_TOUS, 1 }, { EFF_TENSION, -3 }, { EFF_SCANDALE, -1 } } },
        { 4, { { EFF_SOCIAL, 3 }, { EFF_CHOMAGE, -2 }, { EFF_DETTE, 3 }, { EFF_POP_POPULAIRES, 2 } } },
        { 3, { { EFF_TENSION, 4 }, { EFF_POP_MOYENNES, 1 }, { EFF_POP_POPULAIRES, -3 } } },
    } },
};
