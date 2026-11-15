// Mega Président — illustrations en pixel art (tâche 2.02, V2).
//
// Seul module à toucher VDP/PAL pour les illustrations. Une illustration est
// une IMAGE SGDK (res/gfx_art.res, générée par tools/art/make_art.py) : ses
// tuiles sont chargées en VRAM juste après celles de ui.c (même zone que la
// carte de France : carte et illustrations ne sont jamais affichées ensemble),
// sa palette est chargée dans PAL2 (par défaut) ou PAL3, lignes réservées aux
// illustrations en V3 (l'interface n'utilise que PAL0 et PAL1), et elle est
// dessinée sur le plan B.
//
// Règles d'emploi (contrat : src/ui/README.md, section « Illustrations ») :
// - Plusieurs illustrations peuvent être affichées ensemble : leurs tuiles
//   s'empilent en VRAM. Sur une même ligne de palette, elles doivent partager
//   la même palette.
//   Chaque gueule a sa palette : deux illustrations = deux lignes (portrait
//   principal en PAL2, vignette ou seconde gueule en PAL3).
// - Tous les styles de texte restent utilisables (ils n'utilisent que PAL0/PAL1).
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

#define ART_FIRST_EVENT    ART_EVT_GREVE
#define ART_EVENT_COUNT    (ART_EVT_FETE - ART_EVT_GREVE + 1)

// Taille en tuiles (0 x 0 si id invalide).
void ui_art_size(ArtId id, u16 *w, u16 *h);

// Dessine l'illustration (coin haut gauche x, y en tuiles) sur le plan B, avec
// sa palette dans PAL2. Renvoie FALSE (rien dessiné) si id invalide ou si la
// VRAM réservée aux illustrations est pleine (appeler ui_art_close d'abord).
bool ui_art_draw(ArtId id, u16 x, u16 y);
// Idem dans la ligne de palette pal (PAL2 ou PAL3 ; toute autre valeur -> PAL2).
bool ui_art_draw_pal(ArtId id, u16 x, u16 y, u16 pal);
// Efface (plan B) le rectangle occupé par l'illustration placée en x, y.
void ui_art_erase(ArtId id, u16 x, u16 y);
// Réserve ntiles tuiles dans la zone des illustrations (libérées par ui_art_close) ;
// la même clé renvoie la même zone. 0 si la VRAM est pleine. (ui_hemi.c)
u16 ui_art_reserve(u16 key, u16 ntiles);
// Signale qu'une ligne (PAL2/PAL3) est écrite hors de ui_art : ui_art_close la
// restaurera et la prochaine illustration rechargera sa palette.
void ui_art_claim_palette(u16 pal);
// Restaure les palettes empruntées (styles normaux) et libère la VRAM des
// illustrations (gueules comprises). Les tuiles restent à l'écran : effacer avant
// (ui_clear, ui_art_erase, ui_gueule_erase) pour éviter un affichage aux mauvaises couleurs.
void ui_art_close(void);

// --- Gueules V3 (tâches 3.07 à 3.09) -------------------------------------------
// Portraits caricaturaux 10 x 12 tuiles (80 x 96 px), une palette de 15 couleurs
// PROPRE à chaque personnage : deux gueules à l'écran = deux lignes (PAL2 et PAL3).
// Chaque gueule a 4 expressions ; changer d'expression ne redessine qu'un petit
// rectangle de tuiles autour du visage (tuiles d'expression chargées à la demande).
#include "ui/ui_art_gueules.h"   // GueuleId (généré par tools/art/make_art.py)

typedef enum {
    EXPR_NEUTRE = 0,
    EXPR_CONTENT,
    EXPR_FURIEUX,
    EXPR_GENE,
    EXPR_COUNT
} Expr;

