// Voix digitalisée « For sure ! » (clin d'œil de l'écran d'accueil), via le driver
// Z80 PCM de SGDK. Jouée une seule fois par allumage de la console.
// Le PSG (musique, bruitages) est piloté par le 68000 : il n'est pas affecté.
#ifndef AUDIO_VOICE_H
#define AUDIO_VOICE_H

#include <genesis.h>

// Lance la voix si elle n'a pas encore été jouée depuis l'allumage.
// Retourne TRUE si elle démarre (l'appelant peut retarder la musique).
bool voice_play_intro_once(void);
// TRUE tant que la voix est en cours ; libère le driver Z80 à la fin.
bool voice_busy(void);

#endif
