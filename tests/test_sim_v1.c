#include "test.h"
#include "sim/sim.h"
#include "sim/events.h"

// Tests des règles V1 (1.02 à 1.11) et parties automatiques (1.20).

static int v1_in_bounds(const Country *c)
{
    int i, ok = 1;
    u16 seats = 0;
    for (i = 0; i < ELEC_COUNT; i++)
        ok &= c->opinion[i] >= OPINION_MIN && c->opinion[i] <= OPINION_MAX;
    for (i = 0; i < PIL_TOTAL; i++)
        ok &= c->pillars[i] >= PILLAR_MIN && c->pillars[i] <= PILLAR_MAX;
    for (i = 0; i < JAUGE_COUNT; i++) ok &= c->gauges[i] <= GAUGE_MAX;
    for (i = 0; i < GRP_COUNT; i++) { ok &= c->relation[i] <= RELATION_MAX; seats += c->seats[i]; }
    ok &= seats == SEATS_TOTAL;
    ok &= c->growth >= GROWTH_MIN && c->growth <= GROWTH_MAX;
    ok &= c->unemployment >= UNEMP_MIN && c->unemployment <= UNEMP_MAX;
    ok &= c->debt >= DEBT_MIN && c->debt <= DEBT_MAX;
    ok &= c->cp <= CP_MAX;
    ok &= c->n_pending <= SIM_MAX_PENDING;
    ok &= c->budget <= BUDGET_PERSO;
    ok &= c->status < STATUS_COUNT;
    return ok;
}

static void setup(SimSetup *s, u8 orient, u8 pm, u8 diff, u32 seed)
{
    s->orientation = orient;
    s->pm = pm;
    s->difficulty = diff;
    s->turns_per_year = 4;
    s->seed = seed;
}

static void test_v1_init(void)
{
    Country c;
    SimSetup s;
    int d, o, g;

    for (d = 0; d < DIFF_COUNT; d++)
        for (o = 0; o < ORIENT_COUNT; o++) {
            setup(&s, (u8)o, PM_NONE, (u8)d, 11);
            sim_init_ex(&c, &s);
            CHECK(v1_in_bounds(&c));
            CHECK_EQ(c.difficulty, d);
            if (d == DIFF_DIFFICILE) CHECK(!sim_has_majority(&c));
            else CHECK(sim_has_majority(&c));
        }
    {
        Country f, n, h;
        setup(&s, ORIENT_GAUCHE, PM_NONE, DIFF_FACILE, 5);    sim_init_ex(&f, &s);
        setup(&s, ORIENT_GAUCHE, PM_NONE, DIFF_NORMAL, 5);    sim_init_ex(&n, &s);
        setup(&s, ORIENT_GAUCHE, PM_NONE, DIFF_DIFFICILE, 5); sim_init_ex(&h, &s);
        CHECK(f.debt < n.debt && n.debt < h.debt);
        CHECK(f.unemployment < n.unemployment && n.unemployment < h.unemployment);
        CHECK(f.growth > h.growth);
        CHECK(f.relation[GRP_G4] > h.relation[GRP_G4]);
        CHECK(f.cp > h.cp);
    }

    // Premiers ministres
    {
        Country a, b;
        setup(&s, ORIENT_CENTRE, PM_NONE, DIFF_NORMAL, 3);        sim_init_ex(&a, &s);
        setup(&s, ORIENT_CENTRE, PM_RASSEMBLEUR, DIFF_NORMAL, 3); sim_init_ex(&b, &s);
        for (g = 0; g < GRP_COUNT; g++) CHECK_EQ(b.relation[g], a.relation[g] + 5);
        setup(&s, ORIENT_CENTRE, PM_REFORMATEUR, DIFF_NORMAL, 3); sim_init_ex(&b, &s);
        CHECK_EQ(sim_law_cost(&b, LOI_RENOV), law_get(LOI_RENOV)->cost - 1);
        CHECK_EQ(sim_law_cost(&a, LOI_RENOV), law_get(LOI_RENOV)->cost);
        setup(&s, ORIENT_CENTRE, PM_TECHNOCRATE, DIFF_NORMAL, 3); sim_init_ex(&b, &s);
        CHECK_EQ(sim_opinion_target(&b, ELEC_MOYENNES), sim_opinion_target(&a, ELEC_MOYENNES) - 25);
        CHECK(!sim_change_pm(&b, PM_TECHNOCRATE));
        CHECK(sim_change_pm(&b, PM_REFORMATEUR));
        CHECK_EQ(b.pm, PM_REFORMATEUR);
        CHECK(!sim_change_pm(&b, PM_COUNT));
    }

    // Catalogue : 15 lois, coûts et délais dans les bornes
    CHECK_EQ(LOI_COUNT, 40);
    CHECK_EQ(LOI_RETRAITES, 5);
    CHECK_EQ(LOI_LOGEMENT, 14);
    CHECK_EQ(law_get(LOI_IMMIGRATION)->orientation, 70);
    CHECK_EQ(law_get(LOI_ISF)->orientation, -60);

    // Effet REL_PARTI : relation avec le parti du joueur
    setup(&s, ORIENT_DROITE, PM_NONE, DIFF_NORMAL, 3);
    sim_init_ex(&c, &s);
    sim_apply_effect(&c, EFF_REL_PARTI, -7);
    CHECK_EQ(c.relation[GRP_G4], 73);
    CHECK_EQ(sim_effect_scale(EFF_REL_PARTI), 1);
}

