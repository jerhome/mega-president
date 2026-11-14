// Mega Président — sauvegarde d'une partie en cours (tâche V2 2.13).
//
// Logique PORTABLE (sans SGDK). Le bloc à sauvegarder est opaque : copie brute
// des octets de la struct Game. Sur 68000 (big-endian, alignement 2) on relit
// sur la même machine avec le même binaire, donc la copie brute convient.
// Si le jeu est mis à jour et que sizeof(Game) change, la taille stockée ne
// correspond plus : la sauvegarde est refusée proprement.
//
// Format du bloc (GAMESAVE_HEADER + size octets) :
//   0..3  signature « MPG1 »
//   4     version du format (GAMESAVE_VERSION)
//   5     réservé (0)
//   6..7  taille des données (u16 BE)
//   8..9  somme de contrôle Fletcher-16 des données (u16 BE)
//   10..  données
#ifndef SAVE_GAMESAVE_H
#define SAVE_GAMESAVE_H

#include "sim/sim_types.h"

#define GAMESAVE_VERSION 1
#define GAMESAVE_HEADER  10

// Écrit l'en-tête + les données dans out (GAMESAVE_HEADER + size octets).
// Renvoie la longueur totale écrite.
u16 gamesave_pack(const void *src, u16 size, u8 *out);
// Vérifie signature, version, taille (== size) et somme ; si valide copie les
// données dans dst (dst peut être NULL : simple validation) et renvoie 1, sinon renvoie 0 sans toucher dst.
u8 gamesave_unpack(void *dst, u16 size, const u8 *in, u16 len);

#endif
