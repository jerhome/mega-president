#ifndef SIM_RNG_H
#define SIM_RNG_H

#include "sim/sim_types.h"

// Générateur pseudo-aléatoire reproductible (xorshift32).
// Même graine => même partie, sur console comme sur PC.

typedef struct { u32 state; } Rng;

void rng_seed(Rng *r, u32 seed);
u32  rng_next(Rng *r);
// Entier uniforme dans [0, n) ; n > 0.
u16  rng_below(Rng *r, u16 n);
// Entier uniforme dans [lo, hi] ; lo <= hi.
s16  rng_range(Rng *r, s16 lo, s16 hi);

#endif