static void test_v1_gauges(void)
{
    Country c;
    s16 t0;
    sim_init(&c, ORIENT_CENTRE, 21, 4);
    t0 = sim_opinion_target(&c, ELEC_POPULAIRES);
    c.gauges[JAUGE_SCANDALE] = 50;
    CHECK_EQ(sim_opinion_target(&c, ELEC_POPULAIRES), t0 - 60);
    c.gauges[JAUGE_DEFIANCE] = 70;
    CHECK_EQ(sim_opinion_target(&c, ELEC_POPULAIRES), t0 - 60 - 20);
    // Décroissance lente : scandale -1 par trimestre ; la défiance tend de
    // 2 points par trimestre vers 10 + scandale × 3/4.
    c.gauges[JAUGE_SCANDALE] = 50;
    c.gauges[JAUGE_DEFIANCE] = 40;
    sim_end_turn(&c, 0);
    CHECK(c.gauges[JAUGE_SCANDALE] == 49 || c.gauges[JAUGE_SCANDALE] >= 49 + 5);   // affaire possible
    CHECK_EQ(c.gauges[JAUGE_DEFIANCE], 42);
    c.gauges[JAUGE_SCANDALE] = 0;
    c.gauges[JAUGE_DEFIANCE] = 40;
    sim_end_turn(&c, 0);
    CHECK_EQ(c.gauges[JAUGE_DEFIANCE], 38);
}

static void test_v1_493_tv(void)
{
    Country c;
    VoteResult v;
    u8 d0, i, flops = 0;
    sim_init(&c, ORIENT_DROITE, 9, 4);
    c.cp = 15;
    d0 = c.gauges[JAUGE_DEFIANCE];
    CHECK_EQ(sim_can_force_law(&c, LOI_SMIC), LAW_OK);
    CHECK_EQ(sim_force_law(&c, LOI_SMIC), LAW_OK);
    CHECK(law_is_adopted(&c, LOI_SMIC));
    CHECK_EQ(c.cp, 15 - law_get(LOI_SMIC)->cost - ART49_CP);
    CHECK_EQ(c.gauges[JAUGE_DEFIANCE], d0 + 15);
    CHECK_EQ(c.n_pending, 1);
    // Une seule autre loi par an
    CHECK_EQ(sim_force_law(&c, LOI_ISF), LAW_ERR_493);
    for (i = 0; i < 4; i++) sim_end_turn(&c, 0);
    c.cp = 15;
    CHECK_EQ(sim_can_force_law(&c, LOI_ISF), LAW_OK);
    c.cp = 2;
    CHECK_EQ(sim_can_force_law(&c, LOI_ISF), LAW_ERR_CP);
    CHECK_EQ(sim_propose_law(&c, LOI_SMIC, &v), LAW_ERR_ADOPTED);

    // Télévision : 2 CP, une fois par tour, l'usure rapproche du flop
    sim_init(&c, ORIENT_CENTRE, 4, 4);
    c.cp = 10;
    CHECK_EQ(sim_tv_flop_chance(&c), 15);
    CHECK(sim_tv(&c) <= TV_FLOP);
    CHECK_EQ(c.cp, 8);
    CHECK_EQ(sim_tv(&c), TV_ERR_USED);
    CHECK(!sim_tv_available(&c));
    CHECK(sim_tv_flop_chance(&c) > 15);
    c.cp = 1;
    sim_end_turn(&c, 0);
    c.cp = 1;
    CHECK_EQ(sim_tv(&c), TV_ERR_CP);
    // Statistique : usure maximale => flop presque toujours
    for (i = 0; i < 100; i++) {
        sim_init(&c, ORIENT_CENTRE, (u32)i + 1, 4);
        c.cp = 10; c.tv_fatigue = 10;
        CHECK_EQ(sim_tv_flop_chance(&c), 90);
        flops = (u8)(flops + (sim_tv(&c) == TV_FLOP));
    }
    CHECK(flops > 75);
}

// Met un préréglage en vigueur sans passer par le vote.
static void set_preset(Country *c, u8 preset)
{
    BudgetPlan p;
    sim_budget_preset_plan(preset, &p);
    budget_set_current(c, &p);
}

static void test_v1_budget(void)
{
    Country c, r, l;
    VoteResult v;
    TurnReport rep;
    int i;
    sim_init(&c, ORIENT_CENTRE, 31, 4);
    CHECK_EQ(c.budget, BUDGET_EQUILIBRE);
    CHECK(!sim_budget_due(&c));
    CHECK_EQ(sim_budget_vote(&c, BUDGET_RELANCE, &v), BUDGET_ERR_NOT_DUE);
    for (i = 0; i < 3; i++) sim_end_turn(&c, 0);
    CHECK_EQ(sim_quarter(&c), 3);
    CHECK(sim_budget_due(&c));
    CHECK_EQ(sim_budget_vote(&c, BUDGET_COUNT, &v), BUDGET_ERR_ID);
    sim_budget_preview(&c, BUDGET_RELANCE, &v);
    CHECK_EQ(sim_budget_vote(&c, BUDGET_RELANCE, &v), BUDGET_OK);
    if (v.adopted) {
        CHECK_EQ(c.budget, BUDGET_RELANCE);
        CHECK(!sim_budget_due(&c));
    }
    sim_end_turn(&c, &rep);
    CHECK_EQ(rep.budget_reconduit, !v.adopted);

    // 49.3 sur le budget : illimité, 4 CP
    for (i = 0; i < 3; i++) sim_end_turn(&c, 0);
    c.cp = 9;
    CHECK(sim_budget_due(&c));
    CHECK_EQ(sim_budget_force(&c, BUDGET_RIGUEUR), BUDGET_OK);
    CHECK_EQ(c.cp, 5);
    CHECK_EQ(c.budget, BUDGET_RIGUEUR);
    sim_end_turn(&c, &rep);
    CHECK_EQ(rep.budget_reconduit, 0);
    CHECK_EQ(c.budget, BUDGET_RIGUEUR);   // en vigueur toute l'année suivante

    // Reconduction : aucune décision => les dépenses reculent d'un cran vers 0,
    // les impôts ne changent pas (Rigueur : Soc -1, Trans -1, Ménages +1)
    for (i = 0; i < 3; i++) sim_end_turn(&c, 0);
    CHECK(sim_budget_due(&c));
    sim_end_turn(&c, &rep);
    CHECK_EQ(rep.budget_reconduit, 1);
    CHECK_EQ(c.budget_lvl[BUD_SOCIAL], 0);
    CHECK_EQ(c.budget_lvl[BUD_TRANSITION], 0);
    CHECK_EQ(c.budget_lvl[BUD_IMPOT_MENAGES], 1);
    CHECK_EQ(c.budget, BUDGET_PERSO);
    // une seconde reconduction ne touche plus rien (dépenses à 0)
    for (i = 0; i < 4; i++) sim_end_turn(&c, 0);
    CHECK_EQ(c.budget_lvl[BUD_IMPOT_MENAGES], 1);

    // Effets : sur 2 ans, la rigueur réduit la dette et le social par rapport à la relance
    sim_init(&r, ORIENT_CENTRE, 77, 4); set_preset(&r, BUDGET_RIGUEUR);
    sim_init(&l, ORIENT_CENTRE, 77, 4); set_preset(&l, BUDGET_RELANCE);
    for (i = 0; i < 8; i++) {
        r.budget_done = l.budget_done = 1;
        sim_end_turn(&r, 0); sim_end_turn(&l, 0);
    }
    CHECK(r.debt < l.debt);
    CHECK(r.pillars[PIL_SOCIAL] < l.pillars[PIL_SOCIAL]);
    CHECK(r.opinion[ELEC_AISEES] - r.opinion[ELEC_POPULAIRES]
          > l.opinion[ELEC_AISEES] - l.opinion[ELEC_POPULAIRES]);
    CHECK(sim_budget_orientation(BUDGET_RIGUEUR) > 0 && sim_budget_orientation(BUDGET_RELANCE) < 0);
}

