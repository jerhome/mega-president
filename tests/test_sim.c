#include "test.h"
#include "sim/sim.h"

// Vérifie que toutes les grandeurs sont dans leurs bornes.
static int in_bounds(const Country *c)
{
    int i, ok = 1;
    for (i = 0; i < ELEC_COUNT; i++)
        ok &= c->opinion[i] >= OPINION_MIN && c->opinion[i] <= OPINION_MAX;
    for (i = 0; i < PIL_COUNT; i++)
        ok &= c->pillars[i] >= PILLAR_MIN && c->pillars[i] <= PILLAR_MAX;
    for (i = 0; i < JAUGE_COUNT; i++) ok &= c->gauges[i] <= GAUGE_MAX;
    for (i = 0; i < GRP_COUNT; i++) ok &= c->relation[i] <= RELATION_MAX;
    ok &= c->growth >= GROWTH_MIN && c->growth <= GROWTH_MAX;
    ok &= c->unemployment >= UNEMP_MIN && c->unemployment <= UNEMP_MAX;
    ok &= c->debt >= DEBT_MIN && c->debt <= DEBT_MAX;
    ok &= c->cp <= CP_MAX;
    ok &= c->n_pending <= SIM_MAX_PENDING;
    return ok;
}

static void test_init(void)
{
    Country c;
    u16 seats = 0;
    int g;
    sim_init(&c, ORIENT_GAUCHE, 1, 4);
    CHECK(in_bounds(&c));
    CHECK_EQ(c.turns_per_year, 4);
    CHECK_EQ(sim_mandate_turns(&c), 20);
    CHECK_EQ(c.player_group, GRP_G2);
    CHECK_EQ(c.relation[GRP_G2], 80);
    CHECK_EQ(c.relation[GRP_G1], 55);
    CHECK_EQ(c.relation[GRP_G3], 55);
    CHECK_EQ(c.relation[GRP_G4], 30);
    CHECK_EQ(c.relation[GRP_G5], 15);
    for (g = 0; g < GRP_COUNT; g++) seats += c.seats[g];
    CHECK_EQ(seats, SEATS_TOTAL);
    CHECK(c.opinion[ELEC_POPULAIRES] > c.opinion[ELEC_AISEES]);

    sim_init(&c, ORIENT_CENTRE, 1, 4);
    CHECK_EQ(c.player_group, GRP_G3);
    CHECK_EQ(c.relation[GRP_G1], 30);
    CHECK_EQ(c.relation[GRP_G5], 30);
    sim_init(&c, ORIENT_DROITE, 1, 4);
    CHECK_EQ(c.player_group, GRP_G4);
    CHECK_EQ(c.relation[GRP_G1], 15);
    CHECK(c.opinion[ELEC_AISEES] > c.opinion[ELEC_POPULAIRES]);

    sim_init(&c, ORIENT_CENTRE, 1, 12);
    CHECK_EQ(sim_mandate_turns(&c), 60);
}

static void test_popularity(void)
{
    Country c;
    sim_init(&c, ORIENT_CENTRE, 3, 4);
    c.opinion[0] = 500; c.opinion[1] = 300; c.opinion[2] = 1000;
    CHECK_EQ(sim_popularity(&c), 520);   // (2000 + 1200 + 2000) / 10
    c.opinion[0] = c.opinion[1] = c.opinion[2] = 0;
    CHECK_EQ(sim_popularity(&c), 0);
    c.opinion[0] = c.opinion[1] = c.opinion[2] = 1000;
    CHECK_EQ(sim_popularity(&c), 1000);
}

