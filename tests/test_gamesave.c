#include <string.h>
#include "test.h"
#include "save/gamesave.h"

#define N 500

void test_gamesave(void)
{
    u8 src[N], dst[N], blob[GAMESAVE_HEADER + N], bad[GAMESAVE_HEADER + N];
    u16 i, len;

    printf("  gamesave\n");
    for (i = 0; i < N; i++) src[i] = (u8)(i * 7 + 3);
    len = gamesave_pack(src, N, blob);
    CHECK_EQ(len, GAMESAVE_HEADER + N);
    memset(dst, 0, N);
    CHECK_EQ(gamesave_unpack(dst, N, blob, len), 1);
    CHECK(memcmp(src, dst, N) == 0);
    CHECK_EQ(gamesave_unpack(0, N, blob, len), 1);

    // corruption d'un octet de données, puis d'en-tête
    memcpy(bad, blob, len);
    bad[GAMESAVE_HEADER + 123] ^= 0x01;
    memset(dst, 0xAA, N);
    CHECK_EQ(gamesave_unpack(dst, N, bad, len), 0);
    CHECK_EQ(dst[0], 0xAA);  // dst intact
    memcpy(bad, blob, len);
    bad[8] ^= 0x10;
    CHECK_EQ(gamesave_unpack(dst, N, bad, len), 0);

    // taille différente (jeu mis à jour)
    CHECK_EQ(gamesave_unpack(dst, N + 2, blob, len), 0);
    CHECK_EQ(gamesave_unpack(dst, N - 2, blob, len), 0);
    // longueur lue trop courte
    CHECK_EQ(gamesave_unpack(dst, N, blob, len - 1), 0);

    // signature absente / version inconnue
    memcpy(bad, blob, len);
    bad[0] = 'X';
    CHECK_EQ(gamesave_unpack(dst, N, bad, len), 0);
    memcpy(bad, blob, len);
    bad[4] = 99;
    CHECK_EQ(gamesave_unpack(dst, N, bad, len), 0);

    // SRAM vierge
    memset(bad, 0x00, sizeof bad);
    CHECK_EQ(gamesave_unpack(dst, N, bad, len), 0);
    memset(bad, 0xFF, sizeof bad);
    CHECK_EQ(gamesave_unpack(dst, N, bad, len), 0);
}
