#include "sim/rng.h"

void rng_seed(Rng *r, u32 seed)
{
    // xorshift ne doit jamais avoir un état nul.
    r->state = seed ? seed : 0x2545F491u;
}

u32 rng_next(Rng *r)
{
    u32 x = r->state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    r->state = x;
    return x;
}

u16 rng_below(Rng *r, u16 n)
{
    // Multiplication 16x16 -> 32 (instruction native du 68000), sans division.
    u16 hi = (u16)(rng_next(r) >> 16);
    return (u16)(((u32)hi * n) >> 16);
}

s16 rng_range(Rng *r, s16 lo, s16 hi)
{
    return (s16)(lo + (s16)rng_below(r, (u16)(hi - lo + 1)));
}
