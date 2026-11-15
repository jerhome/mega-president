#ifndef SCREENS_H
#define SCREENS_H

#include "ui/screen.h"

// Écrans du jeu (un fichier par écran dans src/screens/).
extern const Screen scr_title;
extern const Screen scr_game;
extern const Screen scr_report;
extern const Screen scr_event;
extern const Screen scr_election;
extern const Screen scr_end;
extern const Screen scr_scores;
extern const Screen scr_help;
extern const Screen scr_impeach;
extern const Screen scr_campaign;
extern const Screen scr_mandate2;
extern const Screen scr_budget;
extern const Screen scr_referendum;
extern const Screen scr_reshuffle;

extern const Screen scr_map;
extern const Screen scr_options;
extern const Screen scr_elec;

// V4 : tendance des électorats (scr_elec.c). t : -1 baisse, 0 stable, +1 hausse.
#include "sim/sim_state.h"
extern s16 g_opinion_prev[ELEC_COUNT];
extern u8  g_opinion_prev_ok;
s8   elec_trend(const Country *c, u8 e);
void elec_trend_icon(u16 x, u16 y, s8 t);

#endif
