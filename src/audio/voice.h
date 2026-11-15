// Voix digitalisée « For sure ! » (clin d'œil de l'écran d'accueil), jouée sur
// le canal PCM 3 du pilote XGM2 (le même pilote que la musique et les bruitages).
// Jouée une seule fois par allumage de la console.
#ifndef AUDIO_VOICE_H
#define AUDIO_VOICE_H

#include <genesis.h>

// Lance la voix si elle n'a pas encore été jouée depuis l'allumage.
// Retourne TRUE si elle démarre (l'appelant peut retarder la musique).
bool voice_play_intro_once(void);
// TRUE tant que la voix est en cours.
bool voice_busy(void);
// Réglage « Voix » (Options). Coupée : voice_play_intro_once ne joue rien (et renvoie FALSE).
void voice_set_enabled(bool on);
// « For sure ! » à la demande (aperçu du réglage), canal 3.
void voice_preview(void);

#endif
