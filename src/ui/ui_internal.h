// Mega Président — détails internes de la couche d'interface (src/ui seulement).
#ifndef UI_INTERNAL_H
#define UI_INTERNAL_H

#include "ui/ui.h"
#include "ui/ui_tiles.h"

// Premières tuiles en VRAM de chaque jeu de tuiles (fixées par ui_init).
// Les icônes sont chargées en DERNIER : ui_tile_icons + ts_ui_icons.numTile
// est la première tuile libre (illustrations et carte, voir ui_art.c / ui_map.c).
extern u16 ui_tile_font, ui_tile_frame, ui_tile_gauge, ui_tile_icons;

#define UI_ATTR(pal, tile) TILE_ATTR_FULL((pal), FALSE, FALSE, FALSE, (tile))
// Tuile de décor (plan B) avec sa ligne de palette.
#define UI_FRAME_ATTR(t) UI_ATTR(ui_frame_pal[(t)], ui_tile_frame + (t))
// Ligne de palette et première tuile de police d'un style.
#define UI_STYLE_PAL(style)  ((style) & 1)
#define UI_FONT_BASE(style)  (ui_tile_font + (((style) >> 1) & 1) * 128)

// Fond d'écran (tuile de la bande) à la rangée y.
u16 ui_bg_attr(u16 y);
// Remet le fond d'écran sur un rectangle du plan B.
void ui_bg_rect(u16 x, u16 y, u16 w, u16 h);

// Boucle des écrans (screen.c) : à appeler à chaque image, juste avant la synchro.
void ui_frame_end(void);
// Changement d'écran : fondu au noir (non bloquant), puis apparition.
void ui_fade_out_start(void);
bool ui_fade_busy(void);
// Animations décoratives permises (aucun fondu en cours ni à venir) : visages animés.
bool ui_anim_allowed(void);
// Écran noir (affichage coupé) pendant init/dessin du nouvel écran, puis fondu d'apparition.
void ui_transition_hide(void);
void ui_transition_show(void);
// Palettes de l'interface rechargées dans les 4 lignes (après un fondu au noir).
void ui_palettes_reset(void);
// Écrit des couleurs de la CRAM sans attendre la file DMA, et met à jour la cible du fondu
// en cours (sinon sa fin écrase la couleur). À préférer à PAL_setColors pour toute
// couleur modifiée après le dessin (hémicycle, carte).
void ui_pal_set(u16 index, const u16 *colors, u16 n);

#endif
