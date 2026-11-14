// Mega Président — manette (tâche 1.4).
//
// Manette 1, 3 boutons suffisent (A, B, C, Start). input_update() est appelé
// une fois par image par la boucle des écrans (screen_run).
// Les délais sont exprimés en temps réel (millisecondes) : identiques en PAL
// (50 Hz) et en NTSC (60 Hz).
#ifndef INPUT_H
#define INPUT_H

#include <genesis.h>

#define INPUT_UP     BUTTON_UP
#define INPUT_DOWN   BUTTON_DOWN
#define INPUT_LEFT   BUTTON_LEFT
#define INPUT_RIGHT  BUTTON_RIGHT
#define INPUT_A      BUTTON_A
#define INPUT_B      BUTTON_B
#define INPUT_C      BUTTON_C
#define INPUT_START  BUTTON_START
#define INPUT_DIRS   (INPUT_UP | INPUT_DOWN | INPUT_LEFT | INPUT_RIGHT)
#define INPUT_OK     INPUT_A               // valider (C est réservé à l'aide)
#define INPUT_CANCEL INPUT_B               // annuler / retour

#define INPUT_REPEAT_DELAY_MS 330   // avant la première répétition
#define INPUT_REPEAT_RATE_MS  100   // puis une répétition tous les ...

void input_init(void);
void input_update(void);

u16 input_held(void);     // boutons enfoncés
u16 input_pressed(void);  // boutons enfoncés à cette image seulement (front)
// Front + répétition automatique des directions maintenues.
// Pour la navigation dans les menus et listes.
u16 input_repeat(void);

// Durée d'une image en 1/300 s (6 en PAL, 5 en NTSC).
u16 input_frame_ticks(void);

#endif