static void test_v1_censure(void)
{
    Country c;
    TurnReport rep;
    int g;
    sim_init(&c, ORIENT_GAUCHE, 41, 4);
    sim_change_pm(&c, PM_TECHNOCRATE);
    for (g = 0; g < GRP_COUNT; g++) if (g != c.player_group) c.relation[g] = 20;
    c.gauges[JAUGE_DEFIANCE] = 80;
    CHECK(sim_censure_threat(&c));
    CHECK(sim_censure_votes(&c) >= SEATS_MAJORITY);
    {
        s16 pop0 = sim_popularity(&c);
        (void)pop0;
        sim_end_turn(&c, &rep);
    }
    CHECK_EQ(rep.censure, CENSURE_ADOPTEE);
    CHECK(rep.new_pm != PM_TECHNOCRATE && rep.new_pm < PM_COUNT);
    CHECK_EQ(c.pm, rep.new_pm);
    CHECK_EQ(c.censures, 1);
    CHECK(c.gauges[JAUGE_DEFIANCE] <= 30);
    CHECK_EQ(c.status, STATUS_PLAYING);   // le président reste

    // Opposition insuffisante : pas de motion
    sim_init(&c, ORIENT_GAUCHE, 41, 4);
    c.gauges[JAUGE_DEFIANCE] = 80;
    CHECK(!sim_censure_threat(&c));
    sim_end_turn(&c, &rep);
    CHECK_EQ(rep.censure, CENSURE_NONE);
}

static void test_v1_dissolution(void)
{
    Country c;
    LegislativeResult lr;
    long hi = 0, lo = 0;
    u32 seed;
    sim_init(&c, ORIENT_CENTRE, 51, 4);
    CHECK(sim_can_dissolve(&c));
    CHECK(sim_dissolve(&c, &lr));
    CHECK(v1_in_bounds(&c));
    CHECK_EQ(lr.bloc_after, sim_bloc_seats(&c));
    CHECK_EQ(lr.majority, sim_has_majority(&c));
    CHECK(!sim_can_dissolve(&c));                  // une fois par an
    sim_end_turn(&c, 0); sim_end_turn(&c, 0); sim_end_turn(&c, 0);
    CHECK(!sim_can_dissolve(&c));
    sim_end_turn(&c, 0);
    CHECK(sim_can_dissolve(&c));

    // Popularité => sièges du bloc présidentiel
    for (seed = 1; seed <= 100; seed++) {
        Country a;
        sim_init(&a, (u8)(seed % 3), seed, 4);
        SET_OPN(a, 600);
        sim_legislatives(&a, &lr); hi += lr.bloc_after;
        CHECK(v1_in_bounds(&a));
        sim_init(&a, (u8)(seed % 3), seed, 4);
        SET_OPN(a, 280);
        sim_legislatives(&a, &lr); lo += lr.bloc_after;
        CHECK(v1_in_bounds(&a));
    }
    CHECK(hi / 100 >= SEATS_MAJORITY);
    CHECK(lo / 100 < SEATS_MAJORITY);   // majorité perdue => majorité relative
}

static void test_v1_impeach(void)
{
    Country c;
    TurnReport rep;
    VoteResult v;
    int g;

    // Procédure engagée, défense, destitution votée
    sim_init(&c, ORIENT_CENTRE, 61, 4);
    c.gauges[JAUGE_SCANDALE] = 95;
    c.gauges[JAUGE_DEFIANCE] = 90;
    for (g = 0; g < GRP_COUNT; g++) c.relation[g] = 5;
    c.relation_base[c.player_group] = 5;
    sim_end_turn(&c, &rep);
    CHECK_EQ(rep.impeach, IMP_EV_ENGAGEE);
    CHECK(sim_impeach_pending(&c));
    c.cp = 10;
    CHECK(sim_defense_available(&c, DEFENSE_TV));
    CHECK(sim_defend(&c, DEFENSE_TV, 0));
    CHECK(!sim_defend(&c, DEFENSE_TV, 0));                // une seule fois
    CHECK(!sim_defend(&c, DEFENSE_NEGOCIER, c.player_group));
    CHECK(sim_defend(&c, DEFENSE_NEGOCIER, GRP_G1));
    CHECK_EQ(c.cp, 10 - DEFENSE_TV_CP - DEFENSE_NEGOCIER_CP);
    for (g = 0; g < GRP_COUNT; g++) c.relation[g] = 0;
    c.gauges[JAUGE_SCANDALE] = 100;
    c.gauges[JAUGE_DEFIANCE] = 100;
    sim_end_turn(&c, &rep);
    CHECK_EQ(rep.impeach, IMP_EV_VOTEE);
    CHECK(rep.impeach_votes >= IMPEACH_MAJORITY);
    CHECK_EQ(c.status, STATUS_DESTITUE);
    CHECK(sim_game_over(&c));
    CHECK(!sim_election_due(&c));
    c.cp = 10;
    CHECK_EQ(sim_propose_law(&c, LOI_RENOV, &v), LAW_ERR_STATE);
    {
        u8 t = c.turn;
        sim_end_turn(&c, &rep);                            // plus rien ne bouge
        CHECK_EQ(c.turn, t);
    }

    // Défense réussie : le scandale retombe, la défiance reste haute
    sim_init(&c, ORIENT_CENTRE, 62, 4);
    c.gauges[JAUGE_SCANDALE] = 80;
    c.gauges[JAUGE_DEFIANCE] = 70;
    sim_end_turn(&c, &rep);
    CHECK_EQ(rep.impeach, IMP_EV_ENGAGEE);
    CHECK(sim_defend(&c, DEFENSE_SACRIFICE, 0));
    sim_end_turn(&c, &rep);
    CHECK_EQ(rep.impeach, IMP_EV_REJETEE);
    CHECK(rep.impeach_votes < IMPEACH_MAJORITY);
    CHECK_EQ(c.status, STATUS_PLAYING);
    CHECK(c.gauges[JAUGE_SCANDALE] < IMPEACH_SCANDALE);
    CHECK(c.gauges[JAUGE_DEFIANCE] >= 50);
    CHECK(!sim_impeach_pending(&c));
}

