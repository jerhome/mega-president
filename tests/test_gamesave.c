#include <string.h>
#include "test.h"
#include "save/gamesave.h"
#include "game/game_state.h"

#define N 500
#define FP 0xBEEF

static void test_blob(void)
{
    u8 src[N], dst[N], blob[GAMESAVE_HEADER + N], bad[GAMESAVE_HEADER + N];
    u16 i, len;

    for (i = 0; i < N; i++) src[i] = (u8)(i * 7 + 3);
    len = gamesave_pack(src, N, FP, 7, blob);
    CHECK_EQ(len, GAMESAVE_HEADER + N);
    CHECK_EQ(gamesave_gen(blob), 7);
    memset(dst, 0, N);
    CHECK_EQ(gamesave_unpack(dst, N, FP, blob, len), 1);
    CHECK(memcmp(src, dst, N) == 0);
    CHECK_EQ(gamesave_unpack(0, N, FP, blob, len), 1);

    // somme : un octet de données, puis octets utiles de l'en-tête
    memcpy(bad, blob, len);
    bad[GAMESAVE_HEADER + 123] ^= 0x01;
    memset(dst, 0xAA, N);
    CHECK_EQ(gamesave_unpack(dst, N, FP, bad, len), 0);
    CHECK_EQ(dst[0], 0xAA);  // dst intact
    memcpy(bad, blob, len);
    bad[8] ^= 0x10;
    CHECK_EQ(gamesave_unpack(dst, N, FP, bad, len), 0);
    memcpy(bad, blob, len);
    bad[13] ^= 0x01;         // génération couverte par la somme
    CHECK_EQ(gamesave_unpack(dst, N, FP, bad, len), 0);

    // empreinte différente (contenu ajouté) : refus et « ancienne version »
    CHECK_EQ(gamesave_unpack(dst, N, FP + 1, blob, len), 0);
    CHECK_EQ(gamesave_is_stale(blob, len, N, FP + 1), 1);
    CHECK_EQ(gamesave_is_stale(blob, len, N, FP), 0);

    // taille différente, longueur courte, signature, version
    CHECK_EQ(gamesave_unpack(dst, N + 2, FP, blob, len), 0);
    CHECK_EQ(gamesave_unpack(dst, N - 2, FP, blob, len), 0);
    CHECK_EQ(gamesave_unpack(dst, N, FP, blob, len - 1), 0);
    memcpy(bad, blob, len);
    bad[0] = 'X';
    CHECK_EQ(gamesave_unpack(dst, N, FP, bad, len), 0);
    memcpy(bad, blob, len);
    bad[4] = 2;              // sauvegarde v2
    CHECK_EQ(gamesave_unpack(dst, N, FP, bad, len), 0);
    CHECK_EQ(gamesave_is_stale(bad, len, N, FP), 1);

    // SRAM vierge : pas « ancienne »
    memset(bad, 0x00, sizeof bad);
    CHECK_EQ(gamesave_unpack(dst, N, FP, bad, len), 0);
    CHECK_EQ(gamesave_is_stale(bad, len, N, FP), 0);
    memset(bad, 0xFF, sizeof bad);
    CHECK_EQ(gamesave_unpack(dst, N, FP, bad, len), 0);
    CHECK_EQ(gamesave_is_stale(bad, len, N, FP), 0);
}

static void test_fletcher(void)
{
    static u8 big[5000];
    u16 i, a = 0, b = 0;

    // vecteurs classiques de Fletcher-16
    CHECK_EQ(gamesave_fletcher16((const u8 *)"abcde", 5), 0xC8F0);
    CHECK_EQ(gamesave_fletcher16((const u8 *)"abcdef", 6), 0x2057);
    // grand bloc, comparé à la définition (modulo à chaque octet)
    for (i = 0; i < 5000; i++) {
        big[i] = (u8)(i * 13 + 255);
        a = (u16)((a + big[i]) % 255);
        b = (u16)((b + a) % 255);
    }
    CHECK_EQ(gamesave_fletcher16(big, 5000), (b << 8) | a);
}

