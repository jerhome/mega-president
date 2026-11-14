// Mega Président — gestion des écrans (tâche 1.1).
//
// Chaque écran fournit init / update / draw / exit (pointeurs pouvant être
// NULL). screen_run() appelle, à chaque image : input_update(), update(),
// draw(), puis attend la synchro verticale. screen_change() est différé à
// l'image suivante : exit() de l'ancien écran, effacement, init() du nouveau.
// draw() est appelé à chaque image : un écran ne redessine que ce qui a changé
// (les tuiles restent affichées d'une image à l'autre).
#ifndef SCREEN_H
#define SCREEN_H

#include <genesis.h>

typedef enum {
    SCREEN_TITLE = 0,
    SCREEN_GAME,
    SCREEN_EVENT,
    SCREEN_ELECTION,
    SCREEN_REPORT,    // journal de fin de tour
    SCREEN_END,
    SCREEN_SCORES,    // high scores (consultation, saisie des initiales)
    SCREEN_HELP,      // aide (règles), retour à l'écran appelant
    SCREEN_IMPEACH,   // procédure de destitution : choix de la défense
    SCREEN_CAMPAIGN,  // thème de campagne avant la présidentielle
    SCREEN_MANDATE2,  // début du 2nd mandat (législatives)
    SCREEN_BUDGET,    // budget annuel
    SCREEN_REFERENDUM,  // V2 : référendum
    SCREEN_RESHUFFLE,   // V2 : remaniement (choix du nouveau PM)
    SCREEN_MAP,       // carte de popularité par région
    SCREEN_COUNT
} ScreenId;

typedef struct {
    void (*init)(void);
    void (*update)(void);
    void (*draw)(void);
    void (*exit)(void);
} Screen;

void screen_register(ScreenId id, const Screen *screen);
void screen_change(ScreenId id);
ScreenId screen_current(void);
// Millisecondes écoulées depuis l'entrée dans l'écran courant (PAL = NTSC).
u32 screen_time_ms(void);
// Boucle principale : initialise ui et manette, entre dans « first », ne revient pas.
void screen_run(ScreenId first);

#endif
