#include "test.h"
#include "sim/sim.h"
#include "save/gamesave.h"

// Tests V4 (docs/SPEC_V4.md) : 5 électorats (4.02, 4.03), budget détaillé (4.08),
// cohabitation et Sénat (4.09), sauvegarde (4.12).

static void new_game(Country *c, u8 orient, u8 diff, u32 seed)
{
    SimSetup s;
    s.orientation = orient;
    s.pm = PM_RASSEMBLEUR;
    s.difficulty = diff;
    s.turns_per_year = 4;
    s.seed = seed;
    sim_init_ex(c, &s);
}

static void to_budget_quarter(Country *c)
{
    while (sim_quarter(c) != 3) sim_end_turn(c, 0);
}

// --- 5 électorats ------------------------------------------------------------

static void test_electorates(void)
{
    Country c;
    u8 e, wsum = 0;
    s32 s;
    CHECK_EQ(ELEC_COUNT, 5);
    CHECK_EQ(ELEC_AISEES, ELEC_CADRES);
    CHECK_EQ(EFF_POP_CADRES, EFF_POP_AISEES);
    for (e = 0; e < ELEC_COUNT; e++) wsum = (u8)(wsum + elec_weight[e]);
    CHECK_EQ(wsum, 100);

    // Popularité pondérée : 26 / 22 / 12 / 28 / 12
    new_game(&c, ORIENT_CENTRE, DIFF_NORMAL, 3);
    c.opinion[0] = 500; c.opinion[1] = 400; c.opinion[2] = 300; c.opinion[3] = 600; c.opinion[4] = 100;
    s = 26 * 500 + 22 * 400 + 12 * 300 + 28 * 600 + 12 * 100;
    CHECK_EQ(sim_popularity(&c), (s + 50) / 100);

    // Électorat-cœur : gauche (Pop + Jeu) / 2, centre (Moy + Ret) / 2, droite (Cad + Ret) / 2
    c.orientation = ORIENT_GAUCHE;  CHECK_EQ(sim_core_opinion(&c), 300);
    c.orientation = ORIENT_CENTRE;  CHECK_EQ(sim_core_opinion(&c), 500);
    c.orientation = ORIENT_DROITE;  CHECK_EQ(sim_core_opinion(&c), 450);

    // Opinion de départ : la gauche démarre forte chez les populaires et les jeunes
    new_game(&c, ORIENT_GAUCHE, DIFF_NORMAL, 4);
    CHECK(c.opinion[ELEC_POPULAIRES] > c.opinion[ELEC_CADRES]);
    CHECK(c.opinion[ELEC_JEUNES] > c.opinion[ELEC_RETRAITES]);
    new_game(&c, ORIENT_DROITE, DIFF_NORMAL, 4);
    CHECK(c.opinion[ELEC_CADRES] > c.opinion[ELEC_JEUNES]);
    CHECK(c.opinion[ELEC_RETRAITES] > c.opinion[ELEC_POPULAIRES]);
}

static void test_effect_keys(void)
{
    Country c;
    new_game(&c, ORIENT_CENTRE, DIFF_NORMAL, 5);
    SET_OPN(c, 400);
    // clés ajoutées en fin de liste : aucune renumérotation
    CHECK(EFF_POP_RETRAITES == EFF_REL_PARTI + 1 && EFF_POP_JEUNES == EFF_REL_PARTI + 2);
    CHECK_EQ(EFF_COUNT, EFF_POP_JEUNES + 1);
    sim_apply_effect(&c, EFF_POP_RETRAITES, -5);
    CHECK_EQ(c.opinion[ELEC_RETRAITES], 350);
    CHECK_EQ(c.opinion[ELEC_JEUNES], 400);
    sim_apply_effect(&c, EFF_POP_JEUNES, 2);
    CHECK_EQ(c.opinion[ELEC_JEUNES], 420);
    CHECK_EQ(c.opinion[ELEC_POPULAIRES], 400);
    sim_apply_effect(&c, EFF_POP_CADRES, 1);
    CHECK_EQ(c.opinion[ELEC_CADRES], 410);
    sim_apply_effect(&c, EFF_POP_TOUS, 1);
    CHECK_EQ(c.opinion[ELEC_POPULAIRES], 410);
    CHECK_EQ(c.opinion[ELEC_RETRAITES], 360);
    CHECK_EQ(c.opinion[ELEC_JEUNES], 430);
    CHECK_EQ(sim_effect_elec(EFF_POP_JEUNES), ELEC_JEUNES);
    CHECK_EQ(sim_effect_elec(EFF_POP_TOUS), ELEC_COUNT);
    CHECK_EQ(sim_effect_elec(EFF_CROISSANCE), ELEC_COUNT);
    CHECK_EQ(sim_effect_scale(EFF_POP_RETRAITES), 10);
}