static void test_v1_second_mandate(void)
{
    Country c;
    LegislativeResult lr;
    u16 s1;
    s16 t0;
    sim_init(&c, ORIENT_GAUCHE, 71, 4);
    c.cp = 10;
    {
        VoteResult v;
        sim_propose_law(&c, LOI_SMIC, &v);
    }
    while (!sim_mandate_over(&c)) sim_end_turn(&c, 0);
    CHECK(sim_election_due(&c));
    CHECK(!sim_game_over(&c));

    // Thème : bonus positif auprès de l'électorat visé
    CHECK(sim_theme_bonus(&c, THEME_POUVOIR_ACHAT, ELEC_POPULAIRES) > 0);
    CHECK_EQ(sim_theme_bonus(&c, THEME_POUVOIR_ACHAT, ELEC_AISEES), 0);
    CHECK_EQ(sim_theme_bonus(&c, THEME_NONE, ELEC_POPULAIRES), 0);
    sim_set_theme(&c, THEME_EMPLOI);
    CHECK_EQ(c.theme, THEME_EMPLOI);

    s1 = sim_mandate_score(&c, 1);
    t0 = sim_opinion_target(&c, ELEC_MOYENNES);
    sim_start_second_mandate(&c, &lr);
    CHECK_EQ(c.mandate, 1);
    CHECK_EQ(c.turn, 0);
    CHECK_EQ(sim_score_first(&c), s1);
    CHECK_EQ(c.theme, THEME_NONE);
    CHECK(!law_is_adopted(&c, LOI_SMIC));
    CHECK(v1_in_bounds(&c));
    CHECK(!sim_can_dissolve(&c));               // législatives à l'instant
    // Usure du pouvoir (et fin de l'état de grâce du 1er mandat remplacée par +4)
    CHECK(sim_opinion_target(&c, ELEC_MOYENNES) < t0 + 40);
    while (!sim_mandate_over(&c)) sim_end_turn(&c, 0);
    CHECK(sim_game_over(&c));
    CHECK(!sim_election_due(&c));
    CHECK_EQ(sim_score_total(&c), s1 + sim_mandate_score(&c, 0));
}

// Second mandat : une loi en attente reste marquée adoptée (non reproposable).
static void test_v1_second_mandate_pending(void)
{
    Country c;
    LegislativeResult lr;
    sim_init(&c, ORIENT_GAUCHE, 72, 4);
    while (!sim_mandate_over(&c)) sim_end_turn(&c, 0);
    c.n_pending = 1;
    c.pending[0].law = LOI_SMIC;
    c.pending[0].turns_left = 2;
    CHECK(!law_is_adopted(&c, LOI_SMIC));
    sim_start_second_mandate(&c, &lr);
    CHECK(law_is_adopted(&c, LOI_SMIC));
    CHECK_EQ(c.n_pending, 1);          // elle s'appliquera quand même
    CHECK_EQ(law_can_propose(&c, LOI_SMIC), LAW_ERR_ADOPTED);
}

// ---------------------------------------------------------------------------
// Parties automatiques (1.20)
// ---------------------------------------------------------------------------

// V4 : trois variantes de budget (optimiseur, extrême, austère) et un joueur à
// préréglage fixe (g_fixed_preset) pour mesurer la dette finale par préréglage.
enum { BOT_ACTIF, BOT_PASSIF, BOT_SCANDALE, BOT_PREMIER_CHOIX, BOT_DISSOLVEUR, BOT_REFERENDUM,
       BOT_OPTIMISEUR, BOT_EXTREME, BOT_AUSTERE, BOT_PRESET, BOT_COUNT };
static u8 g_fixed_preset = BUDGET_EQUILIBRE;
static long n_budgets, n_amend;

#define BOT_NEW_LAW_RESERVE 4

static u8 bot_pref[ORIENT_COUNT][LOI_COUNT] = {
    { LOI_SMIC, LOI_HOPITAL, LOI_RENOV, LOI_ECOLE, LOI_TRANSPORTS, LOI_LOGEMENT,
      LOI_ISF, LOI_NUMERIQUE, LOI_CIR, LOI_ENERGIE, LOI_POLICE, LOI_IS,
      LOI_CHOMAGE, LOI_RETRAITES, LOI_IMMIGRATION },
    { LOI_RENOV, LOI_CIR, LOI_ENERGIE, LOI_ECOLE, LOI_HOPITAL, LOI_POLICE,
      LOI_NUMERIQUE, LOI_TRANSPORTS, LOI_CHOMAGE, LOI_LOGEMENT, LOI_IS,
      LOI_RETRAITES, LOI_SMIC, LOI_ISF, LOI_IMMIGRATION },
    { LOI_IS, LOI_POLICE, LOI_CIR, LOI_ENERGIE, LOI_CHOMAGE, LOI_RETRAITES,
      LOI_IMMIGRATION, LOI_RENOV, LOI_ECOLE, LOI_HOPITAL, LOI_NUMERIQUE,
      LOI_LOGEMENT, LOI_TRANSPORTS, LOI_SMIC, LOI_ISF },
};

