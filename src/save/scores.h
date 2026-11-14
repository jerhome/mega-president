// Mega Président — classements des high scores (tâches 1.15, 1.16).
//
// Logique PORTABLE (sans SGDK) : compile sur PC (tests) et sur Megadrive.
// L'accès à la SRAM est dans sram.c (console seulement).
//
// Format du bloc sérialisé (SCORES_BLOB_SIZE octets) :
//   0..3   signature « MPV1 »
//   4      version du format (SCORES_VERSION)
//   puis, pour chacun des 2 tableaux :
//     10 entrées de 8 octets : 3 initiales (A-Z, 0 si vide), score u16 (BE),
//                              orientation u8, année de fin u16 (BE)
//     somme de contrôle u16 (BE) du tableau
#ifndef SAVE_SCORES_H
#define SAVE_SCORES_H

#include "sim/sim_types.h"

#define SCORES_COUNT       10
#define SCORES_TABLES      2
#define SCORE_TABLE_MANDAT 0   // 1er mandat
#define SCORE_TABLE_CUMUL  1   // cumul sur 10 ans
#define SCORES_VERSION     1

#define SCORES_ENTRY_SIZE  8
#define SCORES_TABLE_SIZE  (SCORES_COUNT * SCORES_ENTRY_SIZE + 2)
#define SCORES_BLOB_SIZE   (5 + SCORES_TABLES * SCORES_TABLE_SIZE)

typedef struct {
    char ini[3];    // 'A'..'Z' ; entrée vide : score 0
    u16 score;
    u8 orient;      // ORIENT_*
    u16 year;       // année de fin de partie
} ScoreEntry;

typedef struct {
    ScoreEntry e[SCORES_COUNT];   // du meilleur au moins bon
} ScoreTable;

typedef struct {
    ScoreTable t[SCORES_TABLES];
} ScoreBoard;

void score_table_reset(ScoreTable *t);
void scores_reset(ScoreBoard *b);

// Rang (0 = premier) qu'obtiendrait ce score, ou -1 s'il n'entre pas.
// Un score nul n'entre jamais ; à égalité, le plus ancien reste devant.
s8 score_table_rank(const ScoreTable *t, u16 score);
#define score_is_high(t, score) (score_table_rank((t), (score)) >= 0)

// Insère l'entrée (initiales normalisées en A-Z) ; renvoie le rang ou -1.
s8 score_table_insert(ScoreTable *t, const char ini[3], u16 score, u8 orient, u16 year);

// Sérialise dans buf (SCORES_BLOB_SIZE octets) ; renvoie SCORES_BLOB_SIZE.
u16 scores_serialize(const ScoreBoard *b, u8 *buf);

// Désérialise. Signature, version ou longueur fausses : tout est réinitialisé.
// Somme fausse ou entrée invalide : seul ce tableau est réinitialisé.
// Renvoie le nombre de tableaux réinitialisés (0 = bloc sain, SCORES_TABLES = tout).
// buf peut être NULL (tout réinitialisé).
u8 scores_deserialize(ScoreBoard *b, const u8 *buf, u16 len);

#endif
