#include "sim/sim.h"
#include "sim/regions.h"

typedef char reg_count_check[(REG_END == REG_COUNT) ? 1 : -1];

#define SENS_REF     3    // sensibilité « moyenne » : sans effet sur l'écart
#define SENS_DIV     50   // écart = (sens - 3) * (pilier - 500) / 50, en dixièmes de %
#define UNEMP_REF    750
#define GAP_MAX      100

// Poids électorats (Pop Moy Ais) | piliers (Soc Éco Tech Écol Séc Intl) | chômage | habitants (x10 000)
const RegionProfile region_profiles[REG_COUNT] = {
    // Hauts-de-France : ouvrière, industrielle, très sensible au chômage
    { { 50, 38, 12 }, { 6, 2, 0, 0, 2, 0 }, 6, 600 },
    // Normandie : ports, nucléaire, agriculture
    { { 42, 42, 16 }, { 3, 3, 0, 2, 1, 2 }, 3, 330 },
    // Île-de-France : classes aisées, technologie, ouverture internationale
    { { 25, 40, 35 }, { 1, 4, 6, 2, 3, 5 }, 2, 1240 },
    // Grand Est : frontières, industrie, Europe
    { { 46, 40, 14 }, { 4, 4, 1, 0, 4, 4 }, 4, 560 },
    // Bretagne : agriculture, mer, écologie
    { { 38, 46, 16 }, { 2, 2, 2, 6, 0, 1 }, 2, 340 },
    // Pays de la Loire : agriculture et PME
    { { 38, 45, 17 }, { 2, 3, 2, 4, 1, 1 }, 2, 390 },
    // Centre-Val de Loire : rural, nucléaire, services publics
    { { 42, 43, 15 }, { 3, 2, 1, 4, 2, 1 }, 3, 260 },
    // Bourgogne-Franche-Comté : rural et industriel, services publics
    { { 44, 42, 14 }, { 5, 2, 0, 3, 2, 2 }, 4, 280 },
    // Nouvelle-Aquitaine : rurale, viticole, écologie
    { { 42, 42, 16 }, { 3, 2, 1, 5, 1, 1 }, 3, 610 },
    // Auvergne-Rhône-Alpes : industrie, économie, technologie (Lyon, Grenoble)
    { { 36, 42, 22 }, { 2, 5, 4, 2, 2, 3 }, 3, 820 },
    // Occitanie : aéronautique et recherche (Toulouse), agriculture
    { { 44, 40, 16 }, { 4, 2, 5, 3, 2, 2 }, 4, 610 },
    // PACA : sécurité, tourisme, retraités
    { { 40, 38, 22 }, { 3, 4, 2, 1, 7, 2 }, 4, 520 },
    // Corse : insularité, sécurité, services publics
    { { 50, 35, 15 }, { 4, 2, 0, 1, 5, 0 }, 3, 35 },
};

// Effets des lois adoptées sur une région (cible de l'écart, dixièmes de %).
typedef struct { u8 law; u8 region; s8 delta; } RegionLaw;