static void test_themes(void)
{
    Country c;
    s16 b;
    new_game(&c, ORIENT_CENTRE, DIFF_NORMAL, 6);
    CHECK_EQ(THEME_COUNT, 5);
    // Santé et retraites : Ret 4, Pop 2, Moy 2 (/4), crédibilité = progression du pilier Social
    c.pillars[PIL_SOCIAL] = (s16)(c.pillars_start[PIL_SOCIAL] + 90);   // +30 de crédibilité
    b = sim_theme_bonus(&c, THEME_SANTE_RETRAITES, ELEC_RETRAITES);
    CHECK_EQ(b, 20 + 30);
    CHECK_EQ(sim_theme_bonus(&c, THEME_SANTE_RETRAITES, ELEC_POPULAIRES), (20 + 30) * 2 / 4);
    CHECK_EQ(sim_theme_bonus(&c, THEME_SANTE_RETRAITES, ELEC_JEUNES), 0);
    CHECK_EQ(sim_theme_bonus(&c, THEME_SANTE_RETRAITES, ELEC_CADRES), 0);
    // Pouvoir d'achat : crédibilité réduite si Social < 0 ou impôts des ménages > 0
    c.pillars[PIL_SOCIAL] = c.pillars_start[PIL_SOCIAL];
    b = sim_theme_bonus(&c, THEME_POUVOIR_ACHAT, ELEC_POPULAIRES);
    c.budget_lvl[BUD_IMPOT_MENAGES] = 1;
    CHECK(sim_theme_bonus(&c, THEME_POUVOIR_ACHAT, ELEC_POPULAIRES) < b);
    c.budget_lvl[BUD_IMPOT_MENAGES] = 0;
    c.budget_lvl[BUD_SOCIAL] = -1;
    CHECK(sim_theme_bonus(&c, THEME_POUVOIR_ACHAT, ELEC_POPULAIRES) < b);
    // La campagne utilise les poids électoraux
    c.budget_lvl[BUD_SOCIAL] = 0;
    c.theme = THEME_NONE;
    SET_OPN(c, 500);
    CHECK_EQ(sim_campaign_popularity(&c), 500);
    c.opinion[ELEC_RETRAITES] = 600;
    CHECK_EQ(sim_campaign_popularity(&c), 500 + 28 * 100 / 100);
}

static void test_referendum_weights(void)
{
    Country c;
    RefResult r;
    s32 tot = 0;
    u8 e;
    new_game(&c, ORIENT_CENTRE, DIFF_NORMAL, 7);
    sim_referendum_preview(&c, LOI_POLICE, &r);
    for (e = 0; e < ELEC_COUNT; e++) tot += (s32)elec_weight[e] * r.yes_elec[e];
    CHECK_EQ(r.yes, (tot + 50) / 100);
    // La police (droite) : plus de « oui » chez les cadres et retraités que chez les jeunes
    CHECK(r.yes_elec[ELEC_CADRES] > r.yes_elec[ELEC_JEUNES]);
    CHECK(r.yes_elec[ELEC_RETRAITES] > r.yes_elec[ELEC_JEUNES]);
    // Un effet immédiat pour les retraités compte pour eux seuls
    sim_referendum_preview(&c, LOI_RETRAITES, &r);
    CHECK(r.yes_elec[ELEC_RETRAITES] >= 50);
}

