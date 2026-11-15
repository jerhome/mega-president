// Généré par tools/ui/make_ui.py : ne pas éditer (voir docs/CHARTE_V3.md).
#ifndef UI_TILES_H
#define UI_TILES_H

// Tuiles de décor (ui_frame.png, plan B), dans l'ordre du jeu de tuiles.
enum {
    UI_T_BG0 = 0,
    UI_T_BG1 = 1,
    UI_T_BG2 = 2,
    UI_T_BG3 = 3,
    UI_T_WIN_TL = 4,
    UI_T_WIN_T = 5,
    UI_T_WIN_TR = 6,
    UI_T_WIN_L = 7,
    UI_T_WIN_C = 8,
    UI_T_WIN_R = 9,
    UI_T_WIN_BL = 10,
    UI_T_WIN_B = 11,
    UI_T_WIN_BR = 12,
    UI_T_HD_TL = 13,
    UI_T_HD_T = 14,
    UI_T_HD_TR = 15,
    UI_T_HD_L = 16,
    UI_T_HD_M = 17,
    UI_T_HD_R = 18,
    UI_T_HD_SL = 19,
    UI_T_HD_S = 20,
    UI_T_HD_SR = 21,
    UI_T_BN_BL = 22,
    UI_T_BN_B = 23,
    UI_T_BN_BR = 24,
    UI_T_SEL_L = 25,
    UI_T_SEL_M = 26,
    UI_T_SEL_R = 27,
    UI_T_SEP_L = 28,
    UI_T_SEP_M = 29,
    UI_T_SEP_R = 30,
    UI_T_PF_TL = 31,
    UI_T_PF_T = 32,
    UI_T_PF_TR = 33,
    UI_T_PF_L = 34,
    UI_T_PF_R = 35,
    UI_T_PF_BL = 36,
    UI_T_PF_B = 37,
    UI_T_PF_BR = 38,
    UI_T_PF_C = 39,
    UI_T_COUNT = 40
};

// Ligne de palette de chaque tuile de décor (0 : PAL0, 1 : PAL1).
static const u8 ui_frame_pal[UI_T_COUNT] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0
};

// Jauges (ui_gauge.png) : jeu (0 : A, 1 : B) x partie (0 gauche, 1 milieu, 2 droite) x 9.
#define UI_GAUGE_TILE(set, part, k) ((set) * 27 + (part) * 9 + (k))

// Icônes (ui_icons.png) : ligne de palette de chaque icône.
#define UI_ICON_COUNT 37
static const u8 ui_icon_pal[UI_ICON_COUNT] = {
    1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0
};

#endif
