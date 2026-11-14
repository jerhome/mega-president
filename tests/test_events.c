#include "test.h"
#include "sim/sim.h"
#include "sim/events.h"

static int in_bounds(const Country *c)
{
    int i, ok = 1;
    for (i = 0; i < ELEC_COUNT; i++)
        ok &= c->opinion[i] >= OPINION_MIN && c->opinion[i] <= OPINION_MAX;
    for (i = 0; i < PIL_COUNT; i++)
        ok &= c->pillars[i] >= PILLAR_MIN && c->pillars[i] <= PILLAR_MAX;
    for (i = 0; i < JAUGE_COUNT; i++) ok &= c->gauges[i] <= GAUGE_MAX;
    ok &= c->unemployment >= UNEMP_MIN && c->unemployment <= UNEMP_MAX;
    ok &= c->debt >= DEBT_MIN && c->debt <= DEBT_MAX && c->cp <= CP_MAX;
    return ok;
}

static void test_catalog(void)
{
    int i, k, j;
    CHECK(EVT_COUNT >= 10 && EVT_COUNT <= 32);
    CHECK(event_get(EVT_COUNT) == 0);
    for (i = 0; i < EVT_COUNT; i++) {
        const EventDef *e = event_get((u8)i);
        CHECK(e != 0);
        CHECK_EQ(e->id, i);
        CHECK(e->weight == 1 || e->weight == 2 || e->weight == 4);
        CHECK(e->n_choices >= 2 && e->n_choices <= EVENT_MAX_CHOICES);
        CHECK(e->cond.var <= EVV_SECURITE);
        for (k = 0; k < e->n_choices; k++) {
            CHECK(e->choices[k].n_effects >= 1 && e->choices[k].n_effects <= EVENT_MAX_EFFECTS);
            for (j = 0; j < e->choices[k].n_effects; j++)
                CHECK(sim_effect_scale(e->choices[k].effects[j].key) > 0);
        }
    }
}

static void test_conditions(void)
{
    Country c;
    const EventDef *g = event_get(EVT_GREVE_GENERALE);
    const EventDef *em = event_get(EVT_EMEUTES);
    int i;
    sim_init(&c, ORIENT_CENTRE, 5, 4);
    CHECK_EQ(g->cond.var, EVV_TENSION);
    CHECK_EQ(g->cond.value, 70);
    c.gauges[JAUGE_TENSION] = 70;
    CHECK(!events_cond_ok(&c, g));       // strictement supérieur
    c.gauges[JAUGE_TENSION] = 71;
    CHECK(events_cond_ok(&c, g));
    CHECK(events_cond_ok(&c, em));
    c.gauges[JAUGE_TENSION] = 50;
    CHECK(!events_cond_ok(&c, em));
    CHECK(events_cond_ok(&c, event_get(EVT_CANICULE)));   // sans condition

    // Jamais d'événement conditionnel tiré si sa condition est fausse.
    for (i = 1; i <= 300; i++) {
        int t;
        sim_init(&c, (u8)(i % 3), (u32)i, 4);
        for (t = 0; t < 19; t++) {
            s16 id;
            c.turn = (u8)(t + 1);
            c.gauges[JAUGE_TENSION] = 20;          // sous tous les seuils
            id = events_draw(&c);
            CHECK(id != EVT_GREVE_GENERALE && id != EVT_EMEUTES);
        }
    }
    // Tension très haute : les événements de tension sont tirables.
    {
        int seen_g = 0, seen_e = 0;
        for (i = 1; i <= 300; i++) {
            sim_init(&c, ORIENT_CENTRE, (u32)i, 4);
            c.turn = 3;
            c.gauges[JAUGE_TENSION] = 90;
            {
                s16 id = events_draw(&c);
                seen_g += id == EVT_GREVE_GENERALE;
                seen_e += id == EVT_EMEUTES;
            }
        }
        CHECK(seen_g > 0 && seen_e > 0);
    }
}

static void test_choose(void)
{
    Country c;
    const EventDef *e = event_get(EVT_CANICULE);
    sim_init(&c, ORIENT_CENTRE, 8, 4);
    c.pillars[PIL_SOCIAL] = 500; c.debt = 1000; c.opinion[ELEC_POPULAIRES] = 400;
    CHECK_EQ(e->n_choices, 3);
    CHECK_EQ(events_choose(&c, EVT_CANICULE, 0), 1);   // SOCIAL +3, DETTE +2, POP_POPULAIRES +2
    CHECK_EQ(c.pillars[PIL_SOCIAL], 530);
    CHECK_EQ(c.debt, 1020);
    CHECK_EQ(c.opinion[ELEC_POPULAIRES], 420);
    CHECK_EQ(events_choose(&c, EVT_CANICULE, 3), 0);    // choix invalide
    CHECK_EQ(events_choose(&c, EVT_COUNT, 0), 0);       // événement invalide
    CHECK_EQ(c.pillars[PIL_SOCIAL], 530);
}