static void test_core_in_institutions(void)
{
    // Les législatives dépendent de l'électorat-cœur : sans alignement ELEC/ORIENT.
    Country a, b;
    LegislativeResult ra, rb;
    long ba = 0, bb = 0;
    u32 seed;
    for (seed = 1; seed <= 60; seed++) {
        new_game(&a, ORIENT_GAUCHE, DIFF_NORMAL, seed);
        b = a;
        SET_OPN(a, 450); SET_OPN(b, 450);
        a.opinion[ELEC_POPULAIRES] = a.opinion[ELEC_JEUNES] = 650;   // cœur de gauche fort
        b.opinion[ELEC_CADRES] = b.opinion[ELEC_RETRAITES] = 650;    // sans effet pour la gauche
        sim_legislatives(&a, &ra);
        sim_legislatives(&b, &rb);
        ba += ra.bloc_after;
        bb += rb.bloc_after;
    }
    CHECK(ba > bb);
}

// --- Budget détaillé -------------------------------------------------------------

static void test_budget_tables(void)
{
    BudgetPlan p, q;
    VoteResult v;
    Country c;
    u8 i, e;

    // Préréglages : valeurs de SPEC_V4 §2.4
    sim_budget_preset_plan(BUDGET_RIGUEUR, &p);
    CHECK_EQ(sim_plan_deficit(&p), -15);
    CHECK_EQ(sim_plan_growth(&p), -25);
    sim_budget_preset_plan(BUDGET_EQUILIBRE, &p);
    CHECK_EQ(sim_plan_deficit(&p), 0);
    CHECK_EQ(sim_plan_growth(&p), 0);
    for (e = 0; e < ELEC_COUNT; e++) CHECK_EQ(sim_plan_opinion(&p, e), 0);
    sim_budget_preset_plan(BUDGET_RELANCE, &p);
    CHECK_EQ(sim_plan_deficit(&p), 12);
    CHECK_EQ(sim_plan_growth(&p), 20);
    CHECK_EQ(sim_budget_plan_preset(&p), BUDGET_RELANCE);
    p.lvl[BUD_IMPOT_ENTREPRISES] = 1;
    CHECK_EQ(sim_budget_plan_preset(&p), BUDGET_PERSO);

    // Popularité pondérée des préréglages : Rigueur ≈ -0,9 point, Relance ≈ -1,3 (V4.14)
    {
        s32 pr = 0, pl = 0;
        for (e = 0; e < ELEC_COUNT; e++) {
            pr += (s32)elec_weight[e] * sim_budget_opinion(BUDGET_RIGUEUR, e);
            pl += (s32)elec_weight[e] * sim_budget_opinion(BUDGET_RELANCE, e);
        }
        CHECK(pr / 100 >= -11 && pr / 100 <= -7);
        // V4.14 : la défiance des marchés (déficit² / 7) ramène l'effet direct de la Relance à
        // environ -1,3 point ; son attrait vient de la croissance et des piliers (voir SIM.md).
        CHECK(pl / 100 >= -16 && pl / 100 <= -10);
    }

    // Asymétrie : +2 ne rapporte que 1,5 fois un cran, -2 coûte 2,5 fois
    CHECK_EQ(sim_budget_line_opinion(BUD_SOCIAL, 1, ELEC_POPULAIRES), 10);
    CHECK_EQ(sim_budget_line_opinion(BUD_SOCIAL, 2, ELEC_POPULAIRES), 15);
    CHECK_EQ(sim_budget_line_opinion(BUD_SOCIAL, -1, ELEC_POPULAIRES), -10);
    CHECK_EQ(sim_budget_line_opinion(BUD_SOCIAL, -2, ELEC_POPULAIRES), -25);
    CHECK_EQ(sim_budget_line_opinion(BUD_SOCIAL, 0, ELEC_POPULAIRES), 0);
    // Impôts : asymétrie inversée (forte hausse très mal ressentie)
    CHECK_EQ(sim_budget_line_opinion(BUD_IMPOT_MENAGES, 2, ELEC_MOYENNES), -25);
    CHECK_EQ(sim_budget_line_opinion(BUD_IMPOT_MENAGES, -2, ELEC_MOYENNES), 15);

    // Déficit, croissance, tension et orientation : linéaires
    for (i = 0; i < BUD_LINES; i++) p.lvl[i] = 0;
    p.lvl[BUD_SOCIAL] = 2;
    q = p; q.lvl[BUD_SOCIAL] = 1;
    CHECK_EQ(sim_plan_deficit(&p), 2 * sim_plan_deficit(&q));
    CHECK_EQ(sim_plan_growth(&p), 2 * sim_plan_growth(&q));
    CHECK_EQ(sim_plan_tension(&p), 2 * sim_plan_tension(&q));
    // piliers asymétriques : +2 = 1,5 x le cran, -2 = -2,5 x
    CHECK_EQ(sim_plan_pillar(&q, PIL_SOCIAL), 3);
    CHECK_EQ(sim_plan_pillar(&p, PIL_SOCIAL), 4);   // 9 / 2, tronqué
    p.lvl[BUD_SOCIAL] = -2;
    CHECK_EQ(sim_plan_pillar(&p, PIL_SOCIAL), -7);  // -22,5 / 3 ... (-5 x 3) / 2 tronqué

    // Sérieux budgétaire : un déficit en moins plaît aux cadres, pas aux populaires
    for (i = 0; i < BUD_LINES; i++) p.lvl[i] = 0;
    p.lvl[BUD_IMPOT_MENAGES] = 1;                       // déficit -6
    CHECK(sim_plan_opinion(&p, ELEC_CADRES) - sim_budget_line_opinion(BUD_IMPOT_MENAGES, 1, ELEC_CADRES) == 6);
    CHECK_EQ(sim_plan_opinion(&p, ELEC_POPULAIRES), sim_budget_line_opinion(BUD_IMPOT_MENAGES, 1, ELEC_POPULAIRES));

    // Budget insincère : malus de vote au-delà de 1,5 point de déficit
    for (i = 0; i < BUD_LINES; i++) p.lvl[i] = 0;
    CHECK_EQ(sim_plan_insincere_malus(&p), 0);
    p.lvl[BUD_SOCIAL] = 2; p.lvl[BUD_EDUCATION] = 2; p.lvl[BUD_SECURITE] = 2; p.lvl[BUD_TRANSITION] = 2;
    CHECK_EQ(sim_plan_deficit(&p), 30);
    CHECK_EQ(sim_plan_insincere_malus(&p), (30 - 15) / 2);
    new_game(&c, ORIENT_GAUCHE, DIFF_FACILE, 11);
    sim_budget_plan_preview(&c, &p, &v);
    {
        VoteResult w;
        BudgetPlan p2 = p;
        parl_vote(&c, sim_plan_orientation(&c, &p2), &w);
        for (i = 0; i < GRP_COUNT; i++)
            if (i != c.player_group) CHECK_EQ(v.score[i], w.score[i] - sim_plan_insincere_malus(&p));
        CHECK_EQ(v.score[c.player_group], w.score[c.player_group]);
    }
}

