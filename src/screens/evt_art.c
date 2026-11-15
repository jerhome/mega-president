// Mega Président — table des vignettes d'événements (45 événements, 15 vignettes).
#include "screens/evt_art.h"
#include "ui/ui.h"
#include "sim/events_ids.h"
#include "sim/events.h"
#include "sim/cohab.h"
#include "game/game.h"

static const u8 evt_art_table[EVT_COUNT] = {
    [EVT_CANICULE] = ART_EVT_CANICULE,
    [EVT_GREVE_GENERALE] = ART_EVT_GREVE,
    [EVT_CRISE_FINANCIERE] = ART_EVT_BOURSE,
    [EVT_SOMMET] = ART_EVT_SOMMET,
    [EVT_EMEUTES] = ART_EVT_SECURITE,
    [EVT_USINE_FERMEE] = ART_EVT_INDUSTRIE,
    [EVT_DECOUVERTE] = ART_EVT_FETE,
    [EVT_POLLUTION] = ART_EVT_POLLUTION,
    [EVT_DINER_LOBBY] = ART_EVT_SCANDALE,
    [EVT_COMMUNICATION] = ART_EVT_SCANDALE,
    [EVT_EPIDEMIE] = ART_EVT_SANTE,
    [EVT_GREVE_ENSEIGNANTS] = ART_EVT_EDUCATION,
    [EVT_CRISE_LAIT] = ART_EVT_AGRICULTURE,
    [EVT_CYBERATTAQUE] = ART_EVT_NUMERIQUE,
    [EVT_GEANT_NUMERIQUE] = ART_EVT_NUMERIQUE,
    [EVT_LOGEMENT] = ART_EVT_GREVE,
    [EVT_GREVE_RAIL] = ART_EVT_GREVE,
    [EVT_AFFAIRE_JUDICIAIRE] = ART_EVT_SCANDALE,
    [EVT_CRISE_DIPLO] = ART_EVT_SOMMET,
    [EVT_PRIX_ELECTRICITE] = ART_EVT_ENERGIE,
    [EVT_SECHERESSE] = ART_EVT_CANICULE,
    [EVT_CLASSEMENT_EDU] = ART_EVT_EDUCATION,
    [EVT_EVENEMENT_SPORTIF] = ART_EVT_FETE,
    [EVT_JUSTICE_MOYENS] = ART_EVT_SCANDALE,
    [EVT_OUTREMER] = ART_EVT_GREVE,
    [EVT_INFLATION_ALIM] = ART_EVT_BOURSE,
    [EVT_LAICITE_ECOLE] = ART_EVT_EDUCATION,
    [EVT_DESINFORMATION] = ART_EVT_NUMERIQUE,
    [EVT_ACCIDENT_INDUSTRIEL] = ART_EVT_INDUSTRIE,
    [EVT_VOTE_JEUNES] = ART_EVT_EDUCATION,
    [EVT_AFFAIRE_SUITE] = ART_EVT_SCANDALE,
    [EVT_SENAT_FRONDEUR] = ART_EVT_SCANDALE,
    [EVT_COUR_COMPTES] = ART_EVT_BOURSE,
    [EVT_AGENCE_NOTATION] = ART_EVT_BOURSE,
    [EVT_SOMMET_CLIMAT] = ART_EVT_POLLUTION,
    [EVT_NOBEL] = ART_EVT_FETE,
    [EVT_URGENCES_PEDIATRIQUES] = ART_EVT_SANTE,
    [EVT_CLASSES_RURALES] = ART_EVT_AGRICULTURE,
    [EVT_FESTIVAL] = ART_EVT_FETE,
    [EVT_LYCEENS] = ART_EVT_EDUCATION,
    [EVT_GREVE_AIGUILLEURS] = ART_EVT_GREVE,
    [EVT_REFORME_INSTITUTIONS] = ART_EVT_SOMMET,
    [EVT_ESPIONNAGE] = ART_EVT_SECURITE,
    [EVT_PANNE_ELECTRIQUE] = ART_EVT_ENERGIE,
    [EVT_MAREE_NOIRE] = ART_EVT_POLLUTION,
};