// Dessine la gueule id avec l'expression e, coin haut gauche (x, y) en tuiles, sur
// le plan B, palette dans la ligne pal (PAL2 ou PAL3 ; PAL0/PAL1 refusées -> PAL2).
// Renvoie FALSE si id invalide ou VRAM des illustrations pleine.
bool ui_gueule_draw(GueuleId id, Expr e, u16 x, u16 y, u16 pal);
// Change l'expression d'une gueule déjà dessinée en (x, y) avec la même ligne pal :
// ne réécrit que le rectangle du visage.
bool ui_gueule_expr(GueuleId id, Expr e, u16 x, u16 y, u16 pal);

// Raccourcis (ordres identiques à ceux de la simulation).
#define GUEULE_PRES(orient)  ((GueuleId)(GUEULE_PRES_GAUCHE + (orient)))          // ORIENT_*
#define GUEULE_PM(profile)   ((GueuleId)(GUEULE_PM_TECHNOCRATE + (profile)))      // PM_*
#define GUEULE_COHAB(camp)   ((GueuleId)(GUEULE_COHAB_GAUCHE + (camp)))           // ORIENT_*
#define GUEULE_CAND(cand)    ((GueuleId)(GUEULE_CAND_GAUCHE_RADICALE + (cand)))   // CAND_* (sauf CAND_JOUEUR)
#define GUEULE_CHEF(grp)     ((GueuleId)(GUEULE_CHEF_UGR + (grp)))                // GRP_*
#define GUEULE_MIN(i)        ((GueuleId)(GUEULE_MIN_ECONOMIE + (i)))              // 6 ministres
// Efface (plan B) le rectangle de la gueule placée en x, y.
void ui_gueule_erase(u16 x, u16 y);

// --- Visages animés (tâche 3.18, sans son) -------------------------------------
// La DERNIÈRE gueule dessinée par ui_gueule_draw s'anime seule (une seule à la fois) :
// clignements irréguliers (100-130 ms toutes les 2,5 à 6 s, parfois doubles), regard
// de côté (0,8 à 1,5 s toutes les 8 à 15 s, expressions neutre et content), et
// changement d'expression (ui_gueule_expr) masqué par un clignement de 80 ms.
// Rien pendant les fondus ni sous une boîte de message ; cadences en millisecondes
// (identiques en PAL et en NTSC) ; aléa propre, jamais celui de la simulation.
// L'animation s'arrête d'elle-même si le visage est recouvert (fenêtre, ui_clear,
// ui_gueule_erase, autre illustration) ou à ui_art_close.
//
// Le personnage parle pendant ms millisecondes (bouche irrégulière, 90 à 160 ms par
// image) ; arrêt net à la fin ou à l'appui sur A, B, C ou START ; 0 = se tait.
// À appeler APRÈS ui_gueule_draw (qui remet la bouche au repos).
void ui_gueule_talk(u16 ms);
// Le personnage dit une réplique (charabia, 4.18) : 2 syllabes avec la voix de la dernière
// gueule dessinée, bouche synchronisée sur le son (fermée sur une labiale, peu ouverte sur
// une voyelle fermée), puis repos : seuls les yeux bougent. Voix coupée dans les Options :
// bouche muette pendant ms millisecondes, comme ui_gueule_talk. À appeler après ui_gueule_draw.
void ui_gueule_say(u16 ms);
// Durée de parole conseillée pour une réplique (≈ 45 ms par caractère, 0,6 à 4 s).
u16 ui_gueule_talk_ms(const char *text);
// Remet le visage au repos et arrête l'animation (la gueule reste affichée).
void ui_gueule_anim_stop(void);
// Boucle des écrans (screen.c) : une fois par image.
void ui_gueule_anim_update(void);
// Réglage « Visages » (Options) : FALSE fige les visages sur l'image de base de leur
// expression (les changements d'expression restent, sans clignement) ; TRUE les relance.
void ui_gueule_anim_enable(bool on);
// Couche d'interface : le rectangle (x, y, w, h) du plan B va être redessiné ;
// arrête l'animation s'il recouvre les yeux ou la bouche.
void ui_gueule_anim_cover(u16 x, u16 y, u16 w, u16 h);

#endif
