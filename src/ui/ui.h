// Mega Président — couche d'interface (tâches 1.2, 1.3, 1.5).
//
// Seul module autorisé à toucher au VDP (tuiles, plans, palettes).
// Le reste du jeu dessine uniquement via ces fonctions : en V2, un pixel
// artiste remplace les images de res/gfx/ sans modifier le code.
// Contrat graphique : src/ui/README.md.
//
// Coordonnées en tuiles : x 0..39, y 0..27 (écran 320x224).
// Les fenêtres et leurs fonds sont sur le plan B, le texte, les jauges et les
// icônes sur le plan A (le fond des glyphes est transparent).
#ifndef UI_H
#define UI_H

#include <genesis.h>

#define UI_COLS 40
#define UI_ROWS 28
#define UI_TEXT_COLS 38   // colonnes utiles (marge d'une colonne de chaque côté)

// Styles = lignes de palette nommées (res/gfx/pal_ui_*.png).
typedef enum {
    UI_STYLE_TEXT = 0,       // pal_ui_text      : texte blanc, cadres, jauge verte
    UI_STYLE_HIGHLIGHT = 1,  // pal_ui_highlight : sélection, titres (jaune)
    UI_STYLE_DIM = 2,        // pal_ui_dim       : texte atténué, jauge bleue
    UI_STYLE_ALERT = 3       // pal_ui_alert     : alerte (rouge), jauge rouge
} UiStyle;

// Alias pour les jauges
#define UI_GAUGE_POSITIVE UI_STYLE_TEXT
#define UI_GAUGE_GOLD     UI_STYLE_HIGHLIGHT
#define UI_GAUGE_NEUTRAL  UI_STYLE_DIM
#define UI_GAUGE_NEGATIVE UI_STYLE_ALERT

typedef enum {
    UI_ICON_CURSOR = 0,
    UI_ICON_ARROW_UP = 1,
    UI_ICON_ARROW_DOWN = 2,
    UI_ICON_BULLET = 3
} UiIcon;

// --- Base -------------------------------------------------------------------
void ui_init(void);                          // écran 320, tuiles et palettes
void ui_clear(void);                         // efface tout (plans A et B)
void ui_clear_rect(u16 x, u16 y, u16 w, u16 h);  // efface le plan A (texte)

// --- Fenêtres -----------------------------------------------------------------
// Cadre de w x h tuiles (w, h >= 2), bord compris ; l'intérieur est vidé.
void ui_window(u16 x, u16 y, u16 w, u16 h);
void ui_window_erase(u16 x, u16 y, u16 w, u16 h);  // efface cadre et contenu

// --- Texte (chaînes UTF-8) ----------------------------------------------------
// Renvoie le nombre de colonnes écrites.
u16 ui_text_draw(u16 x, u16 y, const char *str, UiStyle style);
// Écrit au plus max_cols colonnes, puis complète avec des espaces jusqu'à
// pad_cols (0 = pas de remplissage) pour effacer un ancien texte.
u16 ui_text_draw_n(u16 x, u16 y, const char *str, u16 max_cols, u16 pad_cols, UiStyle style);
// Centré dans [x, x + w[.
void ui_text_center(u16 x, u16 y, u16 w, const char *str, UiStyle style);
// Nombre de colonnes (glyphes) d'une chaîne UTF-8.
u16 ui_text_len(const char *str);
// Retour à la ligne automatique sur w colonnes (mots entiers ; '\n' force un
// saut ; espace insécable U+00A0 / U+202F respectée). Écrit au plus max_lines
// lignes et renvoie le nombre de lignes écrites.
u16 ui_text_wrap(u16 x, u16 y, u16 w, u16 max_lines, const char *str, UiStyle style);
// Nombre de lignes qu'occuperait le texte sur w colonnes (sans rien écrire).
u16 ui_text_count_lines(u16 w, const char *str);

// Nombres entiers à virgule fixe : (425, 1) -> "42,5" ; (-120, 2) -> "-1,20".
// buf doit contenir au moins 13 octets. Renvoie la longueur.
u16 ui_format_number(char *buf, s32 value, u16 decimals);
u16 ui_number_draw(u16 x, u16 y, s32 value, u16 decimals, UiStyle style);

// --- Jauges et icônes ---------------------------------------------------------
// Jauge horizontale de w tuiles (résolution : 8 pixels par tuile).
void ui_gauge_draw(u16 x, u16 y, u16 w, s16 value, s16 max, UiStyle style);
void ui_icon_draw(u16 x, u16 y, UiIcon icon);

// --- Liste avec curseur -------------------------------------------------------
typedef enum {
    UI_LIST_IDLE = 0,   // rien
    UI_LIST_MOVED,      // le curseur a bougé (redessiner)
    UI_LIST_CHOSEN,     // A ou C sur un élément actif : voir list->sel
    UI_LIST_CANCELLED   // B
} UiListResult;

typedef struct {
    const char *const *items;  // chaînes UTF-8
    u32 disabled;              // bit i = élément i grisé, non sélectionnable (32 max)
    u8 count;
    u8 x, y, w, rows;          // w inclut le curseur (col. 0) et les flèches (dernière col.)
    u8 sel, top;
} UiList;

void ui_list_init(UiList *list, u16 x, u16 y, u16 w, u16 rows,
                  const char *const *items, u16 count);
UiListResult ui_list_update(UiList *list);   // lit la manette (input_repeat)
void ui_list_draw(const UiList *list);

// --- Boîte de message avec choix (modale, non bloquante) ----------------------
// Fenêtre pleine largeur en bas de l'écran : titre (facultatif, NULL),
// texte avec retour à la ligne, puis choix (nchoices = 0 : simple « OK »).
// Les chaînes doivent rester valides tant que la boîte est ouverte.
void ui_msgbox_open(const char *title, const char *text,
                    const char *const *choices, u16 nchoices);
// À appeler à chaque image : -1 tant que le joueur n'a pas choisi, sinon
// l'indice du choix (0 pour « OK »). La boîte reste affichée : ui_msgbox_close.
s16 ui_msgbox_update(void);
bool ui_msgbox_is_open(void);
void ui_msgbox_close(void);   // efface la zone de la boîte (l'écran redessine le reste)

#endif