// V2 : après les 15 lois de base (ordre ci-dessus), le joueur automatique utilise
// les 25 nouvelles lois, de la plus proche de son orientation à la plus éloignée.
static void bot_pref_init(void)
{
    static const s8 target[ORIENT_COUNT] = { -50, 0, 50 };
    static int done;
    int o, i, j, n;
    if (done) return;
    done = 1;
    for (o = 0; o < ORIENT_COUNT; o++) {
        u8 *p = bot_pref[o];
        for (n = 0, i = LOI_LOGEMENT + 1; i < LOI_COUNT; i++, n++) p[15 + n] = (u8)i;
        for (i = 15; i < LOI_COUNT; i++)          // tri par insertion, stable
            for (j = i; j > 15; j--) {
                int da = sim_abs16((s16)(law_defs[p[j - 1]].orientation - target[o]));
                int db = sim_abs16((s16)(law_defs[p[j]].orientation - target[o]));
                if (db >= da) break;
                { u8 t = p[j]; p[j] = p[j - 1]; p[j - 1] = t; }
            }
    }
}

static u8 bot_budget(const Country *c)
{
    s16 drift = (s16)(c->debt - c->debt_start);
    switch (c->orientation) {
    case ORIENT_GAUCHE: return drift > 80 ? BUDGET_EQUILIBRE : BUDGET_RELANCE;
    case ORIENT_DROITE: return drift > 60 ? BUDGET_RIGUEUR : BUDGET_EQUILIBRE;
    default:            return drift > 120 ? BUDGET_RIGUEUR : BUDGET_EQUILIBRE;
    }
}

// Popularité prévue d'un plan : moyenne pondérée des effets d'opinion (dixièmes x 100).
static s32 plan_score(const Country *c, const BudgetPlan *p)
{
    s32 t = 0;
    u8 e;
    for (e = 0; e < ELEC_COUNT; e++) t += (s32)elec_weight[e] * sim_plan_opinion(p, e);
    (void)c;
    return t;
}

// Optimiseur : glouton par coordonnées, à partir d'Équilibre ; ne retient que des
// plans que l'Assemblée adopte.
static void bot_plan_optimiseur(const Country *c, BudgetPlan *out)
{
    BudgetPlan cur, cand;
    VoteResult v;
    s32 best;
    int it, l, d, improved;
    sim_budget_preset_plan(BUDGET_EQUILIBRE, &cur);
    best = plan_score(c, &cur);
    for (it = 0; it < 30; it++) {
        s32 bs = best;
        BudgetPlan bp = cur;
        improved = 0;
        for (l = 0; l < BUD_LINES; l++)
            for (d = -1; d <= 1; d += 2) {
                cand = cur;
                cand.lvl[l] = (s8)(cand.lvl[l] + d);
                if (cand.lvl[l] < BUD_LVL_MIN || cand.lvl[l] > BUD_LVL_MAX) continue;
                sim_budget_plan_preview(c, &cand, &v);
                if (!v.adopted) continue;
                if (plan_score(c, &cand) > bs) { bs = plan_score(c, &cand); bp = cand; improved = 1; }
            }
        if (!improved) break;
        cur = bp;
        best = bs;
    }
    *out = cur;
}

static void bot_plan(const Country *c, int bot, BudgetPlan *out)
{
    int i;
    switch (bot) {
    case BOT_OPTIMISEUR: bot_plan_optimiseur(c, out); break;
    case BOT_EXTREME:
        for (i = 0; i < BUD_LINES; i++) out->lvl[i] = (s8)(i < BUD_IMPOT_MENAGES ? 2 : -2);
        break;
    case BOT_AUSTERE:
        for (i = 0; i < BUD_LINES; i++) out->lvl[i] = -2;
        break;
    case BOT_PRESET: sim_budget_preset_plan(g_fixed_preset, out); break;
    default: sim_budget_preset_plan(bot_budget(c), out); break;
    }
}

static long n_ref, n_ref_won, n_reshuffle;

