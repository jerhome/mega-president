#ifndef GAME_STATE_H
#define GAME_STATE_H

// Structure de l'état d'une partie, portable (sans SGDK) : sauvegardée en SRAM,
// vérifiée par game_sanitize() et testée sur PC.

#include "sim/sim.h"
#include "sim/events.h"
#include "sim/ids_v1.h"

typedef struct {
    Country    country;
    TurnReport report;        // compte rendu du dernier trimestre
    s16        event;         // événement à présenter (EVT_*), -1 si aucun
    s8         event_choice;  // choix fait pour l'événement du tour, -1 si aucun
    LawMask    laws_tried;    // lois proposées ce trimestre (bit = id), pour le journal
    LawMask    laws_adopted;  // lois adoptées ce trimestre (bit = id)
    ElectionResult election;  // dernière présidentielle
    LegislativeResult legislatives;  // dernières législatives (dissolution ou 2nd mandat)
    u8         reelected;     // 1 si réélu à la fin du 1er mandat
    u16        score;         // score affiché en fin de partie (mandat ou cumul)
} Game;

// Contrôle de cohérence d'une partie chargée (bornes de tous les index et
// compteurs). Renvoie 1 si la partie est utilisable, 0 si elle doit être refusée.
// Ne modifie pas *g.
u8  game_sanitize(const Game *g);
// Empreinte 16 bits du contenu et de la disposition (EVT/LOI/TXT_COUNT, tailles
// et décalages de Game et de Country) : change dès qu'un ajout de contenu ou de
// champ rendrait une ancienne sauvegarde illisible.
u16 game_fingerprint(void);

#endif
