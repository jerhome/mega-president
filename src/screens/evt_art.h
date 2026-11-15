// Mega Président — illustrations des écrans : table événement -> vignette et
// remise à zéro de l'écran (effacement + restitution des palettes empruntées).
#ifndef EVT_ART_H
#define EVT_ART_H

#include <genesis.h>
#include "ui/ui_art.h"

// Vignette ART_EVT_* associée à l'événement EVT_* (sim/events_ids.h).
ArtId evt_art(u8 evt);

// ui_clear() puis ui_art_close() : à appeler à la place de ui_clear() sur les
// écrans illustrés, et dans leur fonction exit.
void art_reset(void);

// Ministre de tutelle de l'événement (revue DA § 8.3), selon sa vignette : Écologie
// (canicule, pollution, énergie, intempéries), Économie (bourse, industrie,
// numérique, agriculture), Intérieur (sécurité, scandale), Santé, Éducation,
// Affaires étrangères (sommet) ; grève et fête : le Premier ministre en place
// (ou celui de la cohabitation).
GueuleId evt_minister(u8 evt);

// Humeur du ministre devant le choix : EXPR_CONTENT si les effets du choix
// servent son domaine (somme pondérée > 0), EXPR_GENE s'ils le desservent (< 0),
// EXPR_NEUTRE si le choix ne touche pas son domaine.
Expr evt_mood(u8 evt, u8 choice);

#endif
