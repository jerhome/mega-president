#include <string.h>
#include "test.h"
#include "save/scores.h"

static void fill(ScoreTable *t, u16 base)
{
    u8 i;
    score_table_reset(t);
    for (i = 0; i < SCORES_COUNT; i++)
        score_table_insert(t, "AAA", (u16)(base + i * 100), 1, 2030);
}

void test_save(void)
{
    ScoreBoard b, b2;
    ScoreTable t;
    u8 buf[SCORES_BLOB_SIZE], bad[SCORES_BLOB_SIZE];
    u8 i;

    printf("  save : classements\n");
    // table vide : tout score > 0 entre, 0 jamais
    score_table_reset(&t);
    CHECK_EQ(score_table_rank(&t, 0), -1);
    CHECK_EQ(score_table_rank(&t, 1), 0);
    CHECK_EQ(score_table_insert(&t, "BOB", 500, 2, 2031), 0);
    CHECK_EQ(score_table_insert(&t, "ZED", 700, 0, 2032), 0);
    CHECK_EQ(score_table_insert(&t, "MIM", 600, 1, 2033), 1);
    CHECK_EQ(t.e[0].score, 700);
    CHECK_EQ(t.e[1].ini[0], 'M');
    CHECK_EQ(t.e[2].score, 500);
    CHECK_EQ(t.e[3].score, 0);
    // égalité : le plus ancien reste devant
    CHECK_EQ(score_table_insert(&t, "NEW", 600, 1, 2034), 2);
    CHECK_EQ(t.e[1].ini[0], 'M');
    CHECK_EQ(t.e[2].ini[0], 'N');
    // initiales invalides normalisées
    score_table_insert(&t, "a?z", 900, 1, 2035);
    CHECK_EQ(t.e[0].ini[0], 'A');
    CHECK_EQ(t.e[0].ini[1], 'A');
    CHECK_EQ(t.e[0].ini[2], 'Z');

    // table pleine : le dernier sort, un score trop faible n'entre pas
    fill(&t, 100);   // 100..1000
    CHECK_EQ(t.e[0].score, 1000);
    CHECK_EQ(t.e[9].score, 100);
    CHECK_EQ(score_table_rank(&t, 100), -1);
    CHECK(!score_is_high(&t, 50));
    CHECK(score_is_high(&t, 101));
    CHECK_EQ(score_table_insert(&t, "CCC", 550, 1, 2040), 5);
    CHECK_EQ(t.e[9].score, 200);
    CHECK_EQ(score_table_insert(&t, "DDD", 150, 1, 2040), -1);
    CHECK_EQ(score_table_insert(&t, "EEE", 5000, 1, 2040), 0);
    for (i = 1; i < SCORES_COUNT; i++)
        CHECK(t.e[i - 1].score >= t.e[i].score);

    printf("  save : sérialisation\n");
    scores_reset(&b);
    fill(&b.t[0], 100);
    fill(&b.t[1], 2000);
    b.t[1].e[3].year = 2036;
    CHECK_EQ(scores_serialize(&b, buf), SCORES_BLOB_SIZE);
    CHECK(memcmp(buf, "MPV1", 4) == 0);
    CHECK_EQ(buf[4], SCORES_VERSION);
    CHECK_EQ(scores_deserialize(&b2, buf, SCORES_BLOB_SIZE), 0);
    for (i = 0; i < SCORES_COUNT; i++) {
        CHECK_EQ(b.t[0].e[i].score, b2.t[0].e[i].score);
        CHECK(memcmp(b.t[0].e[i].ini, b2.t[0].e[i].ini, 3) == 0);
        CHECK_EQ(b.t[0].e[i].orient, b2.t[0].e[i].orient);
        CHECK_EQ(b.t[0].e[i].year, b2.t[0].e[i].year);
    }
    CHECK_EQ(b2.t[1].e[3].year, 2036);
    CHECK_EQ(b2.t[1].e[0].score, 2900);
    CHECK_EQ(b2.t[0].e[9].score, 100);

    // SRAM vierge (0x00 ou 0xFF) : réinitialisée sans erreur
    memset(bad, 0, sizeof bad);
    CHECK_EQ(scores_deserialize(&b2, bad, SCORES_BLOB_SIZE), SCORES_TABLES);
    CHECK_EQ(b2.t[0].e[0].score, 0);
    memset(bad, 0xFF, sizeof bad);
    CHECK_EQ(scores_deserialize(&b2, bad, SCORES_BLOB_SIZE), SCORES_TABLES);
    CHECK_EQ(scores_deserialize(&b2, NULL, 0), SCORES_TABLES);
    CHECK_EQ(scores_deserialize(&b2, buf, SCORES_BLOB_SIZE - 1), SCORES_TABLES);

    // signature fausse
    memcpy(bad, buf, sizeof bad);
    bad[3] = '2';
    CHECK_EQ(scores_deserialize(&b2, bad, SCORES_BLOB_SIZE), SCORES_TABLES);
    CHECK_EQ(b2.t[1].e[0].score, 0);
    // version fausse
    memcpy(bad, buf, sizeof bad);
    bad[4] = 9;
    CHECK_EQ(scores_deserialize(&b2, bad, SCORES_BLOB_SIZE), SCORES_TABLES);

    // octet corrompu dans le tableau 1 : seul le tableau 1 est réinitialisé
    memcpy(bad, buf, sizeof bad);
    bad[5 + SCORES_TABLE_SIZE + 10] ^= 0x40;
    CHECK_EQ(scores_deserialize(&b2, bad, SCORES_BLOB_SIZE), 1);
    CHECK_EQ(b2.t[0].e[0].score, 1000);
    CHECK_EQ(b2.t[1].e[0].score, 0);
    // somme corrompue dans le tableau 0
    memcpy(bad, buf, sizeof bad);
    bad[5 + SCORES_TABLE_SIZE - 1] ^= 1;
    CHECK_EQ(scores_deserialize(&b2, bad, SCORES_BLOB_SIZE), 1);
    CHECK_EQ(b2.t[0].e[0].score, 0);
    CHECK_EQ(b2.t[1].e[0].score, 2900);

    // initiales invalides avec somme recalculée correcte : tableau rejeté
    // (on réécrit une lettre puis on reserialise via la structure)
    b2 = b;
    b2.t[0].e[2].ini[1] = '!';
    scores_serialize(&b2, bad);
    CHECK_EQ(scores_deserialize(&b2, bad, SCORES_BLOB_SIZE), 1);
    CHECK_EQ(b2.t[0].e[0].score, 0);
    CHECK_EQ(b2.t[1].e[0].score, 2900);

    // balayage : n'importe quel octet modifié ne plante pas et est détecté
    for (i = 0; i < 40; i++) {
        u16 pos = (u16)((i * 37) % SCORES_BLOB_SIZE);
        memcpy(bad, buf, sizeof bad);
        bad[pos] ^= 0x01;
        CHECK(scores_deserialize(&b2, bad, SCORES_BLOB_SIZE) >= 1);
    }
}
