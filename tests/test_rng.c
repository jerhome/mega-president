#include "test.h"
#include "sim/rng.h"

void test_rng(void)
{
    Rng a, b;
    int i;

    // Reproductibilité : même graine, même suite.
    rng_seed(&a, 1234);
    rng_seed(&b, 1234);
    for (i = 0; i < 100; i++)
        CHECK_EQ(rng_next(&a), rng_next(&b));

    // Graine nulle acceptée (état jamais nul).
    rng_seed(&a, 0);
    CHECK(rng_next(&a) != 0);

    // Bornes et répartition grossière de rng_below.
    {
        int counts[6] = {0};
        rng_seed(&a, 42);
        for (i = 0; i < 6000; i++) {
            u16 v = rng_below(&a, 6);
            CHECK(v < 6);
            if (v < 6) counts[v]++;
        }
        for (i = 0; i < 6; i++)
            CHECK(counts[i] > 800 && counts[i] < 1200);
    }

    // rng_range inclusif, y compris négatif.
    {
        int seen_lo = 0, seen_hi = 0;
        rng_seed(&a, 7);
        for (i = 0; i < 2000; i++) {
            s16 v = rng_range(&a, -3, 3);
            CHECK(v >= -3 && v <= 3);
            if (v == -3) seen_lo = 1;
            if (v == 3) seen_hi = 1;
        }
        CHECK(seen_lo && seen_hi);
    }
}
