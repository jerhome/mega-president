#include "sim/budget.h"
#include "sim/sim.h"

// Budget annuel détaillé (V4). Tables et formules : docs/SIM.md « Budget détaillé ».

// Effets d'UN cran (+1) de chaque ligne, en ROM.
typedef struct {
    s8 deficit;                // dixièmes de point de PIB par an
    s8 growth;                 // centièmes de point
    s8 tension;                // points de jauge
    s8 orient;                 // orientation de vote
    s8 opinion[ELEC_COUNT];    // dixièmes de % : Pop Moy Cad Ret Jeu
    s8 pillar[PIL_TOTAL];      // dixièmes par trimestre : Soc Éco Tech Écol Séc Intl
} BudLine;

static const BudLine bud_line[BUD_LINES] = {
    /* Social et retraites */  {  5,   5, -2, -15, { 10,   3,  -3, 10,  3 }, { 3, 0, 0, 0, 0, 0 } },
    /* Éducation, recherche */ {  3,   5,  0, -10, {  2,   5,   3,  0, 10 }, { 1, 0, 3, 0, 0, 0 } },
    /* Sécurité et défense */  {  3,   0,  0,  15, {  2,   3,   2,  8, -3 }, { 0, 0, 0, 0, 3, 1 } },
    /* Transition, réseaux */  {  4,  10, -1, -10, {  0,   3,   3, -2,  8 }, { 0, 1, 0, 3, 0, 0 } },
    /* Impôts des ménages */   { -6, -10,  2,   0, { -6, -10,  -8, -6, -3 }, { 0, 0, 0, 0, 0, 0 } },
    /* Impôts entreprises */   { -5, -15, -1, -20, {  3,   0, -10, -2,  2 }, { 0, -3, 0, 0, 0, 0 } },
};

// Multiplicateur asymétrique des crans -2..+2 (puis division par 2) : rendement
// décroissant en hausse, coupe franche très coûteuse.
static const s8 bud_mult[5] = { -5, -2, 0, 2, 3 };
// Impôts : asymétrie inversée (une forte hausse fait très mal, une baisse rapporte
// peu), sinon baisser les impôts serait toujours dominant (équilibrage V4).
static const s8 bud_mult_tax[5] = { -3, -2, 0, 2, 5 };

// Sérieux budgétaire : sensibilité de chaque électorat au déficit (x deficit / 10).
static const u8 bud_k_serious[ELEC_COUNT] = { 0, 3, 10, 6, 0 };

// Préréglages : Soc Édu Séc Trans Mén Ent
static const s8 bud_preset[BUDGET_COUNT][BUD_LINES] = {
    { -1, 0, 0, -1,  1, 0 },   // Rigueur
    {  0, 0, 0,  0,  0, 0 },   // Équilibre
    {  1, 1, 0,  1,  0, 0 },   // Relance
};

// Plans imposés par le Premier ministre d'opposition selon son camp.
static const s8 bud_cohab[ORIENT_COUNT][BUD_LINES] = {
    {  1, 1, 0,  1, 0,  1 },   // gauche
    {  0, 0, 0,  0, 0,  0 },   // centre
    { -1, 0, 1, -1, 0, -1 },   // droite
};

#define PILLAR_RECALL 12        // V4.14 : rappel des piliers vers 50 (1/32 de l'écart par trimestre)
#define MARKET_DIV 7            // V4.14 : malus (dixièmes) = déficit² / 16, déficit en dixièmes de point de PIB
#define INSINCERE_DEFICIT 15    // au-delà (1,5 point), le budget est « insincère »

static s8 lvl_ok(s8 l) { return (s8)(l < BUD_LVL_MIN ? BUD_LVL_MIN : (l > BUD_LVL_MAX ? BUD_LVL_MAX : l)); }

u8 sim_budget_due(const Country *c)
{
    if (c->status != STATUS_PLAYING || c->budget_done) return 0;
    if (sim_mandate_over(c)) return 0;
    if (c->cohab) return 0;   // le budget est décidé par le Premier ministre d'opposition
    return (u8)(sim_quarter(c) == 3);
}

// --- Plans ---
void sim_budget_preset_plan(u8 preset, BudgetPlan *out)
{
    u8 i;
    if (preset >= BUDGET_COUNT) preset = BUDGET_EQUILIBRE;
    for (i = 0; i < BUD_LINES; i++) out->lvl[i] = bud_preset[preset][i];
}