static void test_effects(void)
{
    Country c;
    Effect list[2] = { { EFF_ECOLOGIE, 4 }, { EFF_TENSION, 10 } };
    sim_init(&c, ORIENT_CENTRE, 5, 4);
    c.opinion[0] = c.opinion[1] = c.opinion[2] = 400;
    c.growth = 100; c.unemployment = 700; c.debt = 1000;
    c.pillars[PIL_ECOLOGIE] = 500; c.gauges[JAUGE_TENSION] = 20; c.cp = 5;
    c.relation[GRP_G4] = 50;

    sim_apply_effect(&c, EFF_POP_AISEES, -3);   CHECK_EQ(c.opinion[2], 370);
    sim_apply_effect(&c, EFF_POP_TOUS, 2);      CHECK_EQ(c.opinion[0], 420);
                                                CHECK_EQ(c.opinion[2], 390);
    sim_apply_effect(&c, EFF_CROISSANCE, 2);    CHECK_EQ(c.growth, 120);
    sim_apply_effect(&c, EFF_CHOMAGE, -3);      CHECK_EQ(c.unemployment, 670);
    sim_apply_effect(&c, EFF_DETTE, 1);         CHECK_EQ(c.debt, 1010);
    sim_apply_effect(&c, EFF_CP, -1);           CHECK_EQ(c.cp, 4);
    sim_apply_effect(&c, EFF_REL_G4, -5);       CHECK_EQ(c.relation[GRP_G4], 45);
    sim_apply_effects(&c, list, 2);
    CHECK_EQ(c.pillars[PIL_ECOLOGIE], 540);
    CHECK_EQ(c.gauges[JAUGE_TENSION], 30);

    // Bornage
    sim_apply_effect(&c, EFF_CP, 100);          CHECK_EQ(c.cp, CP_MAX);
    sim_apply_effect(&c, EFF_CP, -100);         CHECK_EQ(c.cp, 0);
    sim_apply_effect(&c, EFF_POP_TOUS, 127);    CHECK_EQ(c.opinion[1], OPINION_MAX);
    sim_apply_effect(&c, EFF_POP_TOUS, -128);   CHECK_EQ(c.opinion[1], OPINION_MIN);
    sim_apply_effect(&c, EFF_TENSION, 127);     CHECK_EQ(c.gauges[JAUGE_TENSION], GAUGE_MAX);
    sim_apply_effect(&c, EFF_REL_G1, -128);     CHECK_EQ(c.relation[GRP_G1], 0);
    sim_apply_effect(&c, EFF_CHOMAGE, -128);    CHECK_EQ(c.unemployment, UNEMP_MIN);
    sim_apply_effect(&c, EFF_SECURITE, 127);
    sim_apply_effect(&c, EFF_SECURITE, 127);    CHECK_EQ(c.pillars[PIL_SECURITE], PILLAR_MAX);
    sim_apply_effect(&c, EFF_NONE, 5);          // ignoré
    sim_apply_effect(&c, 200, 5);               // clé inconnue ignorée
    CHECK(in_bounds(&c));
    CHECK_EQ(sim_effect_scale(200), 0);
}

