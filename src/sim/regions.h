#ifndef SIM_REGIONS_H
#define SIM_REGIONS_H

// V2 (2.01) : popularité par région. Formules : docs/SIM.md « Régions ».

#include "sim/sim_state.h"

// Les 13 régions, dans le MÊME ORDRE que l'enum REGION_* de src/ui/ui_map.h
// (src/sim ne dépend pas de src/ui : correspondance vérifiée par tests/test_regions.c).
enum {
    REG_HAUTS_DE_FRANCE = 0,
    REG_NORMANDIE,
    REG_ILE_DE_FRANCE,
    REG_GRAND_EST,
    REG_BRETAGNE,
    REG_PAYS_DE_LA_LOIRE,
    REG_CENTRE_VAL_DE_LOIRE,
    REG_BOURGOGNE_FRANCHE_COMTE,
    REG_NOUVELLE_AQUITAINE,
    REG_AUVERGNE_RHONE_ALPES,
    REG_OCCITANIE,
    REG_PACA,
    REG_CORSE,
    REG_END
};

// Profil fixe d'une région (ROM).
typedef struct {
    u8 weight[ELEC_COUNT];   // poids des 5 électorats (somme = 100)
    u8 sens[PIL_TOTAL];      // sensibilité aux piliers (0-9, référence 3)
    u8 sens_unemp;           // sensibilité au chômage (0-9, référence 3)
    u16 pop_share;           // population, en dizaines de milliers d'habitants
} RegionProfile;

extern const RegionProfile region_profiles[REG_COUNT];

// Popularité du président dans une région, en dixièmes de % (0-1000).
// Région invalide : popularité nationale.
s16 sim_region_popularity(const Country *c, u8 region);

// Écart de la région avec l'opinion nationale de ses électorats (dixièmes, cible
// vers laquelle l'écart évolue) : piliers, chômage, lois adoptées.
s16 sim_region_gap_target(const Country *c, u8 region);

// Initialise les écarts (à la cible) / les fait évoluer d'un tour (inertie).
void sim_region_init(Country *c);
void sim_region_update(Country *c);

#endif
