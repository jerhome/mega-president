// Mega Président — couche d'interface (tâches 1.2, 1.3, 1.5 ; refonte V3 3.01-3.04).
//
// Seul module autorisé à toucher au VDP (tuiles, plans, palettes).
// Le reste du jeu dessine uniquement via ces fonctions.
// Contrat graphique : src/ui/README.md ; charte V3 : docs/CHARTE_V3.md.
//
// Coordonnées en tuiles : x 0..39, y 0..27 (écran 320x224).
// Plan B : fond d'écran, fenêtres, bandeaux, barres de sélection, illustrations.
// Plan A : texte, jauges, icônes (fond transparent).
// Palettes : l'interface n'utilise que PAL0 et PAL1 ; PAL2 et PAL3 sont
// réservées aux illustrations (ui_art : portrait en PAL2, vignette en PAL3).
#ifndef UI_H
#define UI_H

#include <genesis.h>

#define UI_COLS 40
#define UI_ROWS 28
#define UI_TEXT_COLS 38   // colonnes utiles (marge d'une colonne de chaque côté)
// Tampon d'une ligne de texte : 38 colonnes en UTF-8 (accents sur 2 octets, ✓ sur 3).
// Ne jamais construire une ligne dans un tableau plus petit (plantage V2 de la 4e année).
#define UI_LINE_BUF 80

// Styles de texte. bit 0 = ligne de palette (PAL0 / PAL1), bit 1 = jeu d'encre
// de la police (A / B) : 4 styles avec 2 palettes et 2 copies de la police.
typedef enum {
    UI_STYLE_TEXT = 0,       // PAL0, encre A : texte courant, valeurs (ivoire)
    UI_STYLE_HIGHLIGHT = 1,  // PAL1, encre A : titres, sélection, valeurs clés (or)
    UI_STYLE_DIM = 2,        // PAL0, encre B : libellés, aides, éléments grisés (gris bleuté)
    UI_STYLE_ALERT = 3       // PAL1, encre B : alerte (rouge)
} UiStyle;

// Jauges : même codage (palette, jeu de couleurs).
#define UI_GAUGE_POSITIVE UI_STYLE_TEXT       // verte
#define UI_GAUGE_GOLD     UI_STYLE_HIGHLIGHT  // dorée
#define UI_GAUGE_NEUTRAL  UI_STYLE_DIM        // bleue
#define UI_GAUGE_NEGATIVE UI_STYLE_ALERT      // rouge

// Icônes 8x8 (ordre de ICONS dans tools/ui/make_ui.py). Chacune a sa palette.
typedef enum {
    UI_ICON_CURSOR = 0,      // curseur animé (or)
    UI_ICON_ARROW_UP,        // défilement
    UI_ICON_ARROW_DOWN,
    UI_ICON_BULLET,          // puce (losange doré)
    UI_ICON_POPULARITY,      // cœur
    UI_ICON_GROWTH,          // courbe montante
    UI_ICON_UNEMPLOYMENT,    // mallette
    UI_ICON_DEBT,            // pièce
    UI_ICON_SOCIAL,          // deux personnes
    UI_ICON_ECONOMY,         // usine
    UI_ICON_TECHNOLOGY,      // puce électronique
    UI_ICON_ECOLOGY,         // feuille
    UI_ICON_SECURITY,        // bouclier
    UI_ICON_INTERNATIONAL,   // globe
    UI_ICON_TENSION,         // éclair
    UI_ICON_SCANDAL,         // point d'exclamation
    UI_ICON_DEFIANCE,        // visage mécontent
    UI_ICON_CP,              // étoile (capital politique)
    UI_ICON_ASSEMBLY,        // hémicycle
    UI_ICON_CALENDAR,        // calendrier
    UI_ICON_BTN_A,           // boutons de la manette
    UI_ICON_BTN_B,
    UI_ICON_BTN_C,
    UI_ICON_BTN_START,
    UI_ICON_CHECK,           // coche verte
    UI_ICON_CROSS,           // croix rouge
    UI_ICON_PERSON,          // personnage
    UI_ICON_NEWS,            // journal
    UI_ICON_UP_GOOD,         // hausse favorable (verte)
    UI_ICON_DOWN_BAD,        // baisse défavorable (rouge)
    UI_ICON_UP_BAD,          // hausse défavorable (rouge)
    UI_ICON_DOWN_GOOD,       // baisse favorable (verte)
    UI_ICON_LAST
} UiIcon;

// --- Base -------------------------------------------------------------------
void ui_init(void);                          // écran 320, tuiles et palettes
// Efface tout : plan A vidé, plan B rempli du fond d'écran texturé. Demande
// aussi une « apparition douce » (fondu bref) à la fin de l'image.
void ui_clear(void);
void ui_clear_rect(u16 x, u16 y, u16 w, u16 h);  // efface le plan A (texte)

