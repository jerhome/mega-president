#ifndef CONTENT_EVENTS_H
#define CONTENT_EVENTS_H

// FICHIER GÉNÉRÉ par tools/content/gen_content.py à partir de docs/contenu/ :
// ne pas modifier à la main (relancer le générateur).

#include "sim/events.h"

// Textes UTF-8 indexés par EVT_*. Les lignes sont séparées par '\n'.
// Vignettes : même ordre que ART_EVT_GREVE.. (src/ui/ui_art.h).
enum {
    VIG_GREVE,
    VIG_CANICULE,
    VIG_INONDATION,
    VIG_SCANDALE,
    VIG_SOMMET,
    VIG_BOURSE,
    VIG_SANTE,
    VIG_POLLUTION,
    VIG_SECURITE,
    VIG_NUMERIQUE,
    VIG_EDUCATION,
    VIG_ENERGIE,
    VIG_AGRICULTURE,
    VIG_INDUSTRIE,
    VIG_FETE,
    VIG_COUNT
};

extern const u8 evt_vignette[EVT_COUNT];
extern const char *const evt_title[EVT_COUNT];
extern const char *const evt_text[EVT_COUNT];                       // 6 lignes max
extern const char *const evt_choice[EVT_COUNT][EVENT_MAX_CHOICES];   // libellés
extern const char *const evt_journal[EVT_COUNT][EVENT_MAX_CHOICES];  // 2 lignes max

#endif
