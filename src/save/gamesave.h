// Mega Président — sauvegarde d'une partie en cours (tâche V2 2.13, V4.1 I2).
//
// Logique PORTABLE (sans SGDK). Le bloc à sauvegarder est opaque : copie brute
// des octets de la struct Game. Sur 68000 (big-endian, alignement 2) on relit
// sur la même machine avec le même binaire, donc la copie brute convient.
// Si le jeu est mis à jour (contenu ajouté, disposition modifiée), l'empreinte
// ou la taille change : la sauvegarde est refusée proprement.
//
// Format du bloc (GAMESAVE_HEADER + size octets) :
//   0..3   signature « MPG1 »
//   4      version du format (GAMESAVE_VERSION)
//   5      réservé (0)
//   6..7   taille des données (u16 BE)
//   8..9   Fletcher-16 de l'en-tête (octets 4..7 et 10..13) et des données (u16 BE)
//   10..11 empreinte du contenu et de la disposition (game_fingerprint, u16 BE)
//   12..13 compteur de génération (u16 BE, comparé modulo 65536)
//   14..   données
//
// Deux emplacements alternés : on écrit toujours dans celui qui n'est PAS le plus
// récent valide ; une coupure en cours d'écriture laisse l'autre intact.
#ifndef SAVE_GAMESAVE_H
#define SAVE_GAMESAVE_H

#include "sim/sim_types.h"

#define GAMESAVE_VERSION 3   // V4.1 : empreinte, génération, deux emplacements
#define GAMESAVE_HEADER  14

// Écrit l'en-tête + les données dans out (GAMESAVE_HEADER + size octets).
// Renvoie la longueur totale écrite.
u16 gamesave_pack(const void *src, u16 size, u16 fp, u16 gen, u8 *out);
// Vérifie signature, version, taille, empreinte et somme ; si valide copie les
// données dans dst (dst peut être NULL : simple validation) et renvoie 1, sinon 0
// sans toucher dst.
u8 gamesave_unpack(void *dst, u16 size, u16 fp, const u8 *in, u16 len);
// 1 si le bloc porte la signature du jeu mais n'est PAS lisible par cette version
// (version, taille, empreinte ou somme différentes) : une sauvegarde « ancienne » ignorée.
u8 gamesave_is_stale(const u8 *in, u16 len, u16 size, u16 fp);
// Génération stockée dans un bloc (sans validation).
u16 gamesave_gen(const u8 *in);
// Emplacement valide le plus récent parmi a et b : 0, 1, ou -1 si aucun.
s8 gamesave_pick(const u8 *a, const u8 *b, u16 len, u16 size, u16 fp);
// Fletcher-16 (modulo différé), exposé pour les tests.
u16 gamesave_fletcher16(const u8 *p, u16 n);

#endif
