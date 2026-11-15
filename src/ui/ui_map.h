// Mega Président — carte de France des 13 régions (tâche 2.01).
//
// La carte occupe 20 x 20 tuiles du plan B et UNE ligne de palette entière (PAL2) :
// un indice de couleur par région, donc colorer une région = changer une couleur
// de palette, sans redessiner de tuile. Les 4 palettes sont déjà prises par les
// styles de ui.h : pendant l'affichage de la carte, PAL2 (style UI_STYLE_DIM)
// contient la palette de la carte. Les écrans qui affichent la carte
// n'emploient donc pas UI_STYLE_DIM ni UI_GAUGE_NEUTRAL ; ui_map_close()
// restaure la palette normale. Contrat : src/ui/README.md, section « Carte ».
#ifndef UI_MAP_H
#define UI_MAP_H

#include <genesis.h>

#define UI_MAP_TILES 20   // carte de 20 x 20 tuiles (160 x 160 pixels)

// Même ordre que les indices 1 à 13 de pal_map (valeur = indice - 1).
typedef enum {
    REGION_HAUTS_DE_FRANCE = 0,
    REGION_NORMANDIE,
    REGION_ILE_DE_FRANCE,
    REGION_GRAND_EST,
    REGION_BRETAGNE,
    REGION_PAYS_DE_LA_LOIRE,
    REGION_CENTRE_VAL_DE_LOIRE,
    REGION_BOURGOGNE_FRANCHE_COMTE,
    REGION_NOUVELLE_AQUITAINE,
    REGION_AUVERGNE_RHONE_ALPES,
    REGION_OCCITANIE,
    REGION_PACA,
    REGION_CORSE,
    REGION_COUNT          // 13
} Region;

#define REGION_NONE 0xFF

// Dessine la carte (coin haut gauche en tuiles, plan B) et charge sa palette
// dans PAL2. Toutes les régions reprennent leur couleur neutre.
void ui_map_draw(u16 x, u16 y);
// Efface la carte et restaure la palette PAL2 normale (style UI_STYLE_DIM).
void ui_map_close(void);
// Colore une région selon value dans [min, max] : rouge (tiers bas), jaune, vert (tiers haut).
void ui_map_set_region_level(u8 region, s16 value, s16 min, s16 max);
// Teintes de la carte (0x0BGR) : faible, moyenne, forte. Un tiers de l'écart
// [min, max] pour chacune (carte : moyenne nationale ± 10 points).
#define UI_MAP_LOW  ((2 << 5) | (7 << 1) | (1 << 9))   // rouge (7,2,1)
#define UI_MAP_MID  ((6 << 5) | (7 << 1) | (1 << 9))   // jaune (7,6,1)
#define UI_MAP_HIGH ((6 << 5) | (1 << 1) | (1 << 9))   // vert (1,6,1)
// Daltonisme (Options, 3.19) : orangé et bleu, lisibles avec un rouge-vert confondu ;
// le bleu est celui de UI_GAUGE_NEUTRAL (légende de la carte), le jaune ne change pas.
#define UI_MAP_LOW_CB  ((2 << 5) | (7 << 1) | (0 << 9))  // orangé (7,2,0)
#define UI_MAP_HIGH_CB ((3 << 5) | (1 << 1) | (7 << 9))  // bleu (1,3,7)
// Couleur neutre (aucune donnée) pour une région.
void ui_map_clear_region(u8 region);
// Région mise en évidence par un clignotement blanc (REGION_NONE : aucune).
// ui_map_update() doit être appelé à chaque image pour animer le clignotement.
void ui_map_highlight(u8 region);
void ui_map_update(void);
// Daltonisme : orangé (tiers bas), jaune, bleu (tiers haut) au lieu de rouge, jaune, vert.
// À appeler avant ui_map_set_region_level (les régions déjà colorées ne changent pas).
void ui_map_set_colorblind(bool on);
// Nom en UTF-8 avec accents ; chaîne vide si region est invalide.
const char *ui_map_region_name(u8 region);

#endif
