// Musique de fond (V3) : puce FM YM2612 via le pilote Z80 XGM2 de SGDK.
// Les morceaux sont composés en code (tools/music/), exportés en VGM puis
// convertis en une ressource XGM2 multi-pistes au build (res/snd_audio.res).
// Le pilote XGM2 adapte la cadence : tempo identique en PAL et en NTSC.
// Changement de morceau : fondu de sortie court puis fondu d'entrée (pas de clic).
#ifndef AUDIO_MUSIC_H
#define AUDIO_MUSIC_H

#include <genesis.h>

typedef enum {
    MUS_TITLE = 0,   // écran titre : city pop douce
    MUS_YEAR1,       // année 1 : état de grâce (sol majeur, arpège montant)
    MUS_YEAR2,       // année 2 : mise au travail
    MUS_YEAR3,       // année 3 : mi-mandat tendu
    MUS_YEAR4,       // année 4 : doute et usure
    MUS_YEAR5,       // année 5 : marche vers l'élection
    MUS_ELECTION,    // soirée électorale
    MUS_REPORT,      // journal de fin de tour : rédaction, machine à écrire, indicatif de JT
    MUS_EVENT,       // événement
    MUS_ASSEMBLY,    // Assemblée nationale
    MUS_MAP,         // carte des régions
    MUS_BUDGET,      // budget
    MUS_REFERENDUM,  // référendum
    MUS_RESHUFFLE,   // remaniement
    MUS_IMPEACH,     // destitution (tension)
    MUS_CAMPAIGN,    // campagne
    MUS_MANDATE2,    // 2nd mandat
    MUS_END_WIN,     // fin de partie : victoire
    MUS_END_LOSE,    // fin de partie : défaite
    MUS_SCORES,      // meilleurs scores
    MUS_HELP,        // aide
    MUS_COUNT
} MusicId;
#define MUS_GAME MUS_YEAR1   // ancien nom du morceau de jeu

// Pour music_for_screen(SCREEN_END, ...) : le 2e paramètre donne l'issue.
#define MUSIC_END_LOSE 0
#define MUSIC_END_WIN  1

void music_init(void);             // une fois au démarrage, APRÈS sfx_init() (pilote XGM2 chargé)
void music_play(MusicId id);       // lance un morceau en boucle (fondu enchaîné) ; sans effet si déjà en cours
void music_play_year(u8 year);     // thème de l'année du mandat (0 à 4, modulo 5) ; sans effet si déjà en cours
void music_for_screen(u8 screen_id, u8 year); // morceau d'un écran (ScreenId) ; year : année (0..9) du
                                   // tableau de bord, ou MUSIC_END_WIN / MUSIC_END_LOSE pour SCREEN_END
void music_stop(void);             // fondu de sortie court puis arrêt
void music_update(void);           // une fois par image (enchaînements)
void music_set_enabled(bool on);   // coupe/active la musique (le choix est mémorisé)

#endif