static const RegionLaw region_laws[] = {
    { LOI_AGRI_PRODUCT, REG_BRETAGNE, 15 }, { LOI_AGRI_PRODUCT, REG_PAYS_DE_LA_LOIRE, 12 },
    { LOI_AGRI_PRODUCT, REG_CENTRE_VAL_DE_LOIRE, 12 }, { LOI_AGRI_PRODUCT, REG_NOUVELLE_AQUITAINE, 10 },
    { LOI_BIO_CANTINE, REG_BRETAGNE, 8 }, { LOI_BIO_CANTINE, REG_PAYS_DE_LA_LOIRE, 6 },
    { LOI_BIO_CANTINE, REG_NOUVELLE_AQUITAINE, 8 }, { LOI_BIO_CANTINE, REG_OCCITANIE, 8 },
    { LOI_ENERGIE, REG_CENTRE_VAL_DE_LOIRE, 15 }, { LOI_ENERGIE, REG_GRAND_EST, 10 },
    { LOI_ENERGIE, REG_NORMANDIE, 10 }, { LOI_ENERGIE, REG_AUVERGNE_RHONE_ALPES, 8 },
    { LOI_TRANSPORTS, REG_ILE_DE_FRANCE, 15 }, { LOI_TRANSPORTS, REG_HAUTS_DE_FRANCE, 6 },
    { LOI_TRANSPORTS, REG_AUVERGNE_RHONE_ALPES, 6 },
    { LOI_DESERTS_MED, REG_BOURGOGNE_FRANCHE_COMTE, 14 }, { LOI_DESERTS_MED, REG_CENTRE_VAL_DE_LOIRE, 12 },
    { LOI_DESERTS_MED, REG_CORSE, 12 }, { LOI_DESERTS_MED, REG_OCCITANIE, 8 },
    { LOI_DESERTS_MED, REG_NOUVELLE_AQUITAINE, 8 },
    { LOI_LOGEMENT, REG_ILE_DE_FRANCE, 10 }, { LOI_LOGEMENT, REG_PACA, 8 },
    { LOI_ENCADREMENT, REG_ILE_DE_FRANCE, 10 },
    { LOI_VOLS_COURTS, REG_OCCITANIE, -12 }, { LOI_VOLS_COURTS, REG_PACA, -6 },
    { LOI_VOLS_COURTS, REG_ILE_DE_FRANCE, -4 },
    { LOI_IMMIGRATION, REG_PACA, 8 }, { LOI_IMMIGRATION, REG_HAUTS_DE_FRANCE, 6 },
    { LOI_POLICE, REG_PACA, 8 }, { LOI_POLICE, REG_ILE_DE_FRANCE, 4 },
    { LOI_CIR, REG_ILE_DE_FRANCE, 6 }, { LOI_CIR, REG_OCCITANIE, 8 },
    { LOI_CIR, REG_AUVERGNE_RHONE_ALPES, 8 },
    { LOI_RENOV, REG_HAUTS_DE_FRANCE, 8 }, { LOI_RENOV, REG_GRAND_EST, 6 },
    { LOI_SMIC, REG_HAUTS_DE_FRANCE, 6 },
    { LOI_CARBONE, REG_HAUTS_DE_FRANCE, -6 }, { LOI_CARBONE, REG_BOURGOGNE_FRANCHE_COMTE, -6 },
};
#define N_REGION_LAWS ((u8)(sizeof(region_laws) / sizeof(region_laws[0])))

s16 sim_region_gap_target(const Country *c, u8 r)
{
    const RegionProfile *p;
    s32 g = 0;
    u8 i;
    if (r >= REG_COUNT) return 0;
    p = &region_profiles[r];
    for (i = 0; i < PIL_TOTAL; i++)
        g += ((s32)p->sens[i] - SENS_REF) * ((s32)c->pillars[i] - 500) / SENS_DIV;
    g -= ((s32)p->sens_unemp - SENS_REF) * ((s32)c->unemployment - UNEMP_REF) / SENS_DIV;
    for (i = 0; i < N_REGION_LAWS; i++)
        if (region_laws[i].region == r && law_is_adopted(c, region_laws[i].law))
            g += region_laws[i].delta;
    return sim_clamp(g, -GAP_MAX, GAP_MAX);
}

s16 sim_region_popularity(const Country *c, u8 r)
{
    const RegionProfile *p;
    s32 v;
    u8 e;
    if (r >= REG_COUNT) return sim_popularity(c);
    p = &region_profiles[r];
    v = 0;
    for (e = 0; e < ELEC_COUNT; e++) v += (s32)p->weight[e] * c->opinion[e];
    v = (v + 50) / 100 + c->region_gap[r];
    return sim_clamp(v, OPINION_MIN, OPINION_MAX);
}

void sim_region_init(Country *c)
{
    u8 r;
    for (r = 0; r < REG_COUNT; r++)
        c->region_gap[r] = (s8)sim_region_gap_target(c, r);
}

void sim_region_update(Country *c)
{
    // Inertie : on comble 1/6 de l'écart par trimestre (arrondi au plus proche).
    s16 div = (s16)(c->turns_per_year * 3 / 2);
    u8 r;
    for (r = 0; r < REG_COUNT; r++) {
        s16 d = (s16)(sim_region_gap_target(c, r) - c->region_gap[r]);
        d = (s16)((d + (d >= 0 ? div / 2 : -(div / 2))) / div);
        c->region_gap[r] = (s8)sim_clamp((s32)c->region_gap[r] + d, -GAP_MAX, GAP_MAX);
    }
}