static void test_laws(void)
{
    Country c;
    VoteResult v;
    int i, k;

    // Catalogue cohérent
    for (i = 0; i < LOI_COUNT; i++) {
        const LawDef *l = law_get((u8)i);
        CHECK(l != 0);
        CHECK_EQ(l->id, i);
        CHECK(l->cost >= 2 && l->cost <= 6);
        CHECK(l->delay_q >= 1 && l->delay_q <= 4);
        CHECK(l->n_effects <= LAW_MAX_EFFECTS && l->n_immediate <= LAW_MAX_IMMEDIATE);
        for (k = 0; k < l->n_effects; k++) CHECK(sim_effect_scale(l->effects[k].key) > 0);
        for (k = 0; k < l->n_immediate; k++) CHECK(sim_effect_scale(l->immediate[k].key) > 0);
    }
    CHECK(law_get(LOI_COUNT) == 0);

    // Hausse du SMIC par un joueur de gauche : adoptée, effets différés de 2 tours.
    sim_init(&c, ORIENT_GAUCHE, 9, 4);
    c.cp = 10;
    {
        s16 pop0 = c.opinion[ELEC_POPULAIRES], soc0 = c.pillars[PIL_SOCIAL];
        u8 rep[SIM_MAX_PENDING], n;
        CHECK_EQ(sim_propose_law(&c, LOI_SMIC, &v), LAW_OK);
        CHECK(v.adopted);
        CHECK_EQ(c.cp, 7);
        CHECK_EQ(c.opinion[ELEC_POPULAIRES], pop0 + 40);
        CHECK_EQ(c.n_pending, 1);
        CHECK(law_is_adopted(&c, LOI_SMIC));
        CHECK_EQ(sim_propose_law(&c, LOI_SMIC, &v), LAW_ERR_ADOPTED);
        n = law_process_pending(&c, rep);
        CHECK_EQ(n, 0);
        CHECK_EQ(c.pillars[PIL_SOCIAL], soc0);
        n = law_process_pending(&c, rep);
        CHECK_EQ(n, 1);
        CHECK_EQ(rep[0], LOI_SMIC);
        CHECK_EQ(c.pillars[PIL_SOCIAL], soc0 + 80);
        CHECK_EQ(c.n_pending, 0);
    }

    // CP insuffisant, identifiant invalide
    sim_init(&c, ORIENT_DROITE, 9, 4);
    c.cp = 1;
    CHECK_EQ(sim_propose_law(&c, LOI_IS, &v), LAW_ERR_CP);
    CHECK_EQ(c.cp, 1);
    CHECK_EQ(sim_propose_law(&c, 99, &v), LAW_ERR_ID);

    // Mode mensuel : délai × 3
    sim_init(&c, ORIENT_DROITE, 9, 12);
    CHECK_EQ(law_delay_turns(&c, law_get(LOI_CIR)), 12);
    sim_init(&c, ORIENT_DROITE, 9, 4);
    CHECK_EQ(law_delay_turns(&c, law_get(LOI_CIR)), 4);

    // Rejet : CP perdu, défiance en hausse, rien en attente.
    sim_init(&c, ORIENT_DROITE, 9, 4);
    c.cp = 10;
    {
        u8 d0 = c.gauges[JAUGE_DEFIANCE];
        CHECK_EQ(sim_propose_law(&c, LOI_SMIC, &v), LAW_OK);
        CHECK(!v.adopted);
        CHECK_EQ(c.cp, 7);
        CHECK_EQ(c.gauges[JAUGE_DEFIANCE], d0 + 5);
        CHECK_EQ(c.n_pending, 0);
        CHECK(!law_is_adopted(&c, LOI_SMIC));
    }
}

static void test_vote(void)
{
    Country c;
    VoteResult v;
    int g;
    sim_init(&c, ORIENT_CENTRE, 2, 4);

    CHECK_EQ(parl_affinity(0, 0), 50);
    CHECK_EQ(parl_affinity(-100, 80), -40);
    CHECK_EQ(parl_affinity(100, -80), -40);

    // Majorité exacte à 289
    for (g = 0; g < GRP_COUNT; g++) { c.seats[g] = 0; c.relation[g] = 0; }
    c.seats[GRP_G1] = 289; c.seats[GRP_G2] = 288;
    c.relation[GRP_G1] = 100;
    parl_vote(&c, 0, &v);
    CHECK_EQ(v.votes_pour, 289);
    CHECK_EQ(v.votes_contre, 288);
    CHECK(v.adopted);
    CHECK(v.pour[GRP_G1] && !v.pour[GRP_G2]);
    c.seats[GRP_G1] = 288; c.seats[GRP_G2] = 289;
    parl_vote(&c, 0, &v);
    CHECK_EQ(v.votes_pour, 288);
    CHECK(!v.adopted);

    // Seuil : relation + affinité >= VOTE_THRESHOLD
    c.relation[GRP_G1] = VOTE_THRESHOLD - 50;   // + affinité 50 (loi à -80)
    parl_vote(&c, -80, &v);
    CHECK(v.pour[GRP_G1]);
    c.relation[GRP_G1] = VOTE_THRESHOLD - 51;
    parl_vote(&c, -80, &v);
    CHECK(!v.pour[GRP_G1]);

    // Évolution des relations après adoption
    sim_init(&c, ORIENT_GAUCHE, 2, 4);
    parl_vote(&c, -60, &v);
    CHECK(v.adopted);
    {
        u8 r1 = c.relation[GRP_G1], r5 = c.relation[GRP_G5];
        parl_after_vote(&c, -60, &v);
        CHECK(c.relation[GRP_G1] > r1);
        CHECK(c.relation[GRP_G5] <= r5);
    }
}

