#include "test.h"
#include "sim/sim.h"
#include <stdio.h>
#include <string.h>

// V2 : popularité par région (2.01) et pilier international (2.08).

static void new_game(Country *c, u8 orient, u32 seed)
{
    SimSetup s;
    s.orientation = orient;
    s.pm = PM_NONE;
    s.difficulty = DIFF_NORMAL;
    s.turns_per_year = 4;
    s.seed = seed;
    sim_init_ex(c, &s);
}

// Moyenne des popularités régionales, pondérée par la population.
static s32 weighted_avg(const Country *c)
{
    s32 sum = 0, pop = 0;
    u8 r;
    for (r = 0; r < REG_COUNT; r++) {
        sum += (s32)sim_region_popularity(c, r) * region_profiles[r].pop_share;
        pop += region_profiles[r].pop_share;
    }
    return sum / pop;
}

// Les REG_* doivent avoir le même ordre que REGION_* de src/ui/ui_map.h.
static void test_enum_matches_ui(void)
{
    static const char *const names[REG_COUNT] = {
        "HAUTS_DE_FRANCE", "NORMANDIE", "ILE_DE_FRANCE", "GRAND_EST", "BRETAGNE",
        "PAYS_DE_LA_LOIRE", "CENTRE_VAL_DE_LOIRE", "BOURGOGNE_FRANCHE_COMTE",
        "NOUVELLE_AQUITAINE", "AUVERGNE_RHONE_ALPES", "OCCITANIE", "PACA", "CORSE"
    };
    FILE *f = fopen("src/ui/ui_map.h", "r");
    char line[200];
    int next = 0;
    CHECK_EQ(REG_HAUTS_DE_FRANCE, 0);
    CHECK_EQ(REG_ILE_DE_FRANCE, 2);
    CHECK_EQ(REG_CORSE, 12);
    CHECK_EQ(REG_END, REG_COUNT);
    if (!f) { printf("  (ui_map.h introuvable : ordre non vérifié)\n"); return; }
    while (fgets(line, sizeof line, f) && next < REG_COUNT) {
        char want[64];
        snprintf(want, sizeof want, "REGION_%s", names[next]);
        if (strstr(line, want) && strstr(line, "REGION_COUNT") == 0) {
            char *p = strstr(line, want);
            if (p[strlen(want)] == ',' || p[strlen(want)] == ' ' || p[strlen(want)] == '=') next++;
        }
    }
    fclose(f);
    CHECK_EQ(next, REG_COUNT);
}

static void test_profiles(void)
{
    u8 r, e;
    for (r = 0; r < REG_COUNT; r++) {
        const RegionProfile *p = &region_profiles[r];
        u8 sum = 0;
        for (e = 0; e < ELEC_COUNT; e++) sum = (u8)(sum + p->weight[e]);
        CHECK_EQ(sum, 100);
    }
    // Moyenne nationale des poids ≈ elec_weight (26 / 22 / 12 / 28 / 12), pondérée par la population.
    {
        s32 w[ELEC_COUNT] = { 0, 0, 0, 0, 0 }, pop = 0;
        for (r = 0; r < REG_COUNT; r++) {
            for (e = 0; e < ELEC_COUNT; e++) w[e] += (s32)region_profiles[r].weight[e] * region_profiles[r].pop_share;
            pop += region_profiles[r].pop_share;
        }
        printf("  poids moyens des électorats : %d / %d / %d / %d / %d %%\n",
               (int)(w[0] / pop), (int)(w[1] / pop), (int)(w[2] / pop), (int)(w[3] / pop), (int)(w[4] / pop));
        for (e = 0; e < ELEC_COUNT; e++) CHECK(sim_abs16((s16)(w[e] / pop - elec_weight[e])) <= 2);
    }
}

static void test_popularity(void)
{
    Country c;
    int o, t;
    u32 seed;
    for (o = 0; o < ORIENT_COUNT; o++)
        for (seed = 1; seed <= 20; seed++) {
            u8 r;
            new_game(&c, (u8)o, seed);
            CHECK(sim_abs16((s16)(weighted_avg(&c) - sim_popularity(&c))) <= 20);
            for (t = 0; t < 20; t++) {
                sim_end_turn(&c, 0);
                for (r = 0; r < REG_COUNT; r++) {
                    s16 p = sim_region_popularity(&c, r);
                    CHECK(p >= 0 && p <= 1000);
                    CHECK(sim_abs16((s16)(p - sim_popularity(&c))) <= 200);
                }
            }
            CHECK(sim_abs16((s16)(weighted_avg(&c) - sim_popularity(&c))) <= 25);
        }
    new_game(&c, ORIENT_CENTRE, 3);
    CHECK_EQ(sim_region_popularity(&c, 13), sim_popularity(&c));
    CHECK_EQ(sim_region_popularity(&c, 255), sim_popularity(&c));
}

