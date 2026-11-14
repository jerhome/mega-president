#ifndef SIM_TYPES_H
#define SIM_TYPES_H

// Types entiers du cœur de simulation.
// Le dossier src/sim/ ne dépend PAS de SGDK : il compile aussi avec gcc sur PC
// (make test). Interdits : float, double, malloc, stdio.

#ifdef SGDK_GCC
#include <types.h>
#else
#include <stdint.h>
typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;
#endif

#endif