static void test_election(void)
{
    Country c;
    ElectionResult r;
    u32 seed;
    int i, o;
    u8 opp[4];

    // Adversaires : jamais le candidat du camp du joueur.
    for (o = 0; o < ORIENT_COUNT; o++) {
        election_opponents((u8)o, opp);
        for (i = 0; i < 4; i++) {
            CHECK(opp[i] < CAND_POOL_COUNT);
            if (o == ORIENT_DROITE) CHECK(opp[i] != CAND_DROITE);
            else CHECK(opp[i] != CAND_CENTRE);
        }
        CHECK(opp[0] != opp[1] && opp[1] != opp[2] && opp[2] != opp[3]);
    }

    for (seed = 1; seed <= 600; seed++) {
        int sum = 0;
        sim_init(&c, (u8)(seed % 3), seed, 4);
        c.opinion[0] = (s16)(seed * 7 % 1001);   // popularités variées
        c.opinion[1] = (s16)(seed * 13 % 1001);
        c.unemployment = (s16)(UNEMP_MIN + seed * 37 % 2000);
        sim_election(&c, &r);
        CHECK_EQ(r.cand[0], CAND_JOUEUR);
        for (i = 0; i < ELECTION_CANDIDATES; i++) {
            CHECK(r.round1[i] >= 0);
            sum += r.round1[i];
        }
        for (i = 1; i < ELECTION_CANDIDATES; i++) {
            if (c.orientation == ORIENT_DROITE) CHECK(r.cand[i] != CAND_DROITE);
            else CHECK(r.cand[i] != CAND_CENTRE);
        }
        CHECK_EQ(sum, 1000);
        CHECK_EQ(r.round2[0] + r.round2[1], 1000);
        CHECK(r.finalist[0] != r.finalist[1]);
        if (r.elected)
            CHECK((r.finalist[0] == CAND_JOUEUR && r.round2[0] > 500) ||
                  (r.finalist[1] == CAND_JOUEUR && r.round2[1] > 500));
    }

    // Plus populaire => meilleur score moyen au 1er tour.
    {
        long lo = 0, hi = 0;
        for (seed = 1; seed <= 200; seed++) {
            sim_init(&c, ORIENT_CENTRE, seed, 4);
            c.opinion[0] = c.opinion[1] = c.opinion[2] = 250;
            sim_election(&c, &r); lo += r.round1[0];
            sim_init(&c, ORIENT_CENTRE, seed, 4);
            c.opinion[0] = c.opinion[1] = c.opinion[2] = 550;
            sim_election(&c, &r); hi += r.round1[0];
        }
        CHECK(hi > lo);
    }

    // Score de fin de mandat
    sim_init(&c, ORIENT_CENTRE, 4, 4);
    c.opinion[0] = c.opinion[1] = c.opinion[2] = 400;
    CHECK_EQ(sim_mandate_score(&c, 0), 400);
    CHECK_EQ(sim_mandate_score(&c, 1), 900);
    c.pillars[PIL_ECOLOGIE] = (s16)(c.pillars_start[PIL_ECOLOGIE] + 100);   // +10 pts
    CHECK_EQ(sim_mandate_score(&c, 0), 450);
    c.unemployment = (s16)(c.unemployment_start - 100);                     // -1 pt
    CHECK_EQ(sim_mandate_score(&c, 0), 500);
    c.opinion[0] = c.opinion[1] = c.opinion[2] = 0;
    c.debt = DEBT_MAX;
    CHECK_EQ(sim_mandate_score(&c, 0), 0);
}

// Joueur automatique simple : propose dans un ordre de préférence chaque loi
// abordable dont l'aperçu du vote est favorable.
static const u8 pref[ORIENT_COUNT][LOI_COUNT] = {
    { LOI_SMIC, LOI_RENOV, LOI_CIR, LOI_POLICE, LOI_IS },
    { LOI_RENOV, LOI_CIR, LOI_POLICE, LOI_IS, LOI_SMIC },
    { LOI_IS, LOI_POLICE, LOI_CIR, LOI_RENOV, LOI_SMIC },
};