ArtId evt_art(u8 evt)
{
    // Entrée absente (0 = ART_TITLE) ou hors table : vignette générique.
    if (evt >= EVT_COUNT || evt_art_table[evt] < ART_FIRST_EVENT)
        return ART_EVT_SCANDALE;
    return (ArtId)evt_art_table[evt];
}

void art_reset(void)
{
    ui_clear();
    ui_art_close();
}

// Indice du ministre (GUEULE_MIN) ou -1 pour le Premier ministre.
static s8 evt_min_index(u8 evt)
{
    switch (evt_art(evt)) {
    case ART_EVT_CANICULE: case ART_EVT_POLLUTION: case ART_EVT_ENERGIE:
    case ART_EVT_INONDATION:
        return GUEULE_MIN_ECOLOGIE - GUEULE_MIN_ECONOMIE;
    case ART_EVT_BOURSE: case ART_EVT_INDUSTRIE: case ART_EVT_NUMERIQUE:
    case ART_EVT_AGRICULTURE:
        return 0;
    case ART_EVT_SECURITE: case ART_EVT_SCANDALE:
        return GUEULE_MIN_INTERIEUR - GUEULE_MIN_ECONOMIE;
    case ART_EVT_SANTE:
        return GUEULE_MIN_SANTE - GUEULE_MIN_ECONOMIE;
    case ART_EVT_EDUCATION:
        return GUEULE_MIN_EDUCATION - GUEULE_MIN_ECONOMIE;
    case ART_EVT_SOMMET:
        return GUEULE_MIN_AFFAIRES_ETR - GUEULE_MIN_ECONOMIE;
    default:
        return -1;
    }
}

GueuleId evt_minister(u8 evt)
{
    s8 i = evt_min_index(evt);
    const Country *c = &g_game.country;
    if (i >= 0)
        return GUEULE_MIN(i);
    return sim_cohabitation(c) ? GUEULE_COHAB(sim_cohab_camp(c)) : GUEULE_PM(c->pm);
}

Expr evt_mood(u8 evt, u8 choice)
{
    const EventDef *def = event_get(evt);
    s8 mi = evt_min_index(evt);
    s16 score = 0;
    u8 i;
    if (!def || choice >= def->n_choices)
        return EXPR_NEUTRE;
    for (i = 0; i < def->choices[choice].n_effects; i++) {
        const Effect *e = &def->choices[choice].effects[i];
        s16 v = e->value;
        switch (mi) {
        case 0:   // Économie : pilier, croissance ; chômage et dette comptent à l'envers
            if (e->key == EFF_ECONOMIE || e->key == EFF_CROISSANCE) score += v;
            else if (e->key == EFF_CHOMAGE || e->key == EFF_DETTE) score -= v;
            break;
        case 1:   // Intérieur : sécurité ; la tension compte à l'envers
            if (e->key == EFF_SECURITE) score += v;
            else if (e->key == EFF_TENSION) score -= v;
            break;
        case 2:   // Santé : pilier social
            if (e->key == EFF_SOCIAL) score += v;
            break;
        case 3:   // Éducation : social et technologie
            if (e->key == EFF_SOCIAL || e->key == EFF_TECHNOLOGIE) score += v;
            break;
        case 4:   // Écologie : pilier écologie
            if (e->key == EFF_ECOLOGIE) score += v;
            break;
        case 5:   // Affaires étrangères : pilier international
            if (e->key == EFF_INTERNATIONAL) score += v;
            break;
        default:  // Premier ministre : opinion, tension à l'envers
            if ((e->key >= EFF_POP_POPULAIRES && e->key <= EFF_POP_TOUS)
                || e->key == EFF_POP_RETRAITES || e->key == EFF_POP_JEUNES) score += v;
            else if (e->key == EFF_TENSION) score -= v;
            break;
        }
    }
    return score > 0 ? EXPR_CONTENT : score < 0 ? EXPR_GENE : EXPR_NEUTRE;
}