static void bot_actions(Country *c, int bot)
{
    VoteResult v;
    int i;
    s16 pop = sim_popularity(c);
    u8 left = (u8)(((sim_mandate_turns(c) - c->turn) * 4 + c->turns_per_year - 1) / c->turns_per_year);   // en tours de 20

    if (bot == BOT_PASSIF || bot == BOT_PREMIER_CHOIX) return;
    bot_pref_init();

    // Défense contre la destitution
    if (sim_impeach_pending(c) && bot != BOT_SCANDALE) {
        u8 best = GRP_COUNT, g;
        sim_defend(c, DEFENSE_SACRIFICE, 0);
        sim_defend(c, DEFENSE_TV, 0);
        for (g = 0; g < GRP_COUNT; g++)
            if (g != c->player_group && (best == GRP_COUNT || c->seats[g] > c->seats[best]))
                best = g;
        sim_defend(c, DEFENSE_NEGOCIER, best);
    }
    // Budget
    if (sim_budget_due(c)) {
        BudgetPlan plan;
        bot_plan(c, bot, &plan);
        n_budgets++;
        if (bot == BOT_EXTREME && c->cp >= BUDGET_493_CP) {
            sim_budget_plan_force(c, &plan);   // 49.3 chaque année
        } else {
            sim_budget_plan_vote(c, &plan, &v);
            if (!v.adopted && c->cp >= BUDGET_493_CP + 2) sim_budget_plan_force(c, &plan);
        }
        if (c->budget_done && sim_budget_amended_line(c)) n_amend++;
    }
    // Dissolution si majorité relative et popularité suffisante (le dissolveur
    // dissout dès qu'il le peut, quelle que soit sa popularité : cohabitations)
    if ((bot == BOT_DISSOLVEUR ? (pop < 420 && c->turn >= 2 * c->turns_per_year)
                               : (!sim_has_majority(c) && pop >= 470)) && sim_can_dissolve(c))
        sim_dissolve(c, 0);
    // V2 : remaniement si la popularité s'effondre avec le Technocrate en poste
    // (jamais en cohabitation) : le Rassembleur remplace son malus d'opinion.
    if (pop < 400 && c->pm == PM_TECHNOCRATE && c->cp >= RESHUFFLE_CP + 4 && !sim_cohabitation(c))
    { sim_reshuffle(c, PM_RASSEMBLEUR); n_reshuffle++; }
    // Variante « référendaire » : dépense ses CP dans un référendum sur la loi la
    // plus approuvée dès que possible (mi-mandat), et remanie si le PM est le Technocrate.
    if (bot == BOT_REFERENDUM && !c->referendum_done && c->cp >= REFERENDUM_CP
        && c->turn >= sim_mandate_turns(c) / 3) {
        RefResult rp;
        u8 best = LOI_COUNT;
        s16 by = 0;
        for (i = 0; i < LOI_COUNT; i++) {
            if (sim_referendum_check(c, (u8)i) != RFD_OK) continue;
            sim_referendum_preview(c, (u8)i, &rp);
            if (rp.yes > by) { by = rp.yes; best = (u8)i; }
        }
        if (best < LOI_COUNT && by >= 520) { sim_referendum(c, best, &rp); n_ref++; n_ref_won += rp.adopted; }
    }
    if (bot == BOT_REFERENDUM && c->pm == PM_TECHNOCRATE && c->cp >= RESHUFFLE_CP + 1
        && c->turn >= c->turns_per_year && !sim_cohabitation(c) && sim_reshuffle(c, PM_RASSEMBLEUR) == RSH_OK)
        n_reshuffle++;
    // Lois, par ordre de préférence ; 49.3 pour une des 4 préférées
    for (i = 0; i < LOI_COUNT; i++) {
        u8 id = bot_pref[c->orientation][i];
        if (law_can_propose(c, id) != LAW_OK) continue;
        // Les lois V2 ne servent que de surplus : le joueur garde une réserve de CP
        // pour le référendum, la télévision et le 49.3.
        if (id > LOI_LOGEMENT && c->cp < law_cost(c, law_get(id)) + BOT_NEW_LAW_RESERVE) continue;
        parl_vote(c, law_get(id)->orientation, &v);
        if (v.adopted) {
            sim_propose_law(c, id, &v);
        } else if (i < 6 && sim_referendum_check(c, id) == RFD_OK) {
            // V2 : référendum (une fois par mandat) sur une loi préférée que
            // l'Assemblée refuse, si le « oui » est probable.
            RefResult rp;
            sim_referendum_preview(c, id, &rp);
            if (rp.yes >= 500) { sim_referendum(c, id, &rp); n_ref++; n_ref_won += rp.adopted; }
            else if (i < 4 && c->gauges[JAUGE_DEFIANCE] < 35 && sim_can_force_law(c, id) == LAW_OK)
                sim_force_law(c, id);
        } else if (i < 4 && c->gauges[JAUGE_DEFIANCE] < 35 && sim_can_force_law(c, id) == LAW_OK)
            sim_force_law(c, id);
    }
    // Télévision, avec modération
    if (bot != BOT_SCANDALE && c->tv_fatigue == 0 && c->cp >= 4 && (pop < 380 || left <= 3))
        sim_tv(c);
}

static u8 bot_event_choice(const Country *c, int bot, u8 ev, Rng *prng)
{
    const EventDef *e = event_get(ev);
    (void)c;
    if (bot == BOT_PREMIER_CHOIX) return 0;
    if (bot == BOT_SCANDALE) {
        u8 k, j, best = 0;
        s16 bv = -1000;
        for (k = 0; k < e->n_choices; k++) {
            s16 s = 0;
            for (j = 0; j < e->choices[k].n_effects; j++)
                if (e->choices[k].effects[j].key == EFF_SCANDALE) s += e->choices[k].effects[j].value;
            if (s > bv) { bv = s; best = k; }
        }
        return best;
    }
    return (u8)rng_below(prng, e->n_choices);
}

typedef struct {
    s16 pop1, debt1, unemp1;   // fin du 1er mandat
    u8  elected, destitue, nat_first, laws1, censures, cohabs, refs;
    s16 pop_e[ELEC_COUNT];     // opinion de chaque électorat à la fin du 1er mandat
    u16 score1, total;         // total : 0 si pas de 2nd mandat complet
} GameStats;

static void bot_mandate(Country *c, int bot, Rng *prng)
{
    TurnReport rep;
    while (!sim_mandate_over(c) && c->status == STATUS_PLAYING) {
        s16 ev;
        bot_actions(c, bot);
        sim_end_turn(c, &rep);
        if (c->status != STATUS_PLAYING || sim_mandate_over(c)) break;
        ev = events_draw(c);
        if (ev >= 0) events_choose(c, (u8)ev, bot_event_choice(c, bot, (u8)ev, prng));
        if (!v1_in_bounds(c)) { CHECK(v1_in_bounds(c)); break; }
    }
}