// Simule deux emplacements de SRAM.
static void test_slots(void)
{
    static u8 A[GAMESAVE_HEADER + N], B[GAMESAVE_HEADER + N], tmp[GAMESAVE_HEADER + N], W[GAMESAVE_HEADER + N];
    u8 d1[N], d2[N], dst[N];
    u16 len = GAMESAVE_HEADER + N, n, cut;

    memset(d1, 0x11, N); memset(d2, 0x22, N);
    memset(A, 0xFF, sizeof A); memset(B, 0xFF, sizeof B);
    CHECK_EQ(gamesave_pick(A, B, len, N, FP), -1);

    gamesave_pack(d1, N, FP, 1, A);
    CHECK_EQ(gamesave_pick(A, B, len, N, FP), 0);
    gamesave_pack(d2, N, FP, 2, B);          // sauvegarde 2 vers B, complète
    CHECK_EQ(gamesave_pick(A, B, len, N, FP), 1);

    // coupure pendant l'écriture de la génération 3 dans A, à divers points
    n = gamesave_pack(d1, N, FP, 3, tmp);
    for (cut = 0; cut < n; cut += 37) {
        memcpy(W, A, n);
        W[0] = 0;                            // invalidation préalable
        if (cut > 1) memcpy(W + 1, tmp + 1, (size_t)cut - 1);
        CHECK_EQ(gamesave_pick(W, B, len, N, FP), 1);   // B (génération 2) intact
        memset(dst, 0, N);
        CHECK_EQ(gamesave_unpack(dst, N, FP, B, len), 1);
        CHECK_EQ(dst[10], 0x22);
    }
    // coupure sans invalidation préalable (blocs mêlés) : somme refuse A
    memcpy(W, A, n);
    memcpy(W + 20, tmp + 20, 100);
    CHECK_EQ(gamesave_pick(W, B, len, N, FP), 1);
    // écriture complète : A devient le plus récent
    memcpy(A, tmp, n);
    CHECK_EQ(gamesave_pick(A, B, len, N, FP), 0);
    // débordement du compteur : 65535 puis 0
    gamesave_pack(d1, N, FP, 65535, A);
    gamesave_pack(d2, N, FP, 0, B);
    CHECK_EQ(gamesave_pick(A, B, len, N, FP), 1);
}

static void new_game(Game *g)
{
    SimSetup s;
    memset(g, 0, sizeof *g);
    s.orientation = ORIENT_CENTRE; s.pm = PM_RASSEMBLEUR; s.difficulty = DIFF_NORMAL;
    s.turns_per_year = 4; s.seed = 42;
    sim_init_ex(&g->country, &s);
    g->event = -1; g->event_choice = -1;
}

#define BAD(field, val) do { Game t = g; t.field = (val); CHECK_EQ(game_sanitize(&t), 0); } while (0)

static void test_sanitize(void)
{
    static Game g, back;
    static u8 blob[GAMESAVE_HEADER + sizeof(Game)];
    u16 len, fp = game_fingerprint();
    u8 *p;
    u32 x = 12345;
    u16 i, ok = 0, k;

    new_game(&g);
    CHECK_EQ(game_sanitize(&g), 1);

    // aller-retour complet
    g.country.turn = 5;
    len = gamesave_pack(&g, (u16)sizeof g, fp, 1, blob);
    memset(&back, 0, sizeof back);
    CHECK_EQ(gamesave_unpack(&back, (u16)sizeof back, fp, blob, len), 1);
    CHECK(memcmp(&g, &back, sizeof g) == 0);
    CHECK_EQ(game_sanitize(&back), 1);

    // empreinte stable et sensible
    CHECK_EQ(game_fingerprint(), fp);
    CHECK(gamesave_unpack(0, (u16)sizeof g, (u16)(fp ^ 1), blob, len) == 0);

    // valeurs hors bornes : chaque cas refusé
    BAD(country.turns_per_year, 0);
    BAD(country.turns_per_year, 6);
    BAD(country.orientation, 3);
    BAD(country.difficulty, DIFF_COUNT);
    BAD(country.pm, PM_COUNT + 2);
    BAD(country.n_pending, SIM_MAX_PENDING + 1);
    BAD(country.cohab_group, GRP_COUNT);
    BAD(country.bud_amend_line, BUD_LINES + 1);
    BAD(country.bud_amend_from, 3);
    BAD(country.budget_lvl[0], 9);
    BAD(country.gauges[0], 101);
    BAD(country.cp, CP_MAX + 1);
    BAD(country.mandate, 2);
    BAD(event, 5000);
    BAD(event, -2);
    BAD(event_choice, 3);
    { Game t = g; t.country.n_pending = 1; t.country.pending[0].law = LOI_COUNT; CHECK_EQ(game_sanitize(&t), 0); }
    { Game t = g; t.country.n_pending = 1; t.country.pending[0].law = 0; CHECK_EQ(game_sanitize(&t), 1); }

    // octets aléatoires : jamais de plantage, refus quasi systématique
    p = (u8 *)&back;
    for (k = 0; k < 200; k++) {
        for (i = 0; i < sizeof back; i++) { x = x * 1103515245u + 12345u; p[i] = (u8)(x >> 16); }
        ok = (u16)(ok + game_sanitize(&back));
    }
    CHECK(ok < 5);
}

void test_gamesave(void)
{
    printf("  gamesave\n");
    test_blob();
    test_fletcher();
    test_slots();
    test_sanitize();
}
