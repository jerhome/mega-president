// Mega Président — illustrations (tâche 2.02), générées par tools/art/make_art.py.
// Ne pas éditer : relancer le script. L'ordre des IMAGE = enum ArtId (src/ui/ui_art.h).
// Pas de compression (chargement direct en VRAM par DMA, aucun tampon en RAM) ;
// tuiles dédoublonnées (ALL : identiques et retournées).

// Écran titre (320 x 128)
IMAGE art_title              "gfx/art_title.png" NONE ALL

// Portraits (56 x 56, palette commune)

// Vignette d'événement (64 x 48)
IMAGE art_evt_greve          "gfx/art_evt_greve.png" NONE ALL

// Vignette d'événement (64 x 48)
IMAGE art_evt_canicule       "gfx/art_evt_canicule.png" NONE ALL

// Vignette d'événement (64 x 48)
IMAGE art_evt_inondation     "gfx/art_evt_inondation.png" NONE ALL

// Vignette d'événement (64 x 48)
IMAGE art_evt_scandale       "gfx/art_evt_scandale.png" NONE ALL

// Vignette d'événement (64 x 48)
IMAGE art_evt_sommet         "gfx/art_evt_sommet.png" NONE ALL

// Vignette d'événement (64 x 48)
IMAGE art_evt_bourse         "gfx/art_evt_bourse.png" NONE ALL

// Vignette d'événement (64 x 48)
IMAGE art_evt_sante          "gfx/art_evt_sante.png" NONE ALL

// Vignette d'événement (64 x 48)
IMAGE art_evt_pollution      "gfx/art_evt_pollution.png" NONE ALL

// Vignette d'événement (64 x 48)
IMAGE art_evt_securite       "gfx/art_evt_securite.png" NONE ALL

// Vignette d'événement (64 x 48)
IMAGE art_evt_numerique      "gfx/art_evt_numerique.png" NONE ALL

// Vignette d'événement (64 x 48)
IMAGE art_evt_education      "gfx/art_evt_education.png" NONE ALL

// Vignette d'événement (64 x 48)
IMAGE art_evt_energie        "gfx/art_evt_energie.png" NONE ALL

// Vignette d'événement (64 x 48)
IMAGE art_evt_agriculture    "gfx/art_evt_agriculture.png" NONE ALL

// Vignette d'événement (64 x 48)
IMAGE art_evt_industrie      "gfx/art_evt_industrie.png" NONE ALL

// Vignette d'événement (64 x 48)
IMAGE art_evt_fete           "gfx/art_evt_fete.png" NONE ALL

// Décor
IMAGE art_decor_hemicycle    "gfx/art_decor_hemicycle.png" NONE ALL

// Décor
IMAGE art_decor_urne         "gfx/art_decor_urne.png" NONE ALL

// Décor
IMAGE art_decor_journal      "gfx/art_decor_journal.png" NONE ALL

