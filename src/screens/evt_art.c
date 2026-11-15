// Mega Président — vignette et ministre de chaque événement.
#include "screens/evt_art.h"
#include "ui/ui.h"
#include "sim/events_ids.h"
#include "sim/events.h"
#include "content/content_events.h"
#include "content/content_repliques.h"
#include "sim/cohab.h"
#include "game/game.h"

// La vignette vient de docs/contenu (champ « Vignette »), émise par gen_content.py
// dans evt_vignette[] ; VIG_* suit l'ordre de ART_EVT_GREVE...
#define VIG_CHECK(v, a) _Static_assert((v) == (a) - ART_FIRST_EVENT, "VIG_* et ART_EVT_* désalignés")
VIG_CHECK(VIG_GREVE, ART_EVT_GREVE);
VIG_CHECK(VIG_CANICULE, ART_EVT_CANICULE);
VIG_CHECK(VIG_INONDATION, ART_EVT_INONDATION);
VIG_CHECK(VIG_SCANDALE, ART_EVT_SCANDALE);
VIG_CHECK(VIG_SOMMET, ART_EVT_SOMMET);
VIG_CHECK(VIG_BOURSE, ART_EVT_BOURSE);
VIG_CHECK(VIG_SANTE, ART_EVT_SANTE);
VIG_CHECK(VIG_POLLUTION, ART_EVT_POLLUTION);
VIG_CHECK(VIG_SECURITE, ART_EVT_SECURITE);
VIG_CHECK(VIG_NUMERIQUE, ART_EVT_NUMERIQUE);
VIG_CHECK(VIG_EDUCATION, ART_EVT_EDUCATION);
VIG_CHECK(VIG_ENERGIE, ART_EVT_ENERGIE);
VIG_CHECK(VIG_AGRICULTURE, ART_EVT_AGRICULTURE);
VIG_CHECK(VIG_INDUSTRIE, ART_EVT_INDUSTRIE);
VIG_CHECK(VIG_FETE, ART_EVT_FETE);
_Static_assert(VIG_COUNT == ART_EVENT_COUNT, "nombre de vignettes");

_Static_assert(REP_PM == GUEULE_MIN_AFFAIRES_ETR - GUEULE_MIN_ECONOMIE + 1, "content_repliques.h : 6 ministres puis le PM");

// Ordre des ministres supposé par evt_min_index et evt_mood (cases 0 à 5).
_Static_assert(GUEULE_MIN_INTERIEUR - GUEULE_MIN_ECONOMIE == 1, "ordre des ministres");
_Static_assert(GUEULE_MIN_SANTE - GUEULE_MIN_ECONOMIE == 2, "ordre des ministres");
_Static_assert(GUEULE_MIN_EDUCATION - GUEULE_MIN_ECONOMIE == 3, "ordre des ministres");
_Static_assert(GUEULE_MIN_ECOLOGIE - GUEULE_MIN_ECONOMIE == 4, "ordre des ministres");
_Static_assert(GUEULE_MIN_AFFAIRES_ETR - GUEULE_MIN_ECONOMIE == 5, "ordre des ministres");

ArtId evt_art(u8 evt)
{
    if (evt >= EVT_COUNT || evt_vignette[evt] >= VIG_COUNT)
        return ART_EVT_SCANDALE;
    return (ArtId)(ART_FIRST_EVENT + evt_vignette[evt]);
}

void art_reset(void)
{
    ui_clear();
    ui_art_close();
}

void art_reset_quiet(void)
{
    ui_clear_quiet();
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
