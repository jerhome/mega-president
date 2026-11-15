// FICHIER GÉNÉRÉ par tools/art/make_art.py : ne pas modifier à la main.
// Gueules V3 : ordre = res/gfx_art.res (tools/art/persos.py, ALL).
#ifndef UI_ART_GUEULES_H
#define UI_ART_GUEULES_H

typedef enum {
    GUEULE_PRES_GAUCHE,
    GUEULE_MIN_INTERIEUR,
    GUEULE_CHEF_UTL,
    GUEULE_COUNT
} GueuleId;

#define GUEULE_W 10   // en tuiles
#define GUEULE_H 12

#endif

#ifdef UI_ART_GUEULES_TABLE
static const GueuleDef gueule_defs[GUEULE_COUNT] = {
    { &gueule_pres_gauche, { &gueule_pres_gauche_content, &gueule_pres_gauche_furieux, &gueule_pres_gauche_gene }, 3, 3, 5, 5 },
    { &gueule_min_interieur, { &gueule_min_interieur_content, &gueule_min_interieur_furieux, &gueule_min_interieur_gene }, 2, 2, 6, 6 },
    { &gueule_chef_utl, { &gueule_chef_utl_content, &gueule_chef_utl_furieux, &gueule_chef_utl_gene }, 2, 3, 5, 6 },
};
#endif
