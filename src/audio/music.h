// Musique de fond : séquenceur PSG (SN76489) sur les canaux 1, 2 (tons) et 3
// (bruit). Le canal 0 reste réservé aux bruitages (audio/sfx.h).
// Compositions originales. Durées en millisecondes : identique en PAL et NTSC.
#ifndef AUDIO_MUSIC_H
#define AUDIO_MUSIC_H

#include <genesis.h>

typedef enum {
    MUS_TITLE = 0,   // marche lente solennelle, ~32 s en boucle
    MUS_GAME,        // tableau de bord : discret, réflexif, ~31 s
    MUS_ELECTION,    // tension puis résolution, ~32 s
    MUS_COUNT
} MusicId;

void music_init(void);             // une fois au démarrage, APRÈS sfx_init() (qui fait PSG_reset)
void music_play(MusicId id);       // lance (ou relance) un morceau en boucle ; sans effet si déjà en cours
void music_stop(void);             // silence les canaux 1 à 3
void music_update(void);           // une fois par image
void music_set_enabled(bool on);   // coupe/active la musique (le choix est mémorisé)

#endif