static void test_budget_vote_plan(void)
{
    Country c;
    BudgetPlan p;
    VoteResult v;
    new_game(&c, ORIENT_CENTRE, DIFF_FACILE, 21);
    c.relation[GRP_G1] = c.relation[GRP_G2] = c.relation[GRP_G3] = 100;
    c.relation[GRP_G4] = c.relation[GRP_G5] = 100;
    sim_budget_preset_plan(BUDGET_EQUILIBRE, &p);
    p.lvl[BUD_TRANSITION] = 2;
    CHECK_EQ(sim_budget_plan_vote(&c, &p, &v), BUDGET_ERR_NOT_DUE);
    to_budget_quarter(&c);
    CHECK(sim_budget_due(&c));
    CHECK_EQ(sim_budget_plan_vote(&c, &p, &v), BUDGET_OK);
    CHECK(v.adopted);
    CHECK_EQ(c.budget_done, 1);
    CHECK_EQ(c.budget, BUDGET_PERSO);
    CHECK_EQ(c.budget_lvl[BUD_TRANSITION] == 2 || c.bud_amend_line == BUD_TRANSITION + 1, 1);
    // 49.3 : 4 CP, plan appliqué, défiance
    new_game(&c, ORIENT_CENTRE, DIFF_NORMAL, 22);
    to_budget_quarter(&c);
    c.cp = 9;
    c.cohab = 0;
    p.lvl[BUD_SOCIAL] = -2;
    CHECK_EQ(sim_budget_plan_force(&c, &p), BUDGET_OK);
    CHECK_EQ(c.cp, 5);
    CHECK_EQ(c.budget_lvl[BUD_SOCIAL] != 0, 1);
    c.budget_done = 0;
    c.cp = 3;
    CHECK_EQ(sim_budget_plan_force(&c, &p), BUDGET_ERR_CP);
    // crans hors bornes ramenés dans -2..+2
    c.cp = 9;
    p.lvl[BUD_SOCIAL] = 9;
    CHECK_EQ(sim_budget_plan_force(&c, &p), BUDGET_OK);
    CHECK(c.budget_lvl[BUD_SOCIAL] <= 2);
}

