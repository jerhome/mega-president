// FICHIER GÉNÉRÉ par tools/art/make_art.py : ne pas modifier à la main.
// Gueules V3 : ordre = res/gfx_art.res (tools/art/persos.py, ALL).
#ifndef UI_ART_GUEULES_H
#define UI_ART_GUEULES_H

typedef enum {
    GUEULE_PRES_GAUCHE,
    GUEULE_PRES_CENTRE,
    GUEULE_PRES_DROITE,
    GUEULE_PM_TECHNOCRATE,
    GUEULE_PM_RASSEMBLEUR,
    GUEULE_PM_REFORMATEUR,
    GUEULE_COHAB_GAUCHE,
    GUEULE_COHAB_CENTRE,
    GUEULE_COHAB_DROITE,
    GUEULE_CAND_GAUCHE_RADICALE,
    GUEULE_CAND_ECOLO,
    GUEULE_CAND_CENTRE,
    GUEULE_CAND_DROITE,
    GUEULE_CAND_NATIONALISTE,
    GUEULE_MIN_ECONOMIE,
    GUEULE_MIN_INTERIEUR,
    GUEULE_MIN_SANTE,
    GUEULE_MIN_EDUCATION,
    GUEULE_MIN_ECOLOGIE,
    GUEULE_MIN_AFFAIRES_ETR,
    GUEULE_CHEF_UGR,
    GUEULE_CHEF_RSV,
    GUEULE_CHEF_MJM,
    GUEULE_CHEF_UTL,
    GUEULE_CHEF_LSO,
    GUEULE_JOUR_CHAINE_INFO,
    GUEULE_JOUR_EDITORIALISTE,
    GUEULE_SENAT_PRESIDENT,
    GUEULE_SYND_UST,
    GUEULE_PATRON_UPE,
    GUEULE_COUNT
} GueuleId;

#define GUEULE_W 10   // en tuiles
#define GUEULE_H 12

#endif

