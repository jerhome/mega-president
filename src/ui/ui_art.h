// Mega Président — illustrations en pixel art (tâche 2.02, V2).
//
// Seul module à toucher VDP/PAL pour les illustrations. Une illustration est
// une IMAGE SGDK (res/gfx_art.res, générée par tools/art/make_art.py) : ses
// tuiles sont chargées en VRAM juste après celles de ui.c (même zone que la
// carte de France : carte et illustrations ne sont jamais affichées ensemble),
// sa palette est chargée dans une ligne de palette EMPRUNTÉE (PAL2 par défaut,
// celle du style UI_STYLE_DIM) et elle est dessinée sur le plan B.
//
// Règles d'emploi (contrat : src/ui/README.md, section « Illustrations ») :
// - Plusieurs illustrations peuvent être affichées ensemble : leurs tuiles
//   s'empilent en VRAM. Sur une même ligne de palette, elles doivent partager
//   la même palette : c'est le cas de tous les portraits (pal commune).
//   Pour afficher une vignette ET un portrait, utiliser deux lignes
//   (ui_art_draw_pal, ex. PAL2 pour le portrait et PAL3 pour la vignette).
// - Tant qu'une ligne est empruntée, ne pas utiliser le style correspondant
//   (PAL2 : UI_STYLE_DIM / UI_GAUGE_NEUTRAL ; PAL3 : UI_STYLE_ALERT ; PAL1 : HIGHLIGHT).
// - Appeler ui_art_close() en quittant l'écran (et avant ui_map_draw) :
//   restaure les palettes empruntées et libère la VRAM.
// - Le texte (plan A) peut s'écrire par-dessus une illustration.
#ifndef UI_ART_H
#define UI_ART_H

#include <genesis.h>

// Ordre = IMAGE de res/gfx_art.res (tools/art/make_art.py).
typedef enum {
    // Écran titre : 40 x 16 tuiles (320 x 128). Les 3 premières lignes de
    // tuiles sont un ciel bleu uni, réservé au titre écrit avec la police.
    ART_TITLE = 0,

    // Portraits : 7 x 7 tuiles (56 x 56), palette commune.
    ART_PRES_GAUCHE,        // le président, ordre ORIENT_* (sim_state.h)
    ART_PRES_CENTRE,
    ART_PRES_DROITE,
    ART_PM_TECHNOCRATE,     // Pierre Delaunay, ordre PM_* (ids_v1.h)
    ART_PM_RASSEMBLEUR,     // Marianne Deschamps
    ART_PM_REFORMATEUR,     // Julien Maréchal
    ART_COHAB_GAUCHE,       // Sophie Carrel, PM d'opposition, ordre ORIENT_*
    ART_COHAB_CENTRE,       // Bernard Lacombe
    ART_COHAB_DROITE,       // Philippe de Montclar
    ART_CAND_ROUSSEL,       // candidats, ordre CAND_* (election.h)
    ART_CAND_VASSEUR,
    ART_CAND_MARCHETTI,
    ART_CAND_VALMONT,
    ART_CAND_LENOIR,

    // Vignettes d'événements : 8 x 6 tuiles (64 x 48), une palette chacune.
    ART_EVT_GREVE,          // grève, manifestation, colère sociale
    ART_EVT_CANICULE,       // canicule, sécheresse
    ART_EVT_INONDATION,     // inondation, tempête, intempéries
    ART_EVT_SCANDALE,       // affaire, scandale, presse, justice
    ART_EVT_SOMMET,         // sommet international, diplomatie
    ART_EVT_BOURSE,         // économie, crise financière, notation, inflation
    ART_EVT_SANTE,          // santé, hôpital, épidémie
    ART_EVT_POLLUTION,      // écologie, pollution, marée noire, climat
    ART_EVT_SECURITE,       // sécurité, émeutes, police, espionnage
    ART_EVT_NUMERIQUE,      // numérique, cyberattaque, désinformation
    ART_EVT_EDUCATION,      // éducation, école, lycéens, enseignants
    ART_EVT_ENERGIE,        // énergie, panne, prix de l'électricité
    ART_EVT_AGRICULTURE,    // agriculture, crise du lait, ruralité
    ART_EVT_INDUSTRIE,      // usine fermée, emploi, accident industriel
    ART_EVT_FETE,           // culture, sport, festival, prix Nobel, bonne nouvelle

    // Décors
    ART_DECOR_HEMICYCLE,    // 20 x 9 tuiles (160 x 72) : vote à l'Assemblée
    ART_DECOR_URNE,         // 8 x 8 tuiles (64 x 64) : élection, référendum
    ART_DECOR_JOURNAL,      // 38 x 5 tuiles (304 x 40) : bandeau « une » du journal (rapport)

    ART_COUNT
} ArtId;

#define ART_NONE ART_COUNT

#define ART_PRES(orient)   ((ArtId)(ART_PRES_GAUCHE + (orient)))     // ORIENT_*
#define ART_PM(profile)    ((ArtId)(ART_PM_TECHNOCRATE + (profile))) // PM_*
#define ART_COHAB(camp)    ((ArtId)(ART_COHAB_GAUCHE + (camp)))      // ORIENT_*
#define ART_CAND(cand)     ((ArtId)(ART_CAND_ROUSSEL + (cand)))      // CAND_* (sauf CAND_JOUEUR)

#define ART_FIRST_PORTRAIT ART_PRES_GAUCHE
#define ART_FIRST_EVENT    ART_EVT_GREVE
#define ART_EVENT_COUNT    (ART_EVT_FETE - ART_EVT_GREVE + 1)

// Taille en tuiles (0 x 0 si id invalide).
void ui_art_size(ArtId id, u16 *w, u16 *h);

// Dessine l'illustration (coin haut gauche x, y en tuiles) sur le plan B, avec
// sa palette dans PAL2. Renvoie FALSE (rien dessiné) si id invalide ou si la
// VRAM réservée aux illustrations est pleine (appeler ui_art_close d'abord).
bool ui_art_draw(ArtId id, u16 x, u16 y);
// Idem dans la ligne de palette pal (PAL1, PAL2 ou PAL3 ; PAL0 est refusée).
bool ui_art_draw_pal(ArtId id, u16 x, u16 y, u16 pal);
// Efface (plan B) le rectangle occupé par l'illustration placée en x, y.
void ui_art_erase(ArtId id, u16 x, u16 y);
// Restaure les palettes empruntées (styles normaux) et libère la VRAM des
// illustrations. Les tuiles restent à l'écran : effacer avant (ui_clear,
// ui_art_erase) pour éviter un affichage aux mauvaises couleurs.
void ui_art_close(void);

#endif