static void test_reconduction(void)
{
    Country c;
    BudgetPlan p;
    TurnReport r;
    int i;
    new_game(&c, ORIENT_CENTRE, DIFF_NORMAL, 31);
    // plan extrême imposé une fois, puis laissé se reconduire
    for (i = 0; i < BUD_LINES; i++) c.budget_lvl[i] = (s8)(i < BUD_IMPOT_MENAGES ? 2 : -2);
    to_budget_quarter(&c);
    sim_end_turn(&c, &r);   // fin d'année sans budget adopté
    CHECK_EQ(r.budget_reconduit, 1);
    for (i = 0; i < BUD_IMPOT_MENAGES; i++) CHECK_EQ(c.budget_lvl[i], 1);
    CHECK_EQ(c.budget_lvl[BUD_IMPOT_MENAGES], -2);   // impôts inchangés
    CHECK_EQ(c.budget_lvl[BUD_IMPOT_ENTREPRISES], -2);
    for (i = 0; i < 4; i++) sim_end_turn(&c, &r);
    for (i = 0; i < BUD_IMPOT_MENAGES; i++) CHECK_EQ(c.budget_lvl[i], 0);
    // reconduction d'un plan négatif : remonte vers 0
    sim_budget_preset_plan(BUDGET_RIGUEUR, &p);
    sim_budget_reconduct_plan(&p);
    CHECK_EQ(p.lvl[BUD_SOCIAL], 0);
    CHECK_EQ(p.lvl[BUD_TRANSITION], 0);
    CHECK_EQ(p.lvl[BUD_IMPOT_MENAGES], 1);
}

// Sénat sans majorité pour le bloc présidentiel (gauche : G1-G3 = 120 sièges).
static void senate_opposed(Country *c)
{
    static const u16 s[GRP_COUNT] = { 30, 40, 50, 120, 108 };
    u8 g;
    for (g = 0; g < GRP_COUNT; g++) c->senate[g] = s[g];
}

