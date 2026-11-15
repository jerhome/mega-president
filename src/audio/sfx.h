// Bruitages des menus (V3) : échantillons PCM courts générés par
// tools/music/pcm.py, joués sur le canal PCM 2 du pilote XGM2 (le canal 1 sert
// aux percussions de la musique, le 3 à la voix). Durées en millisecondes,
// identiques en PAL et en NTSC.
#ifndef AUDIO_SFX_H
#define AUDIO_SFX_H

#include <genesis.h>

typedef enum {
    SFX_MOVE = 0,      // curseur (~35 ms, discret)
    SFX_OK,            // validation (~220 ms)
    SFX_CANCEL,        // retour (~220 ms)
    SFX_ERROR,         // action impossible (~270 ms)
    SFX_ADOPTED,       // loi adoptée, arpège montant (~650 ms)
    SFX_REJECTED,      // loi rejetée, arpège descendant (~700 ms)
    SFX_EVENT,         // arrivée d'un événement (~700 ms)
    SFX_ELECTION_WIN,  // fanfare (~1,5 s)
    SFX_ELECTION_LOSE, // mineur descendant (~1,6 s)
    SFX_COUNT
} SfxId;

void sfx_init(void);              // une fois au démarrage : charge le pilote XGM2
void sfx_play(SfxId id);          // le dernier son gagne, sauf élection (non coupée par SFX_MOVE)
void sfx_update(void);            // une fois par image
void sfx_set_enabled(bool on);    // coupe/active les bruitages (coupe le son en cours)

#endif