u8 sim_budget_plan_preset(const BudgetPlan *p)
{
    u8 b, i;
    for (b = 0; b < BUDGET_COUNT; b++) {
        for (i = 0; i < BUD_LINES; i++)
            if (p->lvl[i] != bud_preset[b][i]) break;
        if (i == BUD_LINES) return b;
    }
    return BUDGET_PERSO;
}

void sim_budget_current_plan(const Country *c, BudgetPlan *out)
{
    u8 i;
    for (i = 0; i < BUD_LINES; i++) out->lvl[i] = c->budget_lvl[i];
}

void sim_budget_plan_clamp(BudgetPlan *p)
{
    u8 i;
    for (i = 0; i < BUD_LINES; i++) p->lvl[i] = lvl_ok(p->lvl[i]);
}

void budget_set_current(Country *c, const BudgetPlan *p)
{
    u8 i;
    BudgetPlan q = *p;
    sim_budget_plan_clamp(&q);
    for (i = 0; i < BUD_LINES; i++) c->budget_lvl[i] = q.lvl[i];
    c->budget = sim_budget_plan_preset(&q);
}

// --- Aperçus ---
s8 sim_plan_orientation(const Country *c, const BudgetPlan *p)
{
    s32 o = sim_player_orientation(c) / 2;
    u8 i;
    for (i = 0; i < BUD_LINES; i++) o += (s32)lvl_ok(p->lvl[i]) * bud_line[i].orient;
    return (s8)sim_clamp(o, -100, 100);
}

s16 sim_plan_deficit(const BudgetPlan *p)
{
    s32 d = 0;
    u8 i;
    for (i = 0; i < BUD_LINES; i++) d += (s32)lvl_ok(p->lvl[i]) * bud_line[i].deficit;
    return (s16)d;
}

s16 sim_plan_growth(const BudgetPlan *p)
{
    s32 g = 0;
    u8 i;
    for (i = 0; i < BUD_LINES; i++) g += (s32)lvl_ok(p->lvl[i]) * bud_line[i].growth;
    return (s16)g;
}

s16 sim_plan_tension(const BudgetPlan *p)
{
    s32 t = 0;
    u8 i;
    for (i = 0; i < BUD_LINES; i++) t += (s32)lvl_ok(p->lvl[i]) * bud_line[i].tension;
    return (s16)t;
}

// Valeur d'un cran multipliée par 2 (le /2 est fait par l'appelant).
static s32 asym(u8 line, s8 lvl, s32 v)
{
    const s8 *m = line >= BUD_IMPOT_MENAGES ? bud_mult_tax : bud_mult;
    return (s32)m[lvl_ok(lvl) - BUD_LVL_MIN] * v;
}

s16 sim_budget_line_opinion(u8 line, s8 lvl, u8 e)
{
    if (line >= BUD_LINES || e >= ELEC_COUNT) return 0;
    return (s16)(asym(line, lvl, bud_line[line].opinion[e]) / 2);
}

s16 sim_plan_opinion(const BudgetPlan *p, u8 e)
{
    s32 t = 0;
    u8 i;
    if (e >= ELEC_COUNT) return 0;
    for (i = 0; i < BUD_LINES; i++) t += asym(i, p->lvl[i], bud_line[i].opinion[e]);
    t /= 2;
    t -= (s32)sim_plan_deficit(p) * bud_k_serious[e] / 10;   // sérieux budgétaire
    {   // V4.14 : défiance des marchés, coût convexe du déficit (tous les électorats)
        s32 d = sim_plan_deficit(p);
        if (d > 0) t -= d * d / MARKET_DIV;
    }
    return (s16)t;
}

s16 sim_plan_pillar(const BudgetPlan *p, u8 pil)
{
    s32 t = 0;
    u8 i;
    if (pil >= PIL_TOTAL) return 0;
    for (i = 0; i < BUD_LINES; i++) t += asym(i, p->lvl[i], bud_line[i].pillar[pil]);
    return (s16)(t / 2);
}

s16 sim_plan_insincere_malus(const BudgetPlan *p)
{
    s16 d = sim_plan_deficit(p);
    return (s16)(d > INSINCERE_DEFICIT ? (d - INSINCERE_DEFICIT) / 2 : 0);
}

