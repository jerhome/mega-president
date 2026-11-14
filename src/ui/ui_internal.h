// Mega Président — détails internes de la couche d'interface (src/ui seulement).
#ifndef UI_INTERNAL_H
#define UI_INTERNAL_H

#include "ui/ui.h"

// Premières tuiles en VRAM de chaque jeu de tuiles (fixées par ui_init).
extern u16 ui_tile_font, ui_tile_frame, ui_tile_gauge, ui_tile_icons;

#define UI_ATTR(style, tile) TILE_ATTR_FULL((style), FALSE, FALSE, FALSE, (tile))

// Cadre : tuiles dans l'ordre de ui_frame.png (3x3).
enum { FR_TL, FR_T, FR_TR, FR_L, FR_C, FR_R, FR_BL, FR_B, FR_BR };

#endif