#ifdef UI_ART_GUEULES_TABLE
static const GueuleDef gueule_defs[GUEULE_COUNT] = {
    { &gueule_pres_gauche, { &gueule_pres_gauche_content, &gueule_pres_gauche_furieux, &gueule_pres_gauche_gene }, 3, 3, 5, 5 },
    { &gueule_pres_centre, { &gueule_pres_centre_content, &gueule_pres_centre_furieux, &gueule_pres_centre_gene }, 3, 2, 5, 6 },
    { &gueule_pres_droite, { &gueule_pres_droite_content, &gueule_pres_droite_furieux, &gueule_pres_droite_gene }, 2, 2, 5, 6 },
    { &gueule_pm_technocrate, { &gueule_pm_technocrate_content, &gueule_pm_technocrate_furieux, &gueule_pm_technocrate_gene }, 3, 1, 4, 7 },
    { &gueule_pm_rassembleur, { &gueule_pm_rassembleur_content, &gueule_pm_rassembleur_furieux, &gueule_pm_rassembleur_gene }, 3, 3, 5, 5 },
    { &gueule_pm_reformateur, { &gueule_pm_reformateur_content, &gueule_pm_reformateur_furieux, &gueule_pm_reformateur_gene }, 2, 0, 6, 8 },
    { &gueule_cohab_gauche, { &gueule_cohab_gauche_content, &gueule_cohab_gauche_furieux, &gueule_cohab_gauche_gene }, 3, 3, 5, 6 },
    { &gueule_cohab_centre, { &gueule_cohab_centre_content, &gueule_cohab_centre_furieux, &gueule_cohab_centre_gene }, 2, 2, 6, 6 },
    { &gueule_cohab_droite, { &gueule_cohab_droite_content, &gueule_cohab_droite_furieux, &gueule_cohab_droite_gene }, 2, 3, 6, 5 },
    { &gueule_cand_gauche_radicale, { &gueule_cand_gauche_radicale_content, &gueule_cand_gauche_radicale_furieux, &gueule_cand_gauche_radicale_gene }, 3, 2, 5, 8 },
    { &gueule_cand_ecolo, { &gueule_cand_ecolo_content, &gueule_cand_ecolo_furieux, &gueule_cand_ecolo_gene }, 2, 4, 5, 4 },
    { &gueule_cand_centre, { &gueule_cand_centre_content, &gueule_cand_centre_furieux, &gueule_cand_centre_gene }, 2, 3, 5, 5 },
    { &gueule_cand_droite, { &gueule_cand_droite_content, &gueule_cand_droite_furieux, &gueule_cand_droite_gene }, 3, 3, 5, 6 },
    { &gueule_cand_nationaliste, { &gueule_cand_nationaliste_content, &gueule_cand_nationaliste_furieux, &gueule_cand_nationaliste_gene }, 3, 3, 5, 6 },
    { &gueule_min_economie, { &gueule_min_economie_content, &gueule_min_economie_furieux, &gueule_min_economie_gene }, 3, 3, 5, 5 },
    { &gueule_min_interieur, { &gueule_min_interieur_content, &gueule_min_interieur_furieux, &gueule_min_interieur_gene }, 2, 2, 6, 6 },
    { &gueule_min_sante, { &gueule_min_sante_content, &gueule_min_sante_furieux, &gueule_min_sante_gene }, 2, 3, 5, 5 },
    { &gueule_min_education, { &gueule_min_education_content, &gueule_min_education_furieux, &gueule_min_education_gene }, 2, 2, 5, 6 },
    { &gueule_min_ecologie, { &gueule_min_ecologie_content, &gueule_min_ecologie_furieux, &gueule_min_ecologie_gene }, 3, 1, 5, 7 },
    { &gueule_min_affaires_etr, { &gueule_min_affaires_etr_content, &gueule_min_affaires_etr_furieux, &gueule_min_affaires_etr_gene }, 2, 3, 5, 5 },
    { &gueule_chef_ugr, { &gueule_chef_ugr_content, &gueule_chef_ugr_furieux, &gueule_chef_ugr_gene }, 3, 2, 5, 6 },
    { &gueule_chef_rsv, { &gueule_chef_rsv_content, &gueule_chef_rsv_furieux, &gueule_chef_rsv_gene }, 2, 4, 6, 4 },
    { &gueule_chef_mjm, { &gueule_chef_mjm_content, &gueule_chef_mjm_furieux, &gueule_chef_mjm_gene }, 3, 2, 5, 6 },
    { &gueule_chef_utl, { &gueule_chef_utl_content, &gueule_chef_utl_furieux, &gueule_chef_utl_gene }, 2, 3, 5, 6 },
    { &gueule_chef_lso, { &gueule_chef_lso_content, &gueule_chef_lso_furieux, &gueule_chef_lso_gene }, 2, 3, 5, 5 },
    { &gueule_jour_chaine_info, { &gueule_jour_chaine_info_content, &gueule_jour_chaine_info_furieux, &gueule_jour_chaine_info_gene }, 3, 2, 5, 6 },
    { &gueule_jour_editorialiste, { &gueule_jour_editorialiste_content, &gueule_jour_editorialiste_furieux, &gueule_jour_editorialiste_gene }, 1, 1, 8, 8 },
    { &gueule_senat_president, { &gueule_senat_president_content, &gueule_senat_president_furieux, &gueule_senat_president_gene }, 3, 2, 4, 7 },
    { &gueule_synd_ust, { &gueule_synd_ust_content, &gueule_synd_ust_furieux, &gueule_synd_ust_gene }, 2, 3, 5, 5 },
    { &gueule_patron_upe, { &gueule_patron_upe_content, &gueule_patron_upe_furieux, &gueule_patron_upe_gene }, 2, 2, 5, 7 },
};
#endif