static void test_senate_amendment(void)
{
    Country c;
    BudgetPlan p;
    VoteResult v;
    TurnReport r;
    new_game(&c, ORIENT_GAUCHE, DIFF_NORMAL, 9);
    senate_opposed(&c);
    CHECK(!sim_senate_has_majority(&c));
    // Relance (Soc +1, Édu +1, Trans +1) : le Sénat rabote la ligne la plus à gauche (Social, -15)
    sim_budget_preset_plan(BUDGET_RELANCE, &p);
    CHECK_EQ(sim_budget_senate_amend_line(&c, &p), BUD_SOCIAL + 1);
    // Rigueur : aucune ligne ne pousse à gauche, pas d'amendement
    {
        BudgetPlan q;
        sim_budget_preset_plan(BUDGET_RIGUEUR, &q);
        CHECK_EQ(sim_budget_senate_amend_line(&c, &q), 0);
    }
    to_budget_quarter(&c);
    c.cp = 10;
    CHECK_EQ(sim_budget_plan_force(&c, &p), BUDGET_OK);
    CHECK_EQ(sim_budget_amended_line(&c), BUD_SOCIAL + 1);
    CHECK_EQ(c.budget_lvl[BUD_SOCIAL], 0);
    CHECK_EQ(c.budget_lvl[BUD_EDUCATION], 1);
    CHECK_EQ(c.budget, BUDGET_PERSO);
    // Lecture définitive : 1 CP, plan rétabli
    CHECK_EQ(c.cp, 6);
    CHECK_EQ(sim_budget_definitive_reading(&c), BUDGET_OK);
    CHECK_EQ(c.cp, 5);
    CHECK_EQ(c.budget_lvl[BUD_SOCIAL], 1);
    CHECK_EQ(c.budget, BUDGET_RELANCE);
    CHECK_EQ(sim_budget_amended_line(&c), 0);
    CHECK_EQ(sim_budget_definitive_reading(&c), BUDGET_ERR_NO_AMEND);
    // Sans lecture définitive : l'amendement est annoncé dans le compte rendu de fin d'année
    new_game(&c, ORIENT_GAUCHE, DIFF_NORMAL, 9);
    senate_opposed(&c);
    to_budget_quarter(&c);
    c.cp = 10;
    CHECK_EQ(sim_budget_plan_force(&c, &p), BUDGET_OK);
    sim_end_turn(&c, &r);
    CHECK_EQ(r.senate_amend, BUD_SOCIAL + 1);
    CHECK_EQ(c.bud_amend_line, 0);
    CHECK_EQ(c.budget_lvl[BUD_SOCIAL], 0);
    // Pas de CP : lecture refusée
    new_game(&c, ORIENT_GAUCHE, DIFF_NORMAL, 9);
    senate_opposed(&c);
    to_budget_quarter(&c);
    c.cp = 5;
    sim_budget_plan_force(&c, &p);
    c.cp = 0;
    CHECK_EQ(sim_budget_definitive_reading(&c), BUDGET_ERR_CP);
    // Si le bloc présidentiel a la majorité au Sénat : aucun amendement
    new_game(&c, ORIENT_GAUCHE, DIFF_NORMAL, 5);
    c.senate[c.player_group] = 348;
    c.senate[0] = c.senate[1] = c.senate[2] = c.senate[3] = c.senate[4] = 0;
    c.senate[c.player_group] = 348;
    CHECK(sim_senate_has_majority(&c));
    CHECK_EQ(sim_budget_senate_amend_line(&c, &p), 0);
    (void)v;
}