static void bot_game(u8 orient, u8 diff, u8 pm, u8 tpy, u32 seed, int bot, GameStats *st)
{
    Country c;
    SimSetup s;
    Rng prng;
    ElectionResult r;
    int k;
    setup(&s, orient, pm, diff, seed);
    s.turns_per_year = tpy;
    sim_init_ex(&c, &s);
    rng_seed(&prng, seed ^ 0x9E3779B9u);
    st->elected = st->destitue = st->nat_first = 0;
    st->total = 0;
    st->score1 = 0;

    bot_mandate(&c, bot, &prng);
    st->pop1 = sim_popularity(&c);
    for (k = 0; k < ELEC_COUNT; k++) st->pop_e[k] = c.opinion[k];
    st->debt1 = c.debt;
    st->unemp1 = c.unemployment;
    st->laws1 = 0;
    for (k = 0; k < LOI_COUNT; k++) st->laws1 = (u8)(st->laws1 + law_is_adopted(&c, (u8)k));
    if (c.status == STATUS_DESTITUE) { st->destitue = 1; st->censures = c.censures; st->cohabs = c.cohabitations; return; }

    if (bot != BOT_PASSIF && bot != BOT_PREMIER_CHOIX) {   // thème : le meilleur pour la campagne
        u8 t, best = 0;
        s16 bp = -1;
        for (t = 0; t < THEME_COUNT; t++) {
            sim_set_theme(&c, t);
            if (sim_campaign_popularity(&c) > bp) { bp = sim_campaign_popularity(&c); best = t; }
        }
        sim_set_theme(&c, best);
    }
    sim_election(&c, &r);
    st->elected = r.elected;
    st->nat_first = (u8)(r.finalist[0] == CAND_NATIONALISTE);
    st->score1 = sim_mandate_score(&c, r.elected);
    if (r.elected) {
        sim_start_second_mandate(&c, 0);
        bot_mandate(&c, bot, &prng);
        if (c.status == STATUS_DESTITUE) st->destitue = 1;
        else st->total = sim_score_total(&c);
    }
    st->censures = c.censures;
    st->cohabs = c.cohabitations;
}

#define AUTO_SEEDS 800

typedef struct {
    double pop_e[ELEC_COUNT];
    double pop, debt, unemp, elected, destit, nat, laws, score1, total, censures, cohabs;
    long n_total;
} Agg;

static void run_config(u8 orient, u8 diff, u8 tpy, int bot, Agg *a)
{
    u32 seed;
    long n1 = 0;
    a->pop = a->debt = a->unemp = a->elected = a->destit = a->nat = 0;
    a->laws = a->score1 = a->total = a->censures = a->cohabs = 0;
    a->n_total = 0;
    { int e; for (e = 0; e < ELEC_COUNT; e++) a->pop_e[e] = 0; }
    for (seed = 1; seed <= AUTO_SEEDS; seed++) {
        GameStats st;
        u8 pm = (u8)(seed % PM_COUNT);   // les 3 Premiers ministres à parts égales
        bot_game(orient, diff, pm, tpy, seed * 2654435761u, bot, &st);
        { int e; for (e = 0; e < ELEC_COUNT; e++) a->pop_e[e] += st.pop_e[e]; }
        a->pop += st.pop1; a->debt += st.debt1; a->unemp += st.unemp1;
        a->elected += st.elected; a->destit += st.destitue; a->nat += st.nat_first;
        a->laws += st.laws1; a->censures += st.censures; a->cohabs += st.cohabs;
        if (!st.destitue || st.elected) { a->score1 += st.score1; n1++; }
        if (st.total) { a->total += st.total; a->n_total++; }
    }
    { int e; for (e = 0; e < ELEC_COUNT; e++) a->pop_e[e] /= 10.0 * AUTO_SEEDS; }
    a->pop /= 10.0 * AUTO_SEEDS; a->debt /= 10.0 * AUTO_SEEDS; a->unemp /= 100.0 * AUTO_SEEDS;
    a->elected *= 100.0 / AUTO_SEEDS; a->destit *= 100.0 / AUTO_SEEDS;
    a->nat *= 100.0 / AUTO_SEEDS; a->laws /= AUTO_SEEDS; a->censures /= AUTO_SEEDS; a->cohabs *= 100.0 / AUTO_SEEDS;
    a->score1 = n1 ? a->score1 / n1 : 0;
    a->total = a->n_total ? a->total / a->n_total : 0;
}

