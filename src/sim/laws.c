#include "sim/laws.h"
#include "sim/effects.h"

#define E(k, v) { EFF_##k, v }

// Valeurs en unités lisibles (contrat §3). Voir docs/SIM.md pour la logique.
const LawDef law_defs[LOI_COUNT] = {
    // Hausse du SMIC : social, pouvoir d'achat ; un peu de chômage et de marges en moins.
    { LOI_SMIC, 4, 2, -60, 4, 2,
      { E(SOCIAL, 8), E(ECONOMIE, -3), E(CHOMAGE, 2), E(TENSION, -6) },
      { E(POP_POPULAIRES, 4), E(POP_AISEES, -3) } },
    // Baisse de l'IS : économie et croissance, au prix de la dette et du social.
    { LOI_IS, 4, 3, 50, 6, 2,
      { E(ECONOMIE, 8), E(CROISSANCE, 3), E(CHOMAGE, -2), E(DETTE, 2),
        E(SOCIAL, -3), E(TENSION, 3) },
      { E(POP_AISEES, 4), E(POP_POPULAIRES, -3) } },
    // Rénovation thermique : écologie, emploi local, coûteuse.
    { LOI_RENOV, 5, 3, -30, 5, 1,
      { E(ECOLOGIE, 10), E(CHOMAGE, -2), E(CROISSANCE, 2), E(DETTE, 2),
        E(SOCIAL, 2) },
      { E(POP_MOYENNES, 2) } },
    // Recrutement de policiers : sécurité, rassure les classes moyennes.
    { LOI_POLICE, 3, 2, 40, 3, 3,
      { E(SECURITE, 10), E(DETTE, 1), E(SOCIAL, -1) },
      { E(POP_MOYENNES, 2), E(POP_AISEES, 1), E(POP_POPULAIRES, -1) } },
    // Crédit d'impôt recherche : technologie et économie, lent.
    { LOI_CIR, 4, 4, 20, 4, 1,
      { E(TECHNOLOGIE, 12), E(ECONOMIE, 4), E(CROISSANCE, 2), E(DETTE, 1) },
      { E(POP_AISEES, 1) } },

    // --- V1 ---
    // Retraites : la dette baisse à terme, la rue gronde tout de suite.
    { LOI_RETRAITES, 6, 4, 40, 4, 3,
      { E(DETTE, -6), E(ECONOMIE, 5), E(CROISSANCE, 1), E(SOCIAL, -5) },
      { E(POP_POPULAIRES, -4), E(POP_MOYENNES, -2), E(TENSION, 8) } },
    // ISF : recettes et justice fiscale, les aisées et l'investissement boudent.
    { LOI_ISF, 4, 2, -60, 4, 2,
      { E(DETTE, -3), E(SOCIAL, 3), E(ECONOMIE, -3), E(CROISSANCE, -1) },
      { E(POP_POPULAIRES, 3), E(POP_AISEES, -5) } },
    // Hôpital : social et apaisement, coûteux.
    { LOI_HOPITAL, 5, 3, -30, 4, 2,
      { E(SOCIAL, 8), E(SECURITE, 2), E(TENSION, -4), E(DETTE, 3) },
      { E(POP_POPULAIRES, 2), E(POP_MOYENNES, 2) } },
    // Enseignants : effets lents sur la technologie et le social.
    { LOI_ECOLE, 4, 4, -20, 4, 1,
      { E(TECHNOLOGIE, 6), E(SOCIAL, 4), E(ECONOMIE, 2), E(DETTE, 3) },
      { E(POP_MOYENNES, 2) } },
    // Énergie : investissement lourd, écologie et économie à long terme.
    { LOI_ENERGIE, 6, 4, 20, 5, 2,
      { E(ECOLOGIE, 6), E(ECONOMIE, 4), E(TECHNOLOGIE, 4), E(CROISSANCE, 1), E(DETTE, 4) },
      { E(POP_MOYENNES, 1), E(POP_AISEES, 1) } },
    // Transports : écologie et pouvoir d'achat.
    { LOI_TRANSPORTS, 5, 3, -40, 4, 2,
      { E(ECOLOGIE, 7), E(SOCIAL, 3), E(TENSION, -3), E(DETTE, 3) },
      { E(POP_POPULAIRES, 3), E(POP_MOYENNES, 1) } },
    // Immigration : sécurité affichée, cohésion sociale en baisse.
    { LOI_IMMIGRATION, 4, 2, 70, 4, 3,
      { E(SECURITE, 6), E(SOCIAL, -4), E(ECONOMIE, -2), E(TENSION, 4) },
      { E(POP_MOYENNES, 2), E(POP_AISEES, 1), E(POP_POPULAIRES, -1) } },
    // Assurance chômage : retour à l'emploi et économies, au prix du social.
    { LOI_CHOMAGE, 5, 3, 50, 4, 3,
      { E(CHOMAGE, -3), E(ECONOMIE, 3), E(DETTE, -3), E(SOCIAL, -5) },
      { E(POP_POPULAIRES, -3), E(POP_AISEES, 2), E(TENSION, 5) } },
    // Taxe numérique : petites recettes, la tech grince.
    { LOI_NUMERIQUE, 3, 2, -20, 3, 2,
      { E(DETTE, -3), E(SOCIAL, 2), E(TECHNOLOGIE, -2) },
      { E(POP_POPULAIRES, 2), E(POP_AISEES, -1) } },
    // Logements sociaux : social et emploi du bâtiment, lent et coûteux.
    { LOI_LOGEMENT, 5, 3, -30, 5, 1,
      { E(SOCIAL, 6), E(CHOMAGE, -1), E(CROISSANCE, 1), E(ECOLOGIE, -1), E(DETTE, 3) },
      { E(POP_POPULAIRES, 3) } },
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
    return (u8)((c->laws_adopted >> id) & 1);
}

u8 law_can_propose(const Country *c, u8 id)
{
    const LawDef *law = law_get(id);
    if (!law) return LAW_ERR_ID;
    if (c->status != STATUS_PLAYING) return LAW_ERR_STATE;
    if (law_is_adopted(c, id)) return LAW_ERR_ADOPTED;
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