static void test_draw(void)
{
    Country c;
    int i, t, n, tot;
    u32 seed;

    // Pas d'événement au tout premier tour.
    sim_init(&c, ORIENT_CENTRE, 3, 4);
    CHECK_EQ(events_draw(&c), -1);

    // Non-répétition, une occurrence par mandat pour les inconditionnels.
    for (seed = 1; seed <= 300; seed++) {
        u32 count[EVT_COUNT];
        int last = -1;
        for (i = 0; i < EVT_COUNT; i++) count[i] = 0;
        sim_init(&c, (u8)(seed % 3), seed, 4);
        for (t = 1; t < 20; t++) {
            s16 id;
            c.turn = (u8)t;
            if (t % 5 == 0) c.gauges[JAUGE_TENSION] = 95;
            else c.gauges[JAUGE_TENSION] = 20;
            id = events_draw(&c);
            if (id < 0) continue;
            CHECK(id != last);
            last = id;
            count[id]++;
            if (event_get((u8)id)->cond.var == EVV_NONE) CHECK(count[id] == 1);
        }
    }

    // Fréquence : ~1 par tour à 4 tours/an (limitée par le stock), ~1/3 à 12.
    CHECK_EQ(events_chance_pct(&c), 100);
    sim_init(&c, ORIENT_CENTRE, 3, 12);
    CHECK_EQ(events_chance_pct(&c), 33);
    n = 0; tot = 0;
    for (seed = 1; seed <= 200; seed++) {
        sim_init(&c, ORIENT_CENTRE, seed, 12);
        for (t = 1; t < 10; t++) {   // début de mandat : stock non épuisé
            c.turn = (u8)t;
            tot++;
            if (events_draw(&c) >= 0) n++;
        }
    }
    CHECK(n > tot * 25 / 100 && n < tot * 42 / 100);
    printf("  Événements : %d sur %d tours (12 tours/an, 9 premiers tours)\n", n, tot);

    // Reproductibilité
    {
        Country a, b;
        int ok = 1;
        sim_init(&a, ORIENT_GAUCHE, 42, 4);
        sim_init(&b, ORIENT_GAUCHE, 42, 4);
        for (t = 1; t < 20; t++) {
            a.turn = b.turn = (u8)t;
            if (events_draw(&a) != events_draw(&b)) ok = 0;
        }
        CHECK(ok);
        CHECK_EQ(a.rng.state, b.rng.state);
        CHECK_EQ(a.events_done, b.events_done);
    }
}

// --- Partie automatique avec événements ------------------------------------

static const u8 pref[ORIENT_COUNT][LOI_COUNT] = {
    { LOI_SMIC, LOI_RENOV, LOI_CIR, LOI_POLICE, LOI_IS },
    { LOI_RENOV, LOI_CIR, LOI_POLICE, LOI_IS, LOI_SMIC },
    { LOI_IS, LOI_POLICE, LOI_CIR, LOI_RENOV, LOI_SMIC },
};

static void auto_laws(Country *c)
{
    int i;
    VoteResult v;
    for (i = 0; i < LOI_COUNT; i++) {
        u8 id = pref[c->orientation][i];
        if (law_can_propose(c, id) != LAW_OK) continue;
        parl_vote(c, law_get(id)->orientation, &v);
        if (v.adopted) sim_propose_law(c, id, &v);
    }
}

static int play(u8 orient, u32 seed, int with_events, Country *c, ElectionResult *r)
{
    Rng player;   // choix aléatoires du joueur, hasard séparé de la simulation
    int n_events = 0;
    sim_init(c, orient, seed, 4);
    rng_seed(&player, seed ^ 0x5bd1e995u);
    while (!sim_mandate_over(c)) {
        if (with_events) {
            s16 id = events_draw(c);
            if (id >= 0) {
                const EventDef *e = event_get((u8)id);
                events_choose(c, (u8)id, (u8)rng_below(&player, e->n_choices));
                n_events++;
            }
        }
        auto_laws(c);
        sim_end_turn(c, 0);
        CHECK(in_bounds(c));
    }
    sim_election(c, r);
    return n_events;
}

#define SEEDS 400

static void test_autoplay_events(void)
{
    static const char *names[ORIENT_COUNT] = { "gauche", "centre", "droite" };
    int o;
    printf("  Partie automatique + événements, choix au hasard (%d graines) :\n", SEEDS);
    printf("  %-7s %9s %9s %8s %8s %6s\n", "orient", "pop% sans", "pop% avec",
           "élu% sans", "élu% avec", "évts");
    for (o = 0; o < ORIENT_COUNT; o++) {
        long pop[2] = { 0, 0 }, el[2] = { 0, 0 }, ev = 0;
        u32 seed;
        int w;
        for (w = 0; w < 2; w++)
            for (seed = 1; seed <= SEEDS; seed++) {
                Country c;
                ElectionResult r;
                int n = play((u8)o, seed * 2654435761u, w, &c, &r);
                pop[w] += sim_popularity(&c);
                el[w] += r.elected;
                if (w) ev += n;
            }
        printf("  %-7s %9.1f %9.1f %8.0f %8.0f %6.1f\n", names[o],
               pop[0] / 10.0 / SEEDS, pop[1] / 10.0 / SEEDS,
               100.0 * el[0] / SEEDS, 100.0 * el[1] / SEEDS, (double)ev / SEEDS);
        CHECK(el[1] > SEEDS / 20 && el[1] < SEEDS * 95 / 100);
    }
}

void test_events(void)
{
    test_catalog();
    test_conditions();
    test_choose();
    test_draw();
    test_autoplay_events();
}
