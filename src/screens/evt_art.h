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

#endif
