#include "sim/actions.h"
#include "sim/sim.h"

// Référendum et remaniement (V2). Constantes : docs/SIM.md.

#define RFD_BASE     470   // « oui » de base (‰) : un référendum lancé par le président
#define RFD_AFF_K    5     // ‰ par point d'affinité loi/électorat au-dessus de 30
#define RFD_AFF_REF  30
#define RFD_IMM_K    6     // ‰ par point d'opinion immédiat de la loi pour l'électorat
#define RFD_POP_REF  450   // popularité de référence (dixièmes)
#define RFD_POP_K    6     // ‰ par dixième d'écart, sur 10
#define RFD_NOISE    40    // aléa ±4 points

// Orientation moyenne de chaque électorat sur l'axe -100..+100.
static const s8 elec_orient[ELEC_COUNT] = { -30, 0, 40, 20, -40 };

u8 sim_referendum_check(const Country *c, u8 law_id)
{
    if (!law_get(law_id)) return RFD_ERR_ID;
    if (c->status != STATUS_PLAYING || sim_mandate_over(c)) return RFD_ERR_STATE;
    if (c->referendum_done) return RFD_ERR_USED;
    if (law_is_adopted(c, law_id)) return RFD_ERR_ADOPTED;
    if (c->n_pending >= SIM_MAX_PENDING) return RFD_ERR_FULL;
    if (c->cp < REFERENDUM_CP) return RFD_ERR_CP;
    return RFD_OK;
}

// Effet immédiat de la loi sur l'opinion d'un électorat (points).
static s16 immediate_for(const LawDef *law, u8 e)
{
    s16 s = 0;
    u8 i;
    for (i = 0; i < law->n_immediate; i++) {
        u8 k = law->immediate[i].key;
        if (k == EFF_POP_TOUS || sim_effect_elec(k) == e) s = (s16)(s + law->immediate[i].value);
    }
    return s;
}

static void ref_compute(const Country *c, const LawDef *law, s16 noise, RefResult *out)
{
    s32 tot = 0;
    u8 e;
    s32 trust = ((s32)sim_popularity(c) - RFD_POP_REF) * RFD_POP_K / 10;
    for (e = 0; e < ELEC_COUNT; e++) {
        s32 aff = parl_affinity(law->orientation, elec_orient[e]);
        s32 y = RFD_BASE + (aff - RFD_AFF_REF) * RFD_AFF_K
              + (s32)immediate_for(law, e) * RFD_IMM_K + trust + noise;
        out->yes_elec[e] = sim_clamp(y, 50, 950);
    }
    // mêmes poids que la popularité (elec_weight)
    tot = 0;
    for (e = 0; e < ELEC_COUNT; e++) tot += (s32)elec_weight[e] * out->yes_elec[e];
    out->yes = (s16)((tot + 50) / 100);
    out->adopted = (u8)(out->yes >= 500);
}

void sim_referendum_preview(const Country *c, u8 law_id, RefResult *out)
{
    const LawDef *law = law_get(law_id);
    if (!law) {
        u8 e;
        for (e = 0; e < ELEC_COUNT; e++) out->yes_elec[e] = 0;
        out->yes = 0;
        out->adopted = 0;
        return;
    }
    ref_compute(c, law, 0, out);
}

u8 sim_referendum(Country *c, u8 law_id, RefResult *out)
{
    RefResult tmp;
    RefResult *r = out ? out : &tmp;
    u8 err = sim_referendum_check(c, law_id);
    if (err != RFD_OK) return err;
    c->cp = (u8)(c->cp - REFERENDUM_CP);
    c->referendum_done = 1;
    ref_compute(c, law_get(law_id), rng_range(&c->rng, -RFD_NOISE, RFD_NOISE), r);
    if (r->adopted) {
        sim_law_adopt(c, law_id, 0);   // sans vote du Parlement
        sim_apply_effect(c, EFF_POP_TOUS, REFERENDUM_POP_WIN);
        sim_apply_effect(c, EFF_DEFIANCE, -REFERENDUM_DEF_WIN);
    } else {
        sim_apply_effect(c, EFF_POP_TOUS, -REFERENDUM_POP_LOSS);
        sim_apply_effect(c, EFF_DEFIANCE, REFERENDUM_DEF_LOSS);
    }
    return RFD_OK;
}

u8 sim_reshuffle_check(const Country *c, u8 pm)
{
    if (c->status != STATUS_PLAYING || sim_mandate_over(c)) return RSH_ERR_STATE;
    if (c->cohab) return RSH_ERR_COHAB;
    if (c->reshuffle_year == sim_year(c) + 1) return RSH_ERR_YEAR;
    if (pm >= PM_COUNT || pm == c->pm) return RSH_ERR_PM;
    if (c->cp < RESHUFFLE_CP) return RSH_ERR_CP;
    return RSH_OK;
}

u8 sim_reshuffle(Country *c, u8 pm)
{
    u8 g, err = sim_reshuffle_check(c, pm);
    if (err != RSH_OK) return err;
    c->cp = (u8)(c->cp - RESHUFFLE_CP);
    c->reshuffle_year = (u8)(sim_year(c) + 1);
    sim_change_pm(c, pm);
    for (g = 0; g < GRP_COUNT; g++)
        c->relation[g] = (u8)sim_clamp((s32)c->relation[g] + RESHUFFLE_REL, 0, RELATION_MAX);
    c->grace_pm = RESHUFFLE_GRACE;
    sim_apply_effect(c, EFF_POP_TOUS, RESHUFFLE_POP);
    return RSH_OK;
}