static void test_sensitivity(void)
{
    Country a, b;
    s16 hdf0, idf0;
    new_game(&a, ORIENT_CENTRE, 5);
    b = a;
    // Chômage en hausse : le Nord (industriel) souffre plus que l'Île-de-France.
    hdf0 = sim_region_gap_target(&a, REG_HAUTS_DE_FRANCE);
    idf0 = sim_region_gap_target(&a, REG_ILE_DE_FRANCE);
    b.unemployment = (s16)(a.unemployment + 300);
    CHECK(sim_region_gap_target(&b, REG_HAUTS_DE_FRANCE) < hdf0 - 10);
    CHECK(sim_region_gap_target(&b, REG_HAUTS_DE_FRANCE) - hdf0
          < sim_region_gap_target(&b, REG_ILE_DE_FRANCE) - idf0);
    // Technologie : l'Île-de-France réagit plus que la Bretagne.
    b = a;
    b.pillars[PIL_TECHNOLOGIE] = 800;
    CHECK(sim_region_gap_target(&b, REG_ILE_DE_FRANCE) > sim_region_gap_target(&a, REG_ILE_DE_FRANCE) + 5);
    CHECK(sim_region_gap_target(&b, REG_BRETAGNE) <= sim_region_gap_target(&a, REG_BRETAGNE));
    // Écologie : la Bretagne réagit.
    b = a;
    b.pillars[PIL_ECOLOGIE] = 800;
    CHECK(sim_region_gap_target(&b, REG_BRETAGNE) > sim_region_gap_target(&a, REG_BRETAGNE) + 5);
    // Loi adoptée : l'agriculture productive profite à la Bretagne.
    b = a;
    law_mask_set(&b.laws_adopted, LOI_AGRI_PRODUCT);
    CHECK(sim_region_gap_target(&b, REG_BRETAGNE) >= sim_region_gap_target(&a, REG_BRETAGNE) + 10);
    // Inertie : l'écart évolue lentement vers la cible.
    b.unemployment = (s16)(a.unemployment + 300);
    {
        s8 g0 = b.region_gap[REG_HAUTS_DE_FRANCE];
        s16 target = sim_region_gap_target(&b, REG_HAUTS_DE_FRANCE);
        sim_region_update(&b);
        CHECK(b.region_gap[REG_HAUTS_DE_FRANCE] < g0);
        CHECK(b.region_gap[REG_HAUTS_DE_FRANCE] > target);
        {
            int i;
            for (i = 0; i < 80; i++) sim_region_update(&b);
            CHECK(sim_abs16((s16)(b.region_gap[REG_HAUTS_DE_FRANCE] - target)) <= 3);
        }
    }
}

static void test_international(void)
{
    Country c;
    s16 p0;
    new_game(&c, ORIENT_CENTRE, 9);
    CHECK(c.pillars[PIL_INTERNATIONAL] >= 480 && c.pillars[PIL_INTERNATIONAL] <= 520);
    CHECK_EQ(c.pillars_start[PIL_INTERNATIONAL], c.pillars[PIL_INTERNATIONAL]);
    CHECK_EQ(PIL_INTERNATIONAL, PIL_COUNT);
    CHECK_EQ(PIL_TOTAL, PIL_COUNT + 1);
    p0 = c.pillars[PIL_INTERNATIONAL];
    sim_apply_effect(&c, EFF_INTERNATIONAL, 5);
    CHECK_EQ(c.pillars[PIL_INTERNATIONAL], p0 + 50);
    sim_apply_effect(&c, EFF_INTERNATIONAL, -127);
    CHECK_EQ(c.pillars[PIL_INTERNATIONAL], 0);
    sim_apply_effect(&c, EFF_INTERNATIONAL, 127);
    CHECK_EQ(c.pillars[PIL_INTERNATIONAL], 1000);
    // Lois internationales
    {
        const LawDef *l = law_get(LOI_AIDE_DEV);
        u8 i, found = 0;
        for (i = 0; i < l->n_effects; i++) if (l->effects[i].key == EFF_INTERNATIONAL) found = 1;
        CHECK(found);
        l = law_get(LOI_EXPORT_ARMES);
        found = 0;
        for (i = 0; i < l->n_effects; i++) if (l->effects[i].key == EFF_INTERNATIONAL && l->effects[i].value < 0) found = 1;
        CHECK(found);
    }
    // Le pilier compte dans l'opinion (poids faible) et dans le score.
    {
        Country a, b;
        new_game(&a, ORIENT_CENTRE, 9);
        b = a;
        b.pillars[PIL_INTERNATIONAL] = 900;
        CHECK(sim_opinion_target(&b, ELEC_AISEES) > sim_opinion_target(&a, ELEC_AISEES));
        CHECK(sim_opinion_target(&b, ELEC_AISEES) - sim_opinion_target(&a, ELEC_AISEES) < 30);
        CHECK(sim_mandate_score(&b, 0) > sim_mandate_score(&a, 0));
        CHECK(sim_pillar_progress(&b) > sim_pillar_progress(&a));
    }
}

void test_regions(void)
{
    test_enum_matches_ui();
    test_profiles();
    test_popularity();
    test_sensitivity();
    test_international();
}
