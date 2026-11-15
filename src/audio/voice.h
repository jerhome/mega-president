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


// --- Charabia des personnages (4.18) ---
// Lance une réplique (2 syllabes) avec la voix de la gueule id (GueuleId, ordre de la
// banque). FALSE si la voix est coupée dans les Options (rien n'est joué).
bool voice_say(u16 gueule);
// Boucle des écrans, une fois par image : enchaîne les syllabes (durées au compteur de temps).
void voice_update(void);
// TRUE tant que la réplique est en cours (syllabes et silence entre elles).
bool voice_talking(void);
// Coupe la réplique en cours.
void voice_say_stop(void);
// Forme de bouche à l'instant : 0 repos, 1 peu ouverte, 2 ouverte.
u8 voice_mouth(void);

#endif
