// Mega Président — réglages de l'écran Options (tâche 3.19, docs/SPEC_OPTIONS.md § 5-6).
//
// Logique PORTABLE (sans SGDK) : compile sur PC (tests) et sur Megadrive.
// L'accès à la SRAM est dans sram.c ; l'application aux modules (son, affichage)
// dans settings_apply.c (console seulement). Les réglages appartiennent à la
// console, pas à la partie : ils ne sont pas dans la sauvegarde de partie.
//
// Bloc sérialisé (SETTINGS_BLOB_SIZE octets, SRAM 224 à 239) :
//   0-1   signature 'O' 'P'
//   2     version du format (SETTINGS_VERSION)
//   3     nombre de valeurs stockées (SET_COUNT)
//   4-11  valeurs, une par réglage dans l'ordre de SettingId (inutilisées : 0)
//   12-13 réservés (0)
//   14-15 somme des octets 0 à 13, XOR 0x5AA5 (u16 gros-boutiste)
#ifndef SAVE_SETTINGS_H
#define SAVE_SETTINGS_H

#include "sim/sim_types.h"

typedef enum {
    SET_MUSIC = 0,  // 0 coupée, 1..4 = 25/50/75/100 %   (défaut 4)
    SET_SFX,        // 0 tous, 1 essentiels, 2 aucun      (défaut 0)
    SET_VOICE,      // 0 non, 1 oui                       (défaut 1)
    SET_FACES,      // 0 figés, 1 animés                  (défaut 1)
    SET_FADES,      // 0 douces, 1 rapides, 2 aucune      (défaut 0)
    SET_COLORS,     // 0 standard, 1 daltonisme           (défaut 0)
    SET_COUNT
} SettingId;

#define SETTINGS_VERSION    1
#define SETTINGS_BLOB_SIZE  16
#define SETTINGS_MAX_VALUES 8     // octets 4 à 11
#define SETTINGS_CHECK_XOR  0x5AA5

u8   settings_get(SettingId id);
void settings_set(SettingId id, u8 v);     // borne la valeur, marque « modifié » si elle change
u8   settings_count(SettingId id);         // nombre de valeurs possibles
u8   settings_default(SettingId id);
void settings_reset(void);                 // tous les défauts (marque « modifié » si besoin)
u8   settings_all_default(void);           // 1 si tous les réglages sont à leur défaut
u8   settings_dirty(void);
void settings_clear_dirty(void);
u16  settings_serialize(u8 *buf);          // écrit SETTINGS_BLOB_SIZE octets
// 0 bloc sain, 1 valeurs corrigées, 2 tout réinitialisé ; buf NULL : défauts.
// Ne marque pas « modifié ».
u8   settings_deserialize(const u8 *buf, u16 len);

#endif
