#include "sim/laws.h"
#include "sim/effects.h"
#include "sim/cohab.h"

#define E(k, v) { EFF_##k, v }

// Valeurs en unités lisibles (contrat §3). Voir docs/SIM.md pour la logique.
const LawDef law_defs[LOI_COUNT] = {
    // Hausse du SMIC : social, pouvoir d'achat ; un peu de chômage et de marges en moins.
    { LOI_SMIC, 4, 2, -60, 4, 2,
      { E(SOCIAL, 8), E(ECONOMIE, -3), E(CHOMAGE, 2), E(TENSION, -6) },
      { E(POP_POPULAIRES, 4), E(POP_AISEES, -3) }, 0 },
    // Baisse de l'IS : économie et croissance, au prix de la dette et du social.
    { LOI_IS, 4, 3, 50, 6, 2,
      { E(ECONOMIE, 8), E(CROISSANCE, 3), E(CHOMAGE, -2), E(DETTE, 2),
        E(SOCIAL, -3), E(TENSION, 3) },
      { E(POP_AISEES, 4), E(POP_POPULAIRES, -3) }, 0 },
    // Rénovation thermique : écologie, emploi local, coûteuse.
    { LOI_RENOV, 5, 3, -30, 5, 1,
      { E(ECOLOGIE, 10), E(CHOMAGE, -2), E(CROISSANCE, 2), E(DETTE, 2),
        E(SOCIAL, 2) },
      { E(POP_MOYENNES, 2) }, 0 },
    // Recrutement de policiers : sécurité, rassure les classes moyennes.
    { LOI_POLICE, 3, 2, 40, 3, 3,
      { E(SECURITE, 10), E(DETTE, 1), E(SOCIAL, -1) },
      { E(POP_MOYENNES, 2), E(POP_AISEES, 1), E(POP_POPULAIRES, -1) }, 1 },
    // Crédit d'impôt recherche : technologie et économie, lent.
    { LOI_CIR, 4, 4, 20, 4, 1,
      { E(TECHNOLOGIE, 12), E(ECONOMIE, 4), E(CROISSANCE, 2), E(DETTE, 1) },
      { E(POP_AISEES, 1) }, 1 },

    // --- V1 ---
    // Retraites : la dette baisse à terme, la rue gronde tout de suite.
    { LOI_RETRAITES, 6, 4, 40, 4, 3,
      { E(DETTE, -6), E(ECONOMIE, 5), E(CROISSANCE, 1), E(SOCIAL, -5) },
      { E(POP_POPULAIRES, -4), E(POP_MOYENNES, -2), E(TENSION, 8) }, 0 },
    // ISF : recettes et justice fiscale, les aisées et l'investissement boudent.
    { LOI_ISF, 4, 2, -60, 4, 2,
      { E(DETTE, -3), E(SOCIAL, 3), E(ECONOMIE, -3), E(CROISSANCE, -1) },
      { E(POP_POPULAIRES, 3), E(POP_AISEES, -5) }, 0 },
    // Hôpital : social et apaisement, coûteux.
    { LOI_HOPITAL, 5, 3, -30, 4, 2,
      { E(SOCIAL, 8), E(SECURITE, 2), E(TENSION, -4), E(DETTE, 3) },
      { E(POP_POPULAIRES, 2), E(POP_MOYENNES, 2) }, 0 },
    // Enseignants : effets lents sur la technologie et le social.
    { LOI_ECOLE, 4, 4, -20, 4, 1,
      { E(TECHNOLOGIE, 6), E(SOCIAL, 4), E(ECONOMIE, 2), E(DETTE, 3) },
      { E(POP_MOYENNES, 2) }, 0 },
    // Énergie : investissement lourd, écologie et économie à long terme.
    { LOI_ENERGIE, 6, 4, 20, 5, 2,
      { E(ECOLOGIE, 6), E(ECONOMIE, 4), E(TECHNOLOGIE, 4), E(CROISSANCE, 1), E(DETTE, 4) },
      { E(POP_MOYENNES, 1), E(POP_AISEES, 1) }, 1 },
    // Transports : écologie et pouvoir d'achat.
    { LOI_TRANSPORTS, 5, 3, -40, 4, 2,
      { E(ECOLOGIE, 7), E(SOCIAL, 3), E(TENSION, -3), E(DETTE, 3) },
      { E(POP_POPULAIRES, 3), E(POP_MOYENNES, 1) }, 0 },
    // Immigration : sécurité affichée, cohésion sociale en baisse.
    { LOI_IMMIGRATION, 4, 2, 70, 5, 3,
      { E(SECURITE, 6), E(SOCIAL, -4), E(ECONOMIE, -2), E(TENSION, 4), E(INTERNATIONAL, -2) },
      { E(POP_MOYENNES, 2), E(POP_AISEES, 1), E(POP_POPULAIRES, -1) }, 1 },
    // Assurance chômage : retour à l'emploi et économies, au prix du social.
    { LOI_CHOMAGE, 5, 3, 50, 4, 3,
      { E(CHOMAGE, -3), E(ECONOMIE, 3), E(DETTE, -3), E(SOCIAL, -5) },
      { E(POP_POPULAIRES, -3), E(POP_AISEES, 2), E(TENSION, 5) }, 0 },
    // Taxe numérique : petites recettes, la tech grince.
    { LOI_NUMERIQUE, 3, 2, -20, 3, 2,
      { E(DETTE, -3), E(SOCIAL, 2), E(TECHNOLOGIE, -2) },
      { E(POP_POPULAIRES, 2), E(POP_AISEES, -1) }, 0 },
    // Logements sociaux : social et emploi du bâtiment, lent et coûteux.
    { LOI_LOGEMENT, 5, 3, -30, 5, 1,
      { E(SOCIAL, 6), E(CHOMAGE, -1), E(CROISSANCE, 1), E(ECOLOGIE, -1), E(DETTE, 3) },
      { E(POP_POPULAIRES, 3) }, 0 },

    // --- V2 (docs/contenu/lois_v2.md) ---
    // Taxe superprofits
    { LOI_SURPROFITS, 4, 2, -55, 2, 3,
      { E(DETTE, -3), E(ECONOMIE, -4) },
      { E(POP_POPULAIRES, 3), E(POP_AISEES, -3), E(REL_PARTI, 2) }, 0 },
    // Semaine de 4 jours
    { LOI_SEMAINE4J, 5, 3, -60, 3, 2,
      { E(CROISSANCE, -2), E(CHOMAGE, -2), E(SOCIAL, 6) },
      { E(POP_POPULAIRES, 4), E(POP_MOYENNES, 1) }, 0 },
    // Cantine gratuite
    { LOI_CANTINE, 3, 2, -35, 2, 2,
      { E(SOCIAL, 6), E(DETTE, 2) },
      { E(POP_POPULAIRES, 3), E(POP_MOYENNES, 2) }, 0 },
    // Fin des vols courts
    { LOI_VOLS_COURTS, 3, 2, -45, 2, 3,
      { E(ECOLOGIE, 8), E(ECONOMIE, -2) },
      { E(POP_AISEES, -2), E(POP_MOYENNES, 1), E(REL_PARTI, 2) }, 0 },
    // Déserts médicaux
    { LOI_DESERTS_MED, 3, 3, -25, 3, 2,
      { E(SOCIAL, 6), E(TENSION, -2), E(DETTE, 1) },
      { E(POP_POPULAIRES, 2), E(POP_MOYENNES, 2) }, 0 },
    // Référendum citoyen
    { LOI_RIC, 5, 4, -30, 2, 2,
      { E(DEFIANCE, -4), E(TENSION, 2) },
      { E(POP_TOUS, 2), E(REL_PARTI, 1) }, 1 },
    // Proportionnelle
    { LOI_PROPORTIONNELLE, 6, 4, -35, 2, 2,
      { E(DEFIANCE, -3), E(TENSION, -2) },
      { E(POP_TOUS, 1), E(REL_PARTI, -2) }, 1 },
    // Aide au développement
    { LOI_AIDE_DEV, 3, 2, -30, 3, 3,
      { E(DETTE, 2), E(DEFIANCE, -2), E(INTERNATIONAL, 8) },
      { E(POP_POPULAIRES, 1), E(POP_AISEES, -1), E(REL_PARTI, 2) }, 1 },
    // Police de proximité
    { LOI_POLICE_PROX, 3, 3, -30, 3, 1,
      { E(SECURITE, 4), E(TENSION, -4), E(DETTE, 1) },
      { E(POP_POPULAIRES, 2) }, 1 },
    // Encadrement des loyers
    { LOI_ENCADREMENT, 3, 2, -40, 2, 3,
      { E(ECONOMIE, -2), E(SOCIAL, 4) },
      { E(POP_POPULAIRES, 3), E(POP_MOYENNES, 1), E(POP_AISEES, -2) }, 0 },
    // Agriculture bio
    { LOI_BIO_CANTINE, 3, 3, -25, 3, 2,
      { E(ECOLOGIE, 6), E(SOCIAL, 2), E(DETTE, 1) },
      { E(POP_MOYENNES, 1), E(REL_PARTI, 1) }, 0 },
    // Pass culture pour tous
    { LOI_CULTURE, 2, 2, -20, 2, 2,
      { E(TECHNOLOGIE, 2), E(DETTE, 1) },
      { E(POP_MOYENNES, 2), E(POP_POPULAIRES, 1) }, 0 },
    // Flat tax
    { LOI_FLAT_TAX, 4, 2, 60, 3, 2,
      { E(ECONOMIE, 6), E(CROISSANCE, 2), E(DETTE, 2) },
      { E(POP_AISEES, 3), E(POP_POPULAIRES, -3) }, 0 },
    // Licenciement assoupli
    { LOI_LICENCIEMENT, 5, 3, 55, 3, 2,
      { E(CHOMAGE, -3), E(CROISSANCE, 2), E(TENSION, 4) },
      { E(POP_POPULAIRES, -3), E(POP_AISEES, 2) }, 0 },
    // Uniforme à l'école
    { LOI_UNIFORME, 2, 1, 40, 2, 3,
      { E(TENSION, 2), E(SOCIAL, -2) },
      { E(POP_MOYENNES, 2), E(POP_AISEES, 1), E(REL_PARTI, 1) }, 0 },
    // Vidéosurveillance
    { LOI_VIDEO, 3, 2, 60, 4, 1,
      { E(SECURITE, 6), E(TENSION, 3), E(DEFIANCE, 2), E(DETTE, 1) },
      { E(POP_AISEES, 2) }, 1 },
    // Moins de parlementaires
    { LOI_PARLEMENTAIRES, 5, 4, 35, 2, 2,
      { E(DETTE, -1), E(DEFIANCE, -2) },
      { E(POP_TOUS, 1), E(REL_PARTI, -3) }, 1 },
    // Loi de programmation
    { LOI_LPM, 4, 4, 50, 4, 2,
      { E(SECURITE, 6), E(TECHNOLOGIE, 4), E(DETTE, 3), E(INTERNATIONAL, 3) },
      { E(POP_AISEES, 1), E(REL_PARTI, 2) }, 1 },
    // Service national
    { LOI_SERVICE_MIL, 5, 4, 55, 4, 2,
      { E(SECURITE, 4), E(SOCIAL, 2), E(DETTE, 2), E(INTERNATIONAL, 2) },
      { E(POP_MOYENNES, 1), E(POP_POPULAIRES, -1) }, 1 },
    // Alliance militaire
    { LOI_ALLIANCE, 3, 2, 30, 4, 2,
      { E(SECURITE, 4), E(DEFIANCE, 1), E(ECONOMIE, 2), E(INTERNATIONAL, 6) },
      { E(POP_AISEES, 1), E(REL_PARTI, 1) }, 1 },
    // Exportations d'armes
    { LOI_EXPORT_ARMES, 3, 2, 45, 5, 1,
      { E(ECONOMIE, 4), E(CROISSANCE, 1), E(DEFIANCE, 2), E(DETTE, -1), E(INTERNATIONAL, -4) },
      { E(POP_POPULAIRES, -1) }, 1 },
    // Agriculture productive
    { LOI_AGRI_PRODUCT, 3, 2, 35, 4, 1,
      { E(ECONOMIE, 4), E(CROISSANCE, 1), E(ECOLOGIE, -6), E(TENSION, 1) },
      { E(POP_MOYENNES, 1) }, 0 },
    // Taxe carbone
    { LOI_CARBONE, 4, 2, -5, 4, 2,
      { E(ECOLOGIE, 6), E(DETTE, -1), E(TENSION, 3), E(INTERNATIONAL, 2) },
      { E(POP_POPULAIRES, -2), E(POP_AISEES, -1) }, 0 },
    // Cybersécurité nationale
    { LOI_CYBER, 2, 2, 10, 3, 1,
      { E(TECHNOLOGIE, 6), E(SECURITE, 2), E(DETTE, 1) },
      { E(POP_MOYENNES, 1) }, 0 },
    // Peines planchers
    { LOI_PEINES_PLANCHERS, 4, 2, 65, 3, 3,
      { E(SECURITE, 6), E(TENSION, 3), E(DETTE, 1) },
      { E(POP_AISEES, 2), E(POP_POPULAIRES, 1), E(REL_PARTI, 1) }, 1 },
};