// Vote : le même parl_vote que les lois, avec le malus d'insincérité retiré aux
// groupes hors du parti du joueur.
static void plan_vote(const Country *c, const BudgetPlan *p, VoteResult *out)
{
    s16 malus = sim_plan_insincere_malus(p);
    s8 o = sim_plan_orientation(c, p);
    u8 g;
    if (!malus) { parl_vote(c, o, out); return; }
    out->votes_pour = 0;
    out->votes_contre = 0;
    for (g = 0; g < GRP_COUNT; g++) {
        s16 s = parl_group_score(c, g, o);
        if (g != c->player_group) s = (s16)(s - malus);
        out->score[g] = s;
        out->pour[g] = (u8)(s >= parl_threshold(c, g));
        if (out->pour[g]) out->votes_pour = (u16)(out->votes_pour + c->seats[g]);
        else out->votes_contre = (u16)(out->votes_contre + c->seats[g]);
    }
    out->adopted = (u8)(out->votes_pour >= SEATS_MAJORITY);
}

void sim_budget_plan_preview(const Country *c, const BudgetPlan *p, VoteResult *out)
{
    plan_vote(c, p, out);
}

// --- Sénat ---
// Ligne la plus éloignée de l'orientation centre-droit du Sénat : celle dont
// la contribution (cran x orientation de la ligne) est la plus négative.
static u8 amend_line_of(const BudgetPlan *p)
{
    s32 worst = 0;
    u8 i, best = 0;
    for (i = 0; i < BUD_LINES; i++) {
        s32 v = (s32)lvl_ok(p->lvl[i]) * bud_line[i].orient;
        if (v < worst) { worst = v; best = (u8)(i + 1); }
    }
    return best;
}

u8 sim_budget_senate_amend_line(const Country *c, const BudgetPlan *p)
{
    if (c->cohab || sim_senate_has_majority(c)) return 0;
    return amend_line_of(p);
}

// Fait reculer d'un cran (vers 0) la ligne amendée.
static void amend_apply(Country *c, BudgetPlan *p, u8 line1)
{
    u8 i = (u8)(line1 - 1);
    c->bud_amend_line = line1;
    c->bud_amend_from = p->lvl[i];
    p->lvl[i] = (s8)(p->lvl[i] + (p->lvl[i] > 0 ? -1 : 1));
}

// --- Compatibilité : préréglages ---
s8 sim_budget_orientation(u8 b)
{
    BudgetPlan p;
    s32 o = 0;
    u8 i;
    sim_budget_preset_plan(b, &p);
    for (i = 0; i < BUD_LINES; i++) o += (s32)p.lvl[i] * bud_line[i].orient;
    return (s8)o;
}
s16 sim_budget_deficit(u8 b) { BudgetPlan p; sim_budget_preset_plan(b, &p); return sim_plan_deficit(&p); }
s16 sim_budget_growth(u8 b)  { BudgetPlan p; sim_budget_preset_plan(b, &p); return sim_plan_growth(&p); }
s16 sim_budget_tension(u8 b) { BudgetPlan p; sim_budget_preset_plan(b, &p); return sim_plan_tension(&p); }
s16 sim_budget_opinion(u8 b, u8 e) { BudgetPlan p; sim_budget_preset_plan(b, &p); return sim_plan_opinion(&p, e); }
s16 sim_budget_pillar(u8 b, u8 pil) { BudgetPlan p; sim_budget_preset_plan(b, &p); return sim_plan_pillar(&p, pil); }

void sim_budget_preview(const Country *c, u8 b, VoteResult *out)
{
    BudgetPlan p;
    sim_budget_preset_plan(b, &p);
    plan_vote(c, &p, out);
}

// --- Actions ---
static void budget_adopt(Country *c, const BudgetPlan *p)
{
    BudgetPlan q = *p;
    u8 am;
    sim_budget_plan_clamp(&q);
    c->bud_amend_line = 0;
    c->bud_amend_from = 0;
    am = sim_budget_senate_amend_line(c, &q);
    if (am) amend_apply(c, &q, am);
    budget_set_current(c, &q);
    c->budget_done = 1;
}

u8 sim_budget_plan_vote(Country *c, const BudgetPlan *p, VoteResult *out)
{
    if (!sim_budget_due(c)) return BUDGET_ERR_NOT_DUE;
    plan_vote(c, p, out);
    if (out->adopted) budget_adopt(c, p);
    else sim_apply_effect(c, EFF_DEFIANCE, 3);
    return BUDGET_OK;
}