// Gueules V3 (80 x 96, une palette par personnage) : portrait neutre, puis un
// patch par expression (content, furieux, gêné) à poser par-dessus (tools/art/g_*.py),
// puis une planche d'animation par expression (clignement, regard, parole : 3.18).
IMAGE gueule_pres_gauche             "gfx/gueule_pres_gauche.png" NONE ALL
IMAGE gueule_pres_gauche_content     "gfx/gueule_pres_gauche_content.png" NONE ALL
IMAGE gueule_pres_gauche_furieux     "gfx/gueule_pres_gauche_furieux.png" NONE ALL
IMAGE gueule_pres_gauche_gene        "gfx/gueule_pres_gauche_gene.png" NONE ALL
IMAGE gueule_pres_gauche_neutre_anim "gfx/gueule_pres_gauche_neutre_anim.png" NONE ALL
IMAGE gueule_pres_gauche_content_anim "gfx/gueule_pres_gauche_content_anim.png" NONE ALL
IMAGE gueule_pres_gauche_furieux_anim "gfx/gueule_pres_gauche_furieux_anim.png" NONE ALL
IMAGE gueule_pres_gauche_gene_anim   "gfx/gueule_pres_gauche_gene_anim.png" NONE ALL
IMAGE gueule_pres_centre             "gfx/gueule_pres_centre.png" NONE ALL
IMAGE gueule_pres_centre_content     "gfx/gueule_pres_centre_content.png" NONE ALL
IMAGE gueule_pres_centre_furieux     "gfx/gueule_pres_centre_furieux.png" NONE ALL
IMAGE gueule_pres_centre_gene        "gfx/gueule_pres_centre_gene.png" NONE ALL
IMAGE gueule_pres_centre_neutre_anim "gfx/gueule_pres_centre_neutre_anim.png" NONE ALL
IMAGE gueule_pres_centre_content_anim "gfx/gueule_pres_centre_content_anim.png" NONE ALL
IMAGE gueule_pres_centre_furieux_anim "gfx/gueule_pres_centre_furieux_anim.png" NONE ALL
IMAGE gueule_pres_centre_gene_anim   "gfx/gueule_pres_centre_gene_anim.png" NONE ALL
IMAGE gueule_pres_droite             "gfx/gueule_pres_droite.png" NONE ALL
IMAGE gueule_pres_droite_content     "gfx/gueule_pres_droite_content.png" NONE ALL
IMAGE gueule_pres_droite_furieux     "gfx/gueule_pres_droite_furieux.png" NONE ALL
IMAGE gueule_pres_droite_gene        "gfx/gueule_pres_droite_gene.png" NONE ALL
IMAGE gueule_pres_droite_neutre_anim "gfx/gueule_pres_droite_neutre_anim.png" NONE ALL
IMAGE gueule_pres_droite_content_anim "gfx/gueule_pres_droite_content_anim.png" NONE ALL
IMAGE gueule_pres_droite_furieux_anim "gfx/gueule_pres_droite_furieux_anim.png" NONE ALL
IMAGE gueule_pres_droite_gene_anim   "gfx/gueule_pres_droite_gene_anim.png" NONE ALL
IMAGE gueule_pm_technocrate          "gfx/gueule_pm_technocrate.png" NONE ALL
IMAGE gueule_pm_technocrate_content  "gfx/gueule_pm_technocrate_content.png" NONE ALL
IMAGE gueule_pm_technocrate_furieux  "gfx/gueule_pm_technocrate_furieux.png" NONE ALL
IMAGE gueule_pm_technocrate_gene     "gfx/gueule_pm_technocrate_gene.png" NONE ALL
IMAGE gueule_pm_technocrate_neutre_anim "gfx/gueule_pm_technocrate_neutre_anim.png" NONE ALL
IMAGE gueule_pm_technocrate_content_anim "gfx/gueule_pm_technocrate_content_anim.png" NONE ALL
IMAGE gueule_pm_technocrate_furieux_anim "gfx/gueule_pm_technocrate_furieux_anim.png" NONE ALL
IMAGE gueule_pm_technocrate_gene_anim "gfx/gueule_pm_technocrate_gene_anim.png" NONE ALL
IMAGE gueule_pm_rassembleur          "gfx/gueule_pm_rassembleur.png" NONE ALL
IMAGE gueule_pm_rassembleur_content  "gfx/gueule_pm_rassembleur_content.png" NONE ALL
IMAGE gueule_pm_rassembleur_furieux  "gfx/gueule_pm_rassembleur_furieux.png" NONE ALL
IMAGE gueule_pm_rassembleur_gene     "gfx/gueule_pm_rassembleur_gene.png" NONE ALL
IMAGE gueule_pm_rassembleur_neutre_anim "gfx/gueule_pm_rassembleur_neutre_anim.png" NONE ALL
IMAGE gueule_pm_rassembleur_content_anim "gfx/gueule_pm_rassembleur_content_anim.png" NONE ALL
IMAGE gueule_pm_rassembleur_furieux_anim "gfx/gueule_pm_rassembleur_furieux_anim.png" NONE ALL
IMAGE gueule_pm_rassembleur_gene_anim "gfx/gueule_pm_rassembleur_gene_anim.png" NONE ALL
IMAGE gueule_pm_reformateur          "gfx/gueule_pm_reformateur.png" NONE ALL
IMAGE gueule_pm_reformateur_content  "gfx/gueule_pm_reformateur_content.png" NONE ALL
IMAGE gueule_pm_reformateur_furieux  "gfx/gueule_pm_reformateur_furieux.png" NONE ALL
IMAGE gueule_pm_reformateur_gene     "gfx/gueule_pm_reformateur_gene.png" NONE ALL
IMAGE gueule_pm_reformateur_neutre_anim "gfx/gueule_pm_reformateur_neutre_anim.png" NONE ALL
IMAGE gueule_pm_reformateur_content_anim "gfx/gueule_pm_reformateur_content_anim.png" NONE ALL
IMAGE gueule_pm_reformateur_furieux_anim "gfx/gueule_pm_reformateur_furieux_anim.png" NONE ALL
IMAGE gueule_pm_reformateur_gene_anim "gfx/gueule_pm_reformateur_gene_anim.png" NONE ALL
IMAGE gueule_cohab_gauche            "gfx/gueule_cohab_gauche.png" NONE ALL
IMAGE gueule_cohab_gauche_content    "gfx/gueule_cohab_gauche_content.png" NONE ALL
IMAGE gueule_cohab_gauche_furieux    "gfx/gueule_cohab_gauche_furieux.png" NONE ALL
IMAGE gueule_cohab_gauche_gene       "gfx/gueule_cohab_gauche_gene.png" NONE ALL
IMAGE gueule_cohab_gauche_neutre_anim "gfx/gueule_cohab_gauche_neutre_anim.png" NONE ALL
IMAGE gueule_cohab_gauche_content_anim "gfx/gueule_cohab_gauche_content_anim.png" NONE ALL
IMAGE gueule_cohab_gauche_furieux_anim "gfx/gueule_cohab_gauche_furieux_anim.png" NONE ALL
IMAGE gueule_cohab_gauche_gene_anim  "gfx/gueule_cohab_gauche_gene_anim.png" NONE ALL
IMAGE gueule_cohab_centre            "gfx/gueule_cohab_centre.png" NONE ALL
IMAGE gueule_cohab_centre_content    "gfx/gueule_cohab_centre_content.png" NONE ALL
IMAGE gueule_cohab_centre_furieux    "gfx/gueule_cohab_centre_furieux.png" NONE ALL
IMAGE gueule_cohab_centre_gene       "gfx/gueule_cohab_centre_gene.png" NONE ALL
IMAGE gueule_cohab_centre_neutre_anim "gfx/gueule_cohab_centre_neutre_anim.png" NONE ALL
IMAGE gueule_cohab_centre_content_anim "gfx/gueule_cohab_centre_content_anim.png" NONE ALL
IMAGE gueule_cohab_centre_furieux_anim "gfx/gueule_cohab_centre_furieux_anim.png" NONE ALL
IMAGE gueule_cohab_centre_gene_anim  "gfx/gueule_cohab_centre_gene_anim.png" NONE ALL
IMAGE gueule_cohab_droite            "gfx/gueule_cohab_droite.png" NONE ALL
IMAGE gueule_cohab_droite_content    "gfx/gueule_cohab_droite_content.png" NONE ALL
IMAGE gueule_cohab_droite_furieux    "gfx/gueule_cohab_droite_furieux.png" NONE ALL
IMAGE gueule_cohab_droite_gene       "gfx/gueule_cohab_droite_gene.png" NONE ALL
IMAGE gueule_cohab_droite_neutre_anim "gfx/gueule_cohab_droite_neutre_anim.png" NONE ALL
IMAGE gueule_cohab_droite_content_anim "gfx/gueule_cohab_droite_content_anim.png" NONE ALL
IMAGE gueule_cohab_droite_furieux_anim "gfx/gueule_cohab_droite_furieux_anim.png" NONE ALL
IMAGE gueule_cohab_droite_gene_anim  "gfx/gueule_cohab_droite_gene_anim.png" NONE ALL
IMAGE gueule_cand_gauche_radicale    "gfx/gueule_cand_gauche_radicale.png" NONE ALL
IMAGE gueule_cand_gauche_radicale_content "gfx/gueule_cand_gauche_radicale_content.png" NONE ALL
IMAGE gueule_cand_gauche_radicale_furieux "gfx/gueule_cand_gauche_radicale_furieux.png" NONE ALL
IMAGE gueule_cand_gauche_radicale_gene "gfx/gueule_cand_gauche_radicale_gene.png" NONE ALL
IMAGE gueule_cand_gauche_radicale_neutre_anim "gfx/gueule_cand_gauche_radicale_neutre_anim.png" NONE ALL
IMAGE gueule_cand_gauche_radicale_content_anim "gfx/gueule_cand_gauche_radicale_content_anim.png" NONE ALL
IMAGE gueule_cand_gauche_radicale_furieux_anim "gfx/gueule_cand_gauche_radicale_furieux_anim.png" NONE ALL
IMAGE gueule_cand_gauche_radicale_gene_anim "gfx/gueule_cand_gauche_radicale_gene_anim.png" NONE ALL
IMAGE gueule_cand_ecolo              "gfx/gueule_cand_ecolo.png" NONE ALL
IMAGE gueule_cand_ecolo_content      "gfx/gueule_cand_ecolo_content.png" NONE ALL
IMAGE gueule_cand_ecolo_furieux      "gfx/gueule_cand_ecolo_furieux.png" NONE ALL
IMAGE gueule_cand_ecolo_gene         "gfx/gueule_cand_ecolo_gene.png" NONE ALL
IMAGE gueule_cand_ecolo_neutre_anim  "gfx/gueule_cand_ecolo_neutre_anim.png" NONE ALL
IMAGE gueule_cand_ecolo_content_anim "gfx/gueule_cand_ecolo_content_anim.png" NONE ALL
IMAGE gueule_cand_ecolo_furieux_anim "gfx/gueule_cand_ecolo_furieux_anim.png" NONE ALL
IMAGE gueule_cand_ecolo_gene_anim    "gfx/gueule_cand_ecolo_gene_anim.png" NONE ALL
IMAGE gueule_cand_centre             "gfx/gueule_cand_centre.png" NONE ALL
IMAGE gueule_cand_centre_content     "gfx/gueule_cand_centre_content.png" NONE ALL
IMAGE gueule_cand_centre_furieux     "gfx/gueule_cand_centre_furieux.png" NONE ALL
IMAGE gueule_cand_centre_gene        "gfx/gueule_cand_centre_gene.png" NONE ALL
IMAGE gueule_cand_centre_neutre_anim "gfx/gueule_cand_centre_neutre_anim.png" NONE ALL
IMAGE gueule_cand_centre_content_anim "gfx/gueule_cand_centre_content_anim.png" NONE ALL
IMAGE gueule_cand_centre_furieux_anim "gfx/gueule_cand_centre_furieux_anim.png" NONE ALL
IMAGE gueule_cand_centre_gene_anim   "gfx/gueule_cand_centre_gene_anim.png" NONE ALL
IMAGE gueule_cand_droite             "gfx/gueule_cand_droite.png" NONE ALL
IMAGE gueule_cand_droite_content     "gfx/gueule_cand_droite_content.png" NONE ALL
IMAGE gueule_cand_droite_furieux     "gfx/gueule_cand_droite_furieux.png" NONE ALL
IMAGE gueule_cand_droite_gene        "gfx/gueule_cand_droite_gene.png" NONE ALL
IMAGE gueule_cand_droite_neutre_anim "gfx/gueule_cand_droite_neutre_anim.png" NONE ALL
IMAGE gueule_cand_droite_content_anim "gfx/gueule_cand_droite_content_anim.png" NONE ALL
IMAGE gueule_cand_droite_furieux_anim "gfx/gueule_cand_droite_furieux_anim.png" NONE ALL
IMAGE gueule_cand_droite_gene_anim   "gfx/gueule_cand_droite_gene_anim.png" NONE ALL
IMAGE gueule_cand_nationaliste       "gfx/gueule_cand_nationaliste.png" NONE ALL
IMAGE gueule_cand_nationaliste_content "gfx/gueule_cand_nationaliste_content.png" NONE ALL
IMAGE gueule_cand_nationaliste_furieux "gfx/gueule_cand_nationaliste_furieux.png" NONE ALL
IMAGE gueule_cand_nationaliste_gene  "gfx/gueule_cand_nationaliste_gene.png" NONE ALL
IMAGE gueule_cand_nationaliste_neutre_anim "gfx/gueule_cand_nationaliste_neutre_anim.png" NONE ALL
IMAGE gueule_cand_nationaliste_content_anim "gfx/gueule_cand_nationaliste_content_anim.png" NONE ALL
IMAGE gueule_cand_nationaliste_furieux_anim "gfx/gueule_cand_nationaliste_furieux_anim.png" NONE ALL
IMAGE gueule_cand_nationaliste_gene_anim "gfx/gueule_cand_nationaliste_gene_anim.png" NONE ALL
IMAGE gueule_min_economie            "gfx/gueule_min_economie.png" NONE ALL
IMAGE gueule_min_economie_content    "gfx/gueule_min_economie_content.png" NONE ALL
IMAGE gueule_min_economie_furieux    "gfx/gueule_min_economie_furieux.png" NONE ALL
IMAGE gueule_min_economie_gene       "gfx/gueule_min_economie_gene.png" NONE ALL
IMAGE gueule_min_economie_neutre_anim "gfx/gueule_min_economie_neutre_anim.png" NONE ALL
IMAGE gueule_min_economie_content_anim "gfx/gueule_min_economie_content_anim.png" NONE ALL
IMAGE gueule_min_economie_furieux_anim "gfx/gueule_min_economie_furieux_anim.png" NONE ALL
IMAGE gueule_min_economie_gene_anim  "gfx/gueule_min_economie_gene_anim.png" NONE ALL
IMAGE gueule_min_interieur           "gfx/gueule_min_interieur.png" NONE ALL
IMAGE gueule_min_interieur_content   "gfx/gueule_min_interieur_content.png" NONE ALL
IMAGE gueule_min_interieur_furieux   "gfx/gueule_min_interieur_furieux.png" NONE ALL
IMAGE gueule_min_interieur_gene      "gfx/gueule_min_interieur_gene.png" NONE ALL
IMAGE gueule_min_interieur_neutre_anim "gfx/gueule_min_interieur_neutre_anim.png" NONE ALL
IMAGE gueule_min_interieur_content_anim "gfx/gueule_min_interieur_content_anim.png" NONE ALL
IMAGE gueule_min_interieur_furieux_anim "gfx/gueule_min_interieur_furieux_anim.png" NONE ALL
IMAGE gueule_min_interieur_gene_anim "gfx/gueule_min_interieur_gene_anim.png" NONE ALL
IMAGE gueule_min_sante               "gfx/gueule_min_sante.png" NONE ALL
IMAGE gueule_min_sante_content       "gfx/gueule_min_sante_content.png" NONE ALL
IMAGE gueule_min_sante_furieux       "gfx/gueule_min_sante_furieux.png" NONE ALL
IMAGE gueule_min_sante_gene          "gfx/gueule_min_sante_gene.png" NONE ALL
IMAGE gueule_min_sante_neutre_anim   "gfx/gueule_min_sante_neutre_anim.png" NONE ALL
IMAGE gueule_min_sante_content_anim  "gfx/gueule_min_sante_content_anim.png" NONE ALL
IMAGE gueule_min_sante_furieux_anim  "gfx/gueule_min_sante_furieux_anim.png" NONE ALL
IMAGE gueule_min_sante_gene_anim     "gfx/gueule_min_sante_gene_anim.png" NONE ALL
IMAGE gueule_min_education           "gfx/gueule_min_education.png" NONE ALL
IMAGE gueule_min_education_content   "gfx/gueule_min_education_content.png" NONE ALL
IMAGE gueule_min_education_furieux   "gfx/gueule_min_education_furieux.png" NONE ALL
IMAGE gueule_min_education_gene      "gfx/gueule_min_education_gene.png" NONE ALL
IMAGE gueule_min_education_neutre_anim "gfx/gueule_min_education_neutre_anim.png" NONE ALL
IMAGE gueule_min_education_content_anim "gfx/gueule_min_education_content_anim.png" NONE ALL
IMAGE gueule_min_education_furieux_anim "gfx/gueule_min_education_furieux_anim.png" NONE ALL
IMAGE gueule_min_education_gene_anim "gfx/gueule_min_education_gene_anim.png" NONE ALL
IMAGE gueule_min_ecologie            "gfx/gueule_min_ecologie.png" NONE ALL
IMAGE gueule_min_ecologie_content    "gfx/gueule_min_ecologie_content.png" NONE ALL
IMAGE gueule_min_ecologie_furieux    "gfx/gueule_min_ecologie_furieux.png" NONE ALL
IMAGE gueule_min_ecologie_gene       "gfx/gueule_min_ecologie_gene.png" NONE ALL
IMAGE gueule_min_ecologie_neutre_anim "gfx/gueule_min_ecologie_neutre_anim.png" NONE ALL
IMAGE gueule_min_ecologie_content_anim "gfx/gueule_min_ecologie_content_anim.png" NONE ALL
IMAGE gueule_min_ecologie_furieux_anim "gfx/gueule_min_ecologie_furieux_anim.png" NONE ALL
IMAGE gueule_min_ecologie_gene_anim  "gfx/gueule_min_ecologie_gene_anim.png" NONE ALL
IMAGE gueule_min_affaires_etr        "gfx/gueule_min_affaires_etr.png" NONE ALL
IMAGE gueule_min_affaires_etr_content "gfx/gueule_min_affaires_etr_content.png" NONE ALL
IMAGE gueule_min_affaires_etr_furieux "gfx/gueule_min_affaires_etr_furieux.png" NONE ALL
IMAGE gueule_min_affaires_etr_gene   "gfx/gueule_min_affaires_etr_gene.png" NONE ALL
IMAGE gueule_min_affaires_etr_neutre_anim "gfx/gueule_min_affaires_etr_neutre_anim.png" NONE ALL
IMAGE gueule_min_affaires_etr_content_anim "gfx/gueule_min_affaires_etr_content_anim.png" NONE ALL
IMAGE gueule_min_affaires_etr_furieux_anim "gfx/gueule_min_affaires_etr_furieux_anim.png" NONE ALL
IMAGE gueule_min_affaires_etr_gene_anim "gfx/gueule_min_affaires_etr_gene_anim.png" NONE ALL
IMAGE gueule_chef_ugr                "gfx/gueule_chef_ugr.png" NONE ALL
IMAGE gueule_chef_ugr_content        "gfx/gueule_chef_ugr_content.png" NONE ALL
IMAGE gueule_chef_ugr_furieux        "gfx/gueule_chef_ugr_furieux.png" NONE ALL
IMAGE gueule_chef_ugr_gene           "gfx/gueule_chef_ugr_gene.png" NONE ALL
IMAGE gueule_chef_ugr_neutre_anim    "gfx/gueule_chef_ugr_neutre_anim.png" NONE ALL
IMAGE gueule_chef_ugr_content_anim   "gfx/gueule_chef_ugr_content_anim.png" NONE ALL
IMAGE gueule_chef_ugr_furieux_anim   "gfx/gueule_chef_ugr_furieux_anim.png" NONE ALL
IMAGE gueule_chef_ugr_gene_anim      "gfx/gueule_chef_ugr_gene_anim.png" NONE ALL
IMAGE gueule_chef_rsv                "gfx/gueule_chef_rsv.png" NONE ALL
IMAGE gueule_chef_rsv_content        "gfx/gueule_chef_rsv_content.png" NONE ALL
IMAGE gueule_chef_rsv_furieux        "gfx/gueule_chef_rsv_furieux.png" NONE ALL
IMAGE gueule_chef_rsv_gene           "gfx/gueule_chef_rsv_gene.png" NONE ALL
IMAGE gueule_chef_rsv_neutre_anim    "gfx/gueule_chef_rsv_neutre_anim.png" NONE ALL
IMAGE gueule_chef_rsv_content_anim   "gfx/gueule_chef_rsv_content_anim.png" NONE ALL
IMAGE gueule_chef_rsv_furieux_anim   "gfx/gueule_chef_rsv_furieux_anim.png" NONE ALL
IMAGE gueule_chef_rsv_gene_anim      "gfx/gueule_chef_rsv_gene_anim.png" NONE ALL
IMAGE gueule_chef_mjm                "gfx/gueule_chef_mjm.png" NONE ALL
IMAGE gueule_chef_mjm_content        "gfx/gueule_chef_mjm_content.png" NONE ALL
IMAGE gueule_chef_mjm_furieux        "gfx/gueule_chef_mjm_furieux.png" NONE ALL
IMAGE gueule_chef_mjm_gene           "gfx/gueule_chef_mjm_gene.png" NONE ALL
IMAGE gueule_chef_mjm_neutre_anim    "gfx/gueule_chef_mjm_neutre_anim.png" NONE ALL
IMAGE gueule_chef_mjm_content_anim   "gfx/gueule_chef_mjm_content_anim.png" NONE ALL
IMAGE gueule_chef_mjm_furieux_anim   "gfx/gueule_chef_mjm_furieux_anim.png" NONE ALL
IMAGE gueule_chef_mjm_gene_anim      "gfx/gueule_chef_mjm_gene_anim.png" NONE ALL
IMAGE gueule_chef_utl                "gfx/gueule_chef_utl.png" NONE ALL
IMAGE gueule_chef_utl_content        "gfx/gueule_chef_utl_content.png" NONE ALL
IMAGE gueule_chef_utl_furieux        "gfx/gueule_chef_utl_furieux.png" NONE ALL
IMAGE gueule_chef_utl_gene           "gfx/gueule_chef_utl_gene.png" NONE ALL
IMAGE gueule_chef_utl_neutre_anim    "gfx/gueule_chef_utl_neutre_anim.png" NONE ALL
IMAGE gueule_chef_utl_content_anim   "gfx/gueule_chef_utl_content_anim.png" NONE ALL
IMAGE gueule_chef_utl_furieux_anim   "gfx/gueule_chef_utl_furieux_anim.png" NONE ALL
IMAGE gueule_chef_utl_gene_anim      "gfx/gueule_chef_utl_gene_anim.png" NONE ALL
IMAGE gueule_chef_lso                "gfx/gueule_chef_lso.png" NONE ALL
IMAGE gueule_chef_lso_content        "gfx/gueule_chef_lso_content.png" NONE ALL
IMAGE gueule_chef_lso_furieux        "gfx/gueule_chef_lso_furieux.png" NONE ALL
IMAGE gueule_chef_lso_gene           "gfx/gueule_chef_lso_gene.png" NONE ALL
IMAGE gueule_chef_lso_neutre_anim    "gfx/gueule_chef_lso_neutre_anim.png" NONE ALL
IMAGE gueule_chef_lso_content_anim   "gfx/gueule_chef_lso_content_anim.png" NONE ALL
IMAGE gueule_chef_lso_furieux_anim   "gfx/gueule_chef_lso_furieux_anim.png" NONE ALL
IMAGE gueule_chef_lso_gene_anim      "gfx/gueule_chef_lso_gene_anim.png" NONE ALL
IMAGE gueule_jour_chaine_info        "gfx/gueule_jour_chaine_info.png" NONE ALL
IMAGE gueule_jour_chaine_info_content "gfx/gueule_jour_chaine_info_content.png" NONE ALL
IMAGE gueule_jour_chaine_info_furieux "gfx/gueule_jour_chaine_info_furieux.png" NONE ALL
IMAGE gueule_jour_chaine_info_gene   "gfx/gueule_jour_chaine_info_gene.png" NONE ALL
IMAGE gueule_jour_chaine_info_neutre_anim "gfx/gueule_jour_chaine_info_neutre_anim.png" NONE ALL
IMAGE gueule_jour_chaine_info_content_anim "gfx/gueule_jour_chaine_info_content_anim.png" NONE ALL
IMAGE gueule_jour_chaine_info_furieux_anim "gfx/gueule_jour_chaine_info_furieux_anim.png" NONE ALL
IMAGE gueule_jour_chaine_info_gene_anim "gfx/gueule_jour_chaine_info_gene_anim.png" NONE ALL
IMAGE gueule_jour_editorialiste      "gfx/gueule_jour_editorialiste.png" NONE ALL
IMAGE gueule_jour_editorialiste_content "gfx/gueule_jour_editorialiste_content.png" NONE ALL
IMAGE gueule_jour_editorialiste_furieux "gfx/gueule_jour_editorialiste_furieux.png" NONE ALL
IMAGE gueule_jour_editorialiste_gene "gfx/gueule_jour_editorialiste_gene.png" NONE ALL
IMAGE gueule_jour_editorialiste_neutre_anim "gfx/gueule_jour_editorialiste_neutre_anim.png" NONE ALL
IMAGE gueule_jour_editorialiste_content_anim "gfx/gueule_jour_editorialiste_content_anim.png" NONE ALL
IMAGE gueule_jour_editorialiste_furieux_anim "gfx/gueule_jour_editorialiste_furieux_anim.png" NONE ALL
IMAGE gueule_jour_editorialiste_gene_anim "gfx/gueule_jour_editorialiste_gene_anim.png" NONE ALL
IMAGE gueule_senat_president         "gfx/gueule_senat_president.png" NONE ALL
IMAGE gueule_senat_president_content "gfx/gueule_senat_president_content.png" NONE ALL
IMAGE gueule_senat_president_furieux "gfx/gueule_senat_president_furieux.png" NONE ALL
IMAGE gueule_senat_president_gene    "gfx/gueule_senat_president_gene.png" NONE ALL
IMAGE gueule_senat_president_neutre_anim "gfx/gueule_senat_president_neutre_anim.png" NONE ALL
IMAGE gueule_senat_president_content_anim "gfx/gueule_senat_president_content_anim.png" NONE ALL
IMAGE gueule_senat_president_furieux_anim "gfx/gueule_senat_president_furieux_anim.png" NONE ALL
IMAGE gueule_senat_president_gene_anim "gfx/gueule_senat_president_gene_anim.png" NONE ALL
IMAGE gueule_synd_ust                "gfx/gueule_synd_ust.png" NONE ALL
IMAGE gueule_synd_ust_content        "gfx/gueule_synd_ust_content.png" NONE ALL
IMAGE gueule_synd_ust_furieux        "gfx/gueule_synd_ust_furieux.png" NONE ALL
IMAGE gueule_synd_ust_gene           "gfx/gueule_synd_ust_gene.png" NONE ALL
IMAGE gueule_synd_ust_neutre_anim    "gfx/gueule_synd_ust_neutre_anim.png" NONE ALL
IMAGE gueule_synd_ust_content_anim   "gfx/gueule_synd_ust_content_anim.png" NONE ALL
IMAGE gueule_synd_ust_furieux_anim   "gfx/gueule_synd_ust_furieux_anim.png" NONE ALL
IMAGE gueule_synd_ust_gene_anim      "gfx/gueule_synd_ust_gene_anim.png" NONE ALL
IMAGE gueule_patron_upe              "gfx/gueule_patron_upe.png" NONE ALL
IMAGE gueule_patron_upe_content      "gfx/gueule_patron_upe_content.png" NONE ALL
IMAGE gueule_patron_upe_furieux      "gfx/gueule_patron_upe_furieux.png" NONE ALL
IMAGE gueule_patron_upe_gene         "gfx/gueule_patron_upe_gene.png" NONE ALL
IMAGE gueule_patron_upe_neutre_anim  "gfx/gueule_patron_upe_neutre_anim.png" NONE ALL
IMAGE gueule_patron_upe_content_anim "gfx/gueule_patron_upe_content_anim.png" NONE ALL
IMAGE gueule_patron_upe_furieux_anim "gfx/gueule_patron_upe_furieux_anim.png" NONE ALL
IMAGE gueule_patron_upe_gene_anim    "gfx/gueule_patron_upe_gene_anim.png" NONE ALL
IMAGE gueule_huissier                "gfx/gueule_huissier.png" NONE ALL
IMAGE gueule_huissier_content        "gfx/gueule_huissier_content.png" NONE ALL
IMAGE gueule_huissier_furieux        "gfx/gueule_huissier_furieux.png" NONE ALL
IMAGE gueule_huissier_gene           "gfx/gueule_huissier_gene.png" NONE ALL
IMAGE gueule_huissier_neutre_anim    "gfx/gueule_huissier_neutre_anim.png" NONE ALL
IMAGE gueule_huissier_content_anim   "gfx/gueule_huissier_content_anim.png" NONE ALL
IMAGE gueule_huissier_furieux_anim   "gfx/gueule_huissier_furieux_anim.png" NONE ALL
IMAGE gueule_huissier_gene_anim      "gfx/gueule_huissier_gene_anim.png" NONE ALL
