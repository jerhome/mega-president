#ifndef SIM_EVENTS_H
#define SIM_EVENTS_H

// Moteur d'événements (tâche 4.1). Aucun texte ici : les textes sont dans
// src/content/ (indexés par EVT_*). Les données (events_data.c, events_ids.h)
// sont générées par tools/content/gen_content.py depuis docs/contenu/.

#include "sim/sim_state.h"
#include "sim/effects.h"
#include "sim/events_ids.h"

#define EVENT_MAX_CHOICES  3
#define EVENT_MAX_EFFECTS  6

// Variable testée par une condition ; valeur comparée en unités INTERNES
// (jauges, CP : tels quels ; piliers, popularité, dette : dixièmes ;
// chômage, croissance : centièmes).
enum {
    EVV_NONE = 0,      // pas de condition : l'événement est « inconditionnel »
    EVV_TENSION, EVV_SCANDALE, EVV_DEFIANCE, EVV_CP,
    EVV_POPULARITE, EVV_CHOMAGE, EVV_DETTE, EVV_CROISSANCE,
    EVV_SOCIAL, EVV_ECONOMIE, EVV_TECHNOLOGIE, EVV_ECOLOGIE, EVV_SECURITE
};
enum { EVO_GT, EVO_GE, EVO_LT, EVO_LE };

typedef struct {
    u8  var;     // EVV_*
    u8  op;      // EVO_*
    s16 value;   // seuil, unités internes
} EventCond;

typedef struct {
    u8 n_effects;
    Effect effects[EVENT_MAX_EFFECTS];
} EventChoice;

typedef struct {
    u8 id;                 // EVT_*
    u8 weight;             // faible 1, moyenne 2, forte 4
    EventCond cond;
    u8 n_choices;          // 2 ou 3
    EventChoice choices[EVENT_MAX_CHOICES];
} EventDef;

extern const EventDef event_defs[EVT_COUNT];

const EventDef *event_get(u8 id);   // NULL si id invalide

// Capacité : 64 événements (EVT_COUNT <= EVENT_CAPACITY), marqués dans
// Country.events_done (0-31) et events_done_hi (32-63).
#define EVENT_CAPACITY 64
// Lecture / marquage du bit « déjà tiré » d'un événement.
u8   events_done_get(const Country *c, u8 id);
void events_done_set(Country *c, u8 id);

// Valeur d'une variable de condition (unités internes).
s16 events_var_value(const Country *c, u8 var);
// 1 si la condition de l'événement est vraie (toujours vrai sans condition).
u8 events_cond_ok(const Country *c, const EventDef *e);
// 1 si l'événement peut être tiré maintenant : condition vraie, différent du
// précédent ; un événement inconditionnel n'est tiré qu'une fois par mandat,
// un événement conditionnel peut revenir.
u8 events_eligible(const Country *c, u8 id);

// Probabilité (en %) qu'un événement survienne ce tour : 100 à 4 tours/an,
// 33 à 12 tours/an (environ un tour sur trois).
u8 events_chance_pct(const Country *c);

// Tire l'événement du tour (consomme du hasard dans c->rng). Renvoie un EVT_*
// ou -1 s'il n'y en a pas (premier tour, tirage négatif, plus d'événement
// éligible). L'événement est marqué comme tiré.
s16 events_draw(Country *c);

// Applique les effets du choix (0 à n_choices-1) via sim_apply_effects.
// Renvoie 1 si appliqué, 0 si identifiant ou choix invalide.
u8 events_choose(Country *c, u8 event_id, u8 choice);

#endif