u8 sim_budget_plan_force(Country *c, const BudgetPlan *p)
{
    u8 g;
    if (!sim_budget_due(c)) return BUDGET_ERR_NOT_DUE;
    if (c->cp < BUDGET_493_CP) return BUDGET_ERR_CP;
    c->cp = (u8)(c->cp - BUDGET_493_CP);
    budget_adopt(c, p);
    sim_apply_effect(c, EFF_DEFIANCE, 8);
    for (g = 0; g < GRP_COUNT; g++)
        if (g != c->player_group)
            c->relation[g] = (u8)sim_clamp((s32)c->relation[g] - 1, 0, RELATION_MAX);
    return BUDGET_OK;
}

u8 sim_budget_vote(Country *c, u8 b, VoteResult *out)
{
    BudgetPlan p;
    if (b >= BUDGET_COUNT) return BUDGET_ERR_ID;
    sim_budget_preset_plan(b, &p);
    return sim_budget_plan_vote(c, &p, out);
}

u8 sim_budget_force(Country *c, u8 b)
{
    BudgetPlan p;
    if (b >= BUDGET_COUNT) return BUDGET_ERR_ID;
    sim_budget_preset_plan(b, &p);
    return sim_budget_plan_force(c, &p);
}

u8 sim_budget_amended_line(const Country *c) { return c->bud_amend_line; }

u8 sim_budget_definitive_reading(Country *c)
{
    BudgetPlan p;
    if (!c->bud_amend_line || !c->budget_done) return BUDGET_ERR_NO_AMEND;
    if (c->cp < BUDGET_SENATE_CP) return BUDGET_ERR_CP;
    c->cp = (u8)(c->cp - BUDGET_SENATE_CP);
    sim_budget_current_plan(c, &p);
    p.lvl[c->bud_amend_line - 1] = c->bud_amend_from;
    budget_set_current(c, &p);
    c->bud_amend_line = 0;
    c->bud_amend_from = 0;
    return BUDGET_OK;
}

// --- Reconduction, cohabitation ---
void sim_budget_reconduct_plan(BudgetPlan *p)
{
    u8 i;
    for (i = 0; i < BUD_IMPOT_MENAGES; i++)   // dépenses seulement
        if (p->lvl[i] != 0) p->lvl[i] = (s8)(p->lvl[i] + (p->lvl[i] > 0 ? -1 : 1));
}

void sim_cohab_plan(const Country *c, BudgetPlan *out)
{
    u8 camp = c->cohab ? sim_cohab_camp(c) : ORIENT_CENTRE;
    s8 a = c->cohab_def_adj;
    u8 i;
    for (i = 0; i < BUD_LINES; i++) out->lvl[i] = bud_cohab[camp][i];
    if (a < -1) a = -1;
    if (a > 1) a = 1;
    out->lvl[BUD_SECURITE] = lvl_ok((s8)(out->lvl[BUD_SECURITE] + a));
}

void sim_cohab_set_defense(Country *c, s8 adjust)
{
    c->cohab_def_adj = (s8)(adjust < -1 ? -1 : (adjust > 1 ? 1 : adjust));
}

u8 budget_year_close(Country *c, u8 *reconduit, u8 *gov)
{
    u8 amended = c->bud_amend_line;
    BudgetPlan p;
    *reconduit = 0;
    *gov = 0;
    if (c->cohab) {
        sim_cohab_plan(c, &p);
        budget_set_current(c, &p);
        *gov = 1;
    } else if (!c->budget_done) {
        sim_budget_current_plan(c, &p);
        sim_budget_reconduct_plan(&p);
        budget_set_current(c, &p);
        *reconduit = 1;
    }
    c->budget_done = 0;
    c->bud_amend_line = 0;
    c->bud_amend_from = 0;
    return amended;
}

void budget_quarter_update(Country *c)
{
    BudgetPlan p;
    u8 i;
    sim_budget_current_plan(c, &p);
    for (i = 0; i < PIL_TOTAL; i++)
        c->pillars[i] = sim_clamp((s32)c->pillars[i] + sim_plan_pillar(&p, i)
                                  + (500 - (s32)c->pillars[i]) / PILLAR_RECALL,
                                  PILLAR_MIN, PILLAR_MAX);
}
