#ifndef CONTENT_EVENTS_H
#define CONTENT_EVENTS_H

// FICHIER GÉNÉRÉ par tools/content/gen_content.py à partir de docs/contenu/ :
// ne pas modifier à la main (relancer le générateur).

#include "sim/events.h"

// Textes UTF-8 indexés par EVT_*. Les lignes sont séparées par '\n'.
extern const char *const evt_title[EVT_COUNT];
extern const char *const evt_text[EVT_COUNT];                       // 6 lignes max
extern const char *const evt_choice[EVT_COUNT][EVENT_MAX_CHOICES];   // libellés
extern const char *const evt_journal[EVT_COUNT][EVENT_MAX_CHOICES];  // 2 lignes max

#endif