static void auto_turn(Country *c, int active)
{
    int i;
    VoteResult v;
    if (!active) return;
    for (i = 0; i < LOI_COUNT; i++) {
        u8 id = pref[c->orientation][i];
        if (law_can_propose(c, id) != LAW_OK) continue;
        parl_vote(c, law_get(id)->orientation, &v);
        if (v.adopted) sim_propose_law(c, id, &v);
    }
}

static void play(u8 orient, u32 seed, int active, Country *c, ElectionResult *r)
{
    TurnReport rep;
    sim_init(c, orient, seed, 4);
    while (!sim_mandate_over(c)) {
        auto_turn(c, active);
        sim_end_turn(c, &rep);
        CHECK(in_bounds(c));
    }
    sim_election(c, r);
}

static void test_reproducible(void)
{
    Country a, b;
    ElectionResult ra, rb;
    play(ORIENT_GAUCHE, 777, 1, &a, &ra);
    play(ORIENT_GAUCHE, 777, 1, &b, &rb);
    CHECK_EQ(sim_popularity(&a), sim_popularity(&b));
    CHECK_EQ(a.debt, b.debt);
    CHECK_EQ(a.unemployment, b.unemployment);
    CHECK_EQ(ra.round1[0], rb.round1[0]);
    CHECK_EQ(ra.round2[0], rb.round2[0]);
    CHECK_EQ(ra.elected, rb.elected);
    CHECK_EQ(a.rng.state, b.rng.state);
}

#define AUTO_SEEDS 400

static void test_autoplay(void)
{
    static const char *names[ORIENT_COUNT] = { "gauche", "centre", "droite" };
    int o, active;
    printf("  Partie automatique (%d graines, 20 tours) :\n", AUTO_SEEDS);
    printf("  %-7s %-6s %5s %5s %5s %6s %5s %5s %5s %6s\n", "orient", "joueur",
           "pop%", "chom", "dette", "pil", "lois", "2ndT", "elu%", "score");
    for (active = 1; active >= 0; active--) {
        for (o = 0; o < ORIENT_COUNT; o++) {
            long pop = 0, un = 0, debt = 0, pil = 0, laws = 0, second = 0, elected = 0, score = 0;
            u32 seed;
            for (seed = 1; seed <= AUTO_SEEDS; seed++) {
                Country c;
                ElectionResult r;
                int k;
                play((u8)o, seed * 2654435761u, active, &c, &r);
                pop += sim_popularity(&c);
                un += c.unemployment;
                debt += c.debt;
                pil += sim_pillar_progress(&c);
                for (k = 0; k < LOI_COUNT; k++) laws += law_is_adopted(&c, (u8)k);
                second += (r.finalist[0] == CAND_JOUEUR || r.finalist[1] == CAND_JOUEUR);
                elected += r.elected;
                score += sim_mandate_score(&c, r.elected);
            }
            printf("  %-7s %-6s %5.1f %5.2f %5.1f %+6.1f %5.1f %5.0f %5.0f %6ld\n",
                   names[o], active ? "actif" : "passif",
                   pop / 10.0 / AUTO_SEEDS, un / 100.0 / AUTO_SEEDS,
                   debt / 10.0 / AUTO_SEEDS, pil / 10.0 / AUTO_SEEDS,
                   (double)laws / AUTO_SEEDS, 100.0 * second / AUTO_SEEDS,
                   100.0 * elected / AUTO_SEEDS, score / AUTO_SEEDS);
            if (active) {
                // Garde-fous d'équilibrage (larges) pour un joueur raisonnable.
                CHECK(elected > AUTO_SEEDS / 10);
                CHECK(elected < AUTO_SEEDS * 9 / 10);
            }
        }
    }
}

void test_sim(void)
{
    test_init();
    test_popularity();
    test_effects();
    test_laws();
    test_vote();
    test_election();
    test_reproducible();
    test_autoplay();
}