// --- Fenêtres -----------------------------------------------------------------
// Cadre arrondi avec ombre portée, de w x h tuiles (w, h >= 2), bord compris ;
// l'intérieur (fond texturé) est vidé.
void ui_window(u16 x, u16 y, u16 w, u16 h);
void ui_window_erase(u16 x, u16 y, u16 w, u16 h);  // efface cadre et contenu (fond d'écran)
// Fenêtre à bandeau de titre : bandeau sur les rangées y à y+2 (titre en or sur
// la rangée y+1, à gauche si left, sinon centré), contenu à partir de y+3.
// h >= 4 ; title peut être NULL (bandeau vide, complété par l'écran).
void ui_window_title(u16 x, u16 y, u16 w, u16 h, const char *title, bool left);
// Bandeau seul (3 rangées) : titre sur la rangée y+1, à gauche (x+2) si left.
void ui_banner(u16 x, u16 y, u16 w, const char *title, bool left);
// Filet gravé horizontal (plan B, dans une fenêtre) : une rangée sans texte.
void ui_separator(u16 x, u16 y, u16 w);
// Médaillon doré de w x h tuiles (cadre d'une tuile) : l'illustration se place
// à l'intérieur (x+1, y+1, w-2, h-2), sur le plan B.
void ui_medallion(u16 x, u16 y, u16 w, u16 h);
// Barre de sélection (plan B, dans une fenêtre) sur w tuiles ; ui_select_bar_clear
// remet le fond de la fenêtre.
void ui_select_bar(u16 x, u16 y, u16 w);
void ui_select_bar_clear(u16 x, u16 y, u16 w);
// Aide des boutons, centrée sur [x, x + w[ : « A : valider   B : retour ».
// « A : », « B : », « C : » et « START : » deviennent des icônes de bouton.
void ui_footer(u16 x, u16 y, u16 w, const char *hints);

// --- Texte (chaînes UTF-8) ----------------------------------------------------
// Renvoie le nombre de colonnes écrites.
u16 ui_text_draw(u16 x, u16 y, const char *str, UiStyle style);
// Écrit au plus max_cols colonnes, puis complète avec des espaces jusqu'à
// pad_cols (0 = pas de remplissage) pour effacer un ancien texte.
u16 ui_text_draw_n(u16 x, u16 y, const char *str, u16 max_cols, u16 pad_cols, UiStyle style);
// Centré dans [x, x + w[.
void ui_text_center(u16 x, u16 y, u16 w, const char *str, UiStyle style);
// Aligné à droite : dernière colonne en x_end - 1.
void ui_text_right(u16 x_end, u16 y, const char *str, UiStyle style);
// Nombre de colonnes (glyphes) d'une chaîne UTF-8.
u16 ui_text_len(const char *str);
// Retour à la ligne automatique sur w colonnes (mots entiers ; '\n' force un
// saut ; espace insécable U+00A0 / U+202F respectée). Écrit au plus max_lines
// lignes et renvoie le nombre de lignes écrites.
u16 ui_text_wrap(u16 x, u16 y, u16 w, u16 max_lines, const char *str, UiStyle style);
// Nombre de lignes qu'occuperait le texte sur w colonnes (sans rien écrire).
u16 ui_text_count_lines(u16 w, const char *str);
// Écrit UNE ligne (coupée sur w colonnes) et renvoie la suite du texte, ou NULL
// quand tout est écrit : pour faire couler un texte autour d'une illustration.
const char *ui_text_wrap_line(u16 x, u16 y, u16 w, const char *str, UiStyle style);

// Nombres entiers à virgule fixe : (425, 1) -> "42,5" ; (-120, 2) -> "-1,20".
// buf doit contenir au moins 13 octets. Renvoie la longueur.
u16 ui_format_number(char *buf, s32 value, u16 decimals);
u16 ui_number_draw(u16 x, u16 y, s32 value, u16 decimals, UiStyle style);

// --- Jauges et icônes ---------------------------------------------------------
// Jauge horizontale à reflet de w tuiles (bouts arrondis ; 8 pixels par tuile).
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
    u32 disabled;              // bit i = élément i grisé, non sélectionnable (éléments 0-31)
    u32 disabled_hi;           // idem pour les éléments 32-63 (bit i - 32)
    u8 count;
    u8 x, y, w, rows;          // w inclut le curseur (col. 0) et les flèches (dernière col.)
    u8 sel, top;
    u8 flags;                  // UI_LIST_NOBAR : pas de barre de sélection
    u8 bar_sel;                // ligne qui porte la barre (interne, 0xFF : aucune)
} UiList;

#define UI_LIST_NOBAR 1        // liste hors d'une fenêtre : curseur seul

void ui_list_init(UiList *list, u16 x, u16 y, u16 w, u16 rows,
                  const char *const *items, u16 count);
UiListResult ui_list_update(UiList *list);   // lit la manette (input_repeat)
void ui_list_draw(UiList *list);
// Grise (on = 1) ou réactive l'élément i (0-63).
void ui_list_set_disabled(UiList *list, u16 i, bool on);
// 1 si l'élément i est sélectionnable.
bool ui_list_enabled(const UiList *list, u16 i);

// --- Boîte de message avec choix (modale, non bloquante) ----------------------
// Fenêtre pleine largeur en bas de l'écran : titre (facultatif, NULL),
// texte avec retour à la ligne, puis choix (nchoices = 0 : simple « OK »).
// Elle s'ouvre en se dépliant (≈ 60 ms). Les chaînes doivent rester valides
// tant que la boîte est ouverte.
void ui_msgbox_open(const char *title, const char *text,
                    const char *const *choices, u16 nchoices);
// À appeler à chaque image : -1 tant que le joueur n'a pas choisi, sinon
// l'indice du choix (0 pour « OK »). La boîte reste affichée : ui_msgbox_close.
s16 ui_msgbox_update(void);
bool ui_msgbox_is_open(void);
void ui_msgbox_close(void);   // efface la zone de la boîte (l'écran redessine le reste)

// --- Animations ---------------------------------------------------------------
// Durées en millisecondes (identiques en PAL et en NTSC).
#define UI_FADE_OUT_MS   120   // fondu au noir avant un changement d'écran
#define UI_FADE_IN_MS    220   // apparition du nouvel écran
#define UI_SOFT_MS       150   // apparition douce après un redessin complet (ui_clear)

// Annule l'apparition douce demandée par ui_clear (redessin sans effet).
void ui_soft_cancel(void);

#endif
