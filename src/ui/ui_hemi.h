// Mega Président — hémicycle dynamique (tâche 3.17).
//
// Demi-cercle de 20 x 10 tuiles (160 x 80 px) : un point par siège (Assemblée :
// 577, Sénat : 348), colorés par groupe de gauche (G1) à droite (G5), perchoir et
// tribune au centre. Dessiné sur le PLAN A (le fond de la fenêtre se voit entre
// les sièges), palette dans PAL3 (ligne des illustrations : ne pas afficher de
// vignette en PAL3 en même temps). Tuiles générées à l'exécution, sans tampon en
// RAM, dans la zone VRAM des illustrations (libérée par ui_art_close).
// Géométrie : tools/ui/make_hemi.py -> src/ui/ui_hemi_data.h.
#ifndef UI_HEMI_H_INCLUDED
#define UI_HEMI_H_INCLUDED

#include <genesis.h>

#define UI_HEMI_W 20
#define UI_HEMI_H 10
#define UI_HEMI_GROUPS 5

typedef enum { UI_HEMI_ASSEMBLY = 0, UI_HEMI_SENATE } UiHemiChamber;

// Dessine l'hémicycle en (x, y) tuiles ; seats[g] = sièges du groupe g (somme =
// 577 ou 348, sinon complétée par le dernier groupe). Tous les sièges allumés.
void ui_hemi_draw(u16 x, u16 y, UiHemiChamber ch, const u16 *seats);
// Vote : pour[g] != 0 si le groupe vote pour. Les groupes « contre » s'éteignent ;
// animate : tout s'éteint puis les « pour » s'allument de gauche à droite
// (un groupe toutes les 250 ms ; appeler ui_hemi_update à chaque image).
void ui_hemi_vote(const u8 *pour, bool animate);
// Met un groupe en évidence (clignotement doux) ; 0xFF : aucun.
void ui_hemi_focus(u8 group);
// Animation : à appeler à chaque image tant que l'hémicycle est affiché.
// Renvoie TRUE quand l'animation du vote est terminée.
bool ui_hemi_update(void);

// Jauge de majorité : barre de w tuiles (votes / total, dorée si >= majority,
// rouge sinon) avec un repère « ▼ » sur la majorité, sur 2 rangées (y : repère,
// y + 1 : barre).
void ui_hemi_majority(u16 x, u16 y, u16 w, u16 votes, u16 total, u16 majority);

#endif
