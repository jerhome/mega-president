#ifndef STRBUF_H
#define STRBUF_H

// Tampon de texte borné, portable (PC et 68000, sans SGDK).
// Remplace les strcat sans borne : un dépassement coupe le texte sur une
// frontière UTF-8 (jamais au milieu d'un caractère) et positionne `truncated`.
// Interdits : float, double, malloc, stdio.

#include "sim/sim_types.h"

typedef struct {
    char *buf;       // zone de travail, toujours terminée par 0
    u16   cap;       // taille de la zone, terminateur compris
    u16   len;       // octets utilisés (hors terminateur)
    u8    truncated; // 1 si un ajout a été coupé
} StrBuf;

// Taille d'une ligne d'écran (égale à UI_LINE_BUF dans ui.h).
#define SB_LINE_SIZE 80

// Initialise sb sur buf[cap] ; buf devient la chaîne vide. cap = 0 toléré.
void sb_init(StrBuf *sb, char *buf, u16 cap);

// Ajoute une chaîne (NULL toléré), coupée sur une frontière UTF-8 si besoin.
void sb_str(StrBuf *sb, const char *s);

// Ajoute un entier avec `decimals` décimales à virgule française
// (1234, 2 -> « 12,34 ») ; decimals plafonné à 9 ; négatif avec « - ».
void sb_num(StrBuf *sb, s32 value, u16 decimals);

// Comme sb_num, mais « + » devant les valeurs strictement positives.
void sb_signed(StrBuf *sb, s32 value, u16 decimals);

// Ajoute « avant → après » (nombres à `decimals` décimales).
void sb_arrow(StrBuf *sb, s32 before, s32 after, u16 decimals);

// Déclare un StrBuf `name` sur un tampon local de SB_LINE_SIZE octets.
#define SB_LINE(name) \
    char name##_mem[SB_LINE_SIZE]; \
    StrBuf name; \
    sb_init(&name, name##_mem, SB_LINE_SIZE)

#endif
