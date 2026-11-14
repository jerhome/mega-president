// Bruitages des menus : sons courts générés par le PSG (SN76489, canal 0).
// Aucun fichier audio, aucun driver Z80. Durées en millisecondes, donc
// identiques en PAL et en NTSC.
#ifndef AUDIO_SFX_H
#define AUDIO_SFX_H

#include <genesis.h>

typedef enum {
    SFX_MOVE = 0,      // curseur (~25 ms, discret)
    SFX_OK,            // validation (~120 ms)
    SFX_CANCEL,        // retour (~120 ms)
    SFX_ERROR,         // action impossible (~220 ms)
    SFX_ADOPTED,       // loi adoptée, arpège montant (~400 ms)
    SFX_REJECTED,      // loi rejetée, arpège descendant (~400 ms)
    SFX_EVENT,         // arrivée d'un événement (~300 ms)
    SFX_ELECTION_WIN,  // fanfare (~1 s)
    SFX_ELECTION_LOSE, // mineur descendant (~1 s)
    SFX_COUNT
} SfxId;

void sfx_init(void);              // une fois au démarrage
void sfx_play(SfxId id);          // le dernier son gagne, sauf élection (non coupée par SFX_MOVE)
void sfx_update(void);            // une fois par image
void sfx_set_enabled(bool on);    // coupe/active les bruitages (coupe le son en cours)

#endif