static void test_cohab_budget(void)
{
    Country c;
    BudgetPlan p;
    TurnReport r;
    u8 camp;
    int i;
    new_game(&c, ORIENT_DROITE, DIFF_DIFFICILE, 41);
    cohab_start(&c);
    camp = sim_cohab_camp(&c);
    CHECK(c.cohab);
    CHECK_EQ(sim_budget_due(&c), 0);
    sim_cohab_plan(&c, &p);
    if (camp == ORIENT_GAUCHE) {
        CHECK(p.lvl[BUD_SOCIAL] == 1 && p.lvl[BUD_EDUCATION] == 1 && p.lvl[BUD_SECURITE] == 0
              && p.lvl[BUD_TRANSITION] == 1 && p.lvl[BUD_IMPOT_MENAGES] == 0 && p.lvl[BUD_IMPOT_ENTREPRISES] == 1);
    } else {
        CHECK(p.lvl[BUD_SOCIAL] == -1 && p.lvl[BUD_SECURITE] == 1 && p.lvl[BUD_IMPOT_ENTREPRISES] == -1);
    }
    // La ligne Sécurité et défense reste au président (±1 cran, bornée à ±2)
    sim_cohab_set_defense(&c, 1);
    {
        BudgetPlan q;
        sim_cohab_plan(&c, &q);
        CHECK_EQ(q.lvl[BUD_SECURITE], p.lvl[BUD_SECURITE] + 1);
    }
    // Fin d'année : le plan du PM, ligne défense ajustée, entre en vigueur sans CP
    {
        u8 cp = c.cp;
        BudgetPlan q;
        sim_cohab_plan(&c, &q);
        for (i = 0; i < 4; i++) sim_end_turn(&c, &r);
        if (c.status == STATUS_PLAYING && c.cohab) {
            CHECK_EQ(r.budget_gov, 1);
            for (i = 0; i < BUD_LINES; i++) CHECK_EQ(c.budget_lvl[i], q.lvl[i]);
            CHECK(c.cp >= cp);
        }
    }
}

// --- Sauvegarde --------------------------------------------------------------------

static void test_save_version(void)
{
    u8 src[64], blob[GAMESAVE_HEADER + 64];
    u16 i, len;
    CHECK_EQ(GAMESAVE_VERSION, 3);
    for (i = 0; i < 64; i++) src[i] = (u8)i;
    len = gamesave_pack(src, 64, 0x1234, 1, blob);
    CHECK_EQ(gamesave_unpack(0, 64, 0x1234, blob, len), 1);
    // une sauvegarde V3 (version 1) est refusée proprement, sans migration
    blob[4] = 1;
    CHECK_EQ(gamesave_unpack(0, 64, 0x1234, blob, len), 0);
    CHECK(sizeof(Country) <= 220);
}

static void test_bounds_long_run(void)
{
    // Parties longues avec plans variés : tout reste dans les bornes.
    Country c;
    u32 seed;
    int t, i, e;
    for (seed = 1; seed <= 60; seed++) {
        BudgetPlan p;
        VoteResult v;
        new_game(&c, (u8)(seed % 3), (u8)(seed % 3), seed);
        for (t = 0; t < 40 && c.status == STATUS_PLAYING; t++) {
            if (sim_mandate_over(&c)) break;
            if (sim_budget_due(&c)) {
                for (i = 0; i < BUD_LINES; i++) p.lvl[i] = (s8)((int)((seed * 7 + t * 3 + i * 5) % 5) - 2);
                sim_budget_plan_vote(&c, &p, &v);
            }
            sim_end_turn(&c, 0);
            for (e = 0; e < ELEC_COUNT; e++)
                CHECK(c.opinion[e] >= OPINION_MIN && c.opinion[e] <= OPINION_MAX);
            for (i = 0; i < BUD_LINES; i++) CHECK(c.budget_lvl[i] >= -2 && c.budget_lvl[i] <= 2);
            for (i = 0; i < PIL_TOTAL; i++) CHECK(c.pillars[i] >= PILLAR_MIN && c.pillars[i] <= PILLAR_MAX);
        }
    }
}

void test_sim_v4(void)
{
    printf("  V4 : électorats et budget détaillé\n");
    test_electorates();
    test_effect_keys();
    test_themes();
    test_referendum_weights();
    test_core_in_institutions();
    test_budget_tables();
    test_budget_vote_plan();
    test_reconduction();
    test_senate_amendment();
    test_cohab_budget();
    test_save_version();
    test_bounds_long_run();
}