u8 law_cost(const Country *c, const LawDef *law)
{
    u8 cost = law->cost;
    if (c->pm == PM_REFORMATEUR && cost > 1) cost--;
    return cost;
}

const LawDef *law_get(u8 id)
{
    return id < LOI_COUNT ? &law_defs[id] : 0;
}

u8 law_delay_turns(const Country *c, const LawDef *law)
{
    // turns_per_year / 4 tours par trimestre (1 en V0, 3 en mode 60 tours).
    u16 t = (u16)law->delay_q * (c->turns_per_year >> 2);
    return (u8)(t ? t : 1);
}

u8 law_is_adopted(const Country *c, u8 id)
{
    return law_mask_get(&c->laws_adopted, id);
}

// Tours restants avant l'application d'une loi votée (0 = pas en attente).
u8 law_pending_turns(const Country *c, u8 id)
{
    for (u8 i = 0; i < c->n_pending; i++)
        if (c->pending[i].law == id) return c->pending[i].turns_left;
    return 0;
}

u8 law_can_propose(const Country *c, u8 id)
{
    const LawDef *law = law_get(id);
    if (!law) return LAW_ERR_ID;
    if (c->status != STATUS_PLAYING) return LAW_ERR_STATE;
    if (law_is_adopted(c, id)) return LAW_ERR_ADOPTED;
    if (c->cohab && !sim_law_sovereign(id)) return LAW_ERR_COHAB;
    if (c->cp < law_cost(c, law)) return LAW_ERR_CP;
    if (c->n_pending >= SIM_MAX_PENDING) return LAW_ERR_FULL;
    return LAW_OK;
}

u8 law_process_pending(Country *c, u8 *out)
{
    u8 i = 0, n = 0;
    while (i < c->n_pending) {
        PendingLaw *p = &c->pending[i];
        if (p->turns_left > 1) {
            p->turns_left--;
            i++;
            continue;
        }
        {
            const LawDef *law = law_get(p->law);
            if (law) sim_apply_effects(c, law->effects, law->n_effects);
            out[n++] = p->law;
        }
        // retrait en conservant l'ordre
        {
            u8 j;
            for (j = i; j + 1 < c->n_pending; j++)
                c->pending[j] = c->pending[j + 1];
            c->n_pending--;
        }
    }
    return n;
}