static void test_v1_autoplay(void)
{
    static const char *on[ORIENT_COUNT] = { "gauche", "centre", "droite" };
    static const char *dn[DIFF_COUNT] = { "facile", "normal", "diffic" };
    static const char *bn[BOT_COUNT] = { "actif", "passif", "scand.", "1erch", "dissol", "refer", "optim.", "extrem", "auster", "fixe" };
    int o, d, b;
    double passive_max = 0, scand_destit = 0, active_destit_max = 0;
    double el_norm[ORIENT_COUNT] = { 0 }, el_easy = 0, el_hard = 0, el_mid = 0;

    printf("  Partie automatique V1 (%d graines, 20 tours, événements, 3 PM) :\n", AUTO_SEEDS);
    printf("  %-6s %-6s %-6s %5s %5s %5s %5s %5s %5s %5s %5s %5s %5s %6s\n", "diff", "orient", "joueur",
           "pop%", "chom", "dette", "lois", "élu%", "dest%", "cens", "coh/100", "NAT1%", "sc1", "cumul");
    for (b = 0; b < BOT_COUNT; b++) {
        for (d = 0; d < DIFF_COUNT; d++) {
            if (b >= BOT_SCANDALE && d != DIFF_NORMAL) continue;
            if (b == BOT_PRESET) continue;   // traité à part (dette par préréglage)
            for (o = 0; o < ORIENT_COUNT; o++) {
                Agg a;
                run_config((u8)o, (u8)d, 4, b, &a);
                printf("  %-6s %-6s %-6s %5.1f %5.2f %5.1f %5.1f %5.0f %5.1f %5.2f %5.0f %5.0f %5.0f %6.0f\n",
                       dn[d], on[o], bn[b], a.pop, a.unemp, a.debt, a.laws, a.elected, a.destit,
                       a.censures, a.cohabs, a.nat, a.score1, a.total);
                if (b == BOT_ACTIF) {
                    if (d == DIFF_NORMAL) { el_norm[o] = a.elected; el_mid += a.elected / 3; }
                    if (d == DIFF_FACILE) el_easy += a.elected / 3;
                    if (d == DIFF_DIFFICILE) el_hard += a.elected / 3;
                    if (a.destit > active_destit_max) active_destit_max = a.destit;
                    // Dette : dérive réaliste sur 5 ans
                    CHECK(a.debt < 100.0 + (d == DIFF_FACILE ? 0 : d == DIFF_NORMAL ? 10 : 20) + 20);
                }
                // Cible SPEC_V4 §3.4 (tâche 4.14) : aucune variante de budget ne dépasse le
                // joueur actif de plus de 15 points de réélection (mesuré : optimiseur +11 au plus).
                if (b >= BOT_OPTIMISEUR && b <= BOT_AUSTERE)
                    CHECK(a.elected <= el_norm[o] + 15.0);
                if (b == BOT_PASSIF && d == DIFF_NORMAL && a.elected > passive_max) passive_max = a.elected;
                if (b == BOT_SCANDALE && a.destit > scand_destit) scand_destit = a.destit;
                if (b == BOT_PREMIER_CHOIX) CHECK(a.debt < 110.0 + 18);
            }
        }
    }
    printf("  Actions V2 du joueur automatique : %ld référendums (%ld adoptés), %ld remaniements\n",
           n_ref, n_ref_won, n_reshuffle);
    printf("  Réélection moyenne (actif) : facile %.0f %%, normal %.0f %%, difficile %.0f %%\n",
           el_easy, el_mid, el_hard);
    printf("  Budget : %ld budgets présentés, %ld amendés par le Sénat (%.0f %%)\n",
           n_budgets, n_amend, n_budgets ? 100.0 * n_amend / n_budgets : 0.0);
    // V4 : popularité finale par électorat et par orientation (joueur actif, normal).
    printf("  Popularité finale du 1er mandat par électorat (actif, normal) : Pop Moy Cad Ret Jeu\n");
    for (o = 0; o < ORIENT_COUNT; o++) {
        Agg a;
        int e;
        run_config((u8)o, DIFF_NORMAL, 4, BOT_ACTIF, &a);
        printf("  %-6s", on[o]);
        for (e = 0; e < ELEC_COUNT; e++) {
            printf(" %5.1f", a.pop_e[e]);
            CHECK(a.pop_e[e] > 20.0 && a.pop_e[e] < 80.0);   // aucun électorat durablement hors de 20-80 %
        }
        printf("\n");
    }
    // V4 : un préréglage fixe toute la partie (dette, popularité, réélection).
    printf("  Préréglage fixe (normal, centre) : pop%% dette élu%%\n");
    for (b = 0; b < BUDGET_COUNT; b++) {
        static const char *pn[BUDGET_COUNT] = { "rigueur", "équilibre", "relance" };
        Agg a;
        g_fixed_preset = (u8)b;
        run_config(ORIENT_CENTRE, DIFF_NORMAL, 4, BOT_PRESET, &a);
        printf("  %-9s %5.1f %6.1f %4.0f\n", pn[b], a.pop, a.debt, a.elected);
        CHECK(a.elected <= el_norm[ORIENT_CENTRE] + 15.0);   // aucun préréglage fixe ne domine
    }
    // Cibles d'équilibrage (1.20)
    for (o = 0; o < ORIENT_COUNT; o++) {
        CHECK(el_norm[o] >= 35);
        CHECK(el_norm[o] <= 65);
    }
    CHECK(el_easy > el_mid && el_mid > el_hard);
    CHECK(passive_max < 5);
    CHECK(active_destit_max < 5);
    CHECK(scand_destit > 0);

    // Mode 60 tours (V2, 2.15) : mêmes statistiques que le mode 20 tours (écart <= 5 points).
    printf("  Mode 60 tours (actif), écart 60 - 20 tours (pop %% / chômage pt / dette pt / élu %% / destit %%) :\n");
    printf("  %-6s %-6s | %6s %6s %6s %5s %5s | %6s %6s %6s %5s %5s\n", "diff", "orient",
           "pop20", "chom20", "det20", "élu20", "des20", "pop60", "chom60", "det60", "élu60", "des60");
    for (d = 0; d < DIFF_COUNT; d++)
        for (o = 0; o < ORIENT_COUNT; o++) {
            Agg a4, a12;
            run_config((u8)o, (u8)d, 4, BOT_ACTIF, &a4);
            run_config((u8)o, (u8)d, 12, BOT_ACTIF, &a12);
            printf("  %-6s %-6s | %6.1f %6.2f %6.1f %5.0f %5.1f | %6.1f %6.2f %6.1f %5.0f %5.1f\n", dn[d], on[o],
                   a4.pop, a4.unemp, a4.debt, a4.elected, a4.destit,
                   a12.pop, a12.unemp, a12.debt, a12.elected, a12.destit);
            CHECK(sim_abs16((s16)((a12.pop - a4.pop) * 10)) <= 50);
            CHECK(sim_abs16((s16)((a12.unemp - a4.unemp) * 10)) <= 50);
            CHECK(sim_abs16((s16)((a12.debt - a4.debt) * 10)) <= 50);
            CHECK(sim_abs16((s16)((a12.elected - a4.elected) * 10)) <= 90);
            CHECK(sim_abs16((s16)((a12.destit - a4.destit) * 10)) <= 50);
        }
}

static void test_v1_reproducible(void)
{
    GameStats a, b;
    bot_game(ORIENT_DROITE, DIFF_NORMAL, PM_RASSEMBLEUR, 4, 4242, BOT_ACTIF, &a);
    bot_game(ORIENT_DROITE, DIFF_NORMAL, PM_RASSEMBLEUR, 4, 4242, BOT_ACTIF, &b);
    CHECK_EQ(a.pop1, b.pop1);
    CHECK_EQ(a.debt1, b.debt1);
    CHECK_EQ(a.elected, b.elected);
    CHECK_EQ(a.score1, b.score1);
    CHECK_EQ(a.total, b.total);
}

void test_sim_v1(void)
{
    test_v1_init();
    test_v1_gauges();
    test_v1_493_tv();
    test_v1_budget();
    test_v1_censure();
    test_v1_dissolution();
    test_v1_impeach();
    test_v1_second_mandate();
    test_v1_second_mandate_pending();
    test_v1_reproducible();
    test_v1_autoplay();
}
