#ifndef TEST_H
#define TEST_H

// Mini framework de tests sur PC (aucune dépendance).
#include <stdio.h>
#include "budget_compat.h"

extern int test_failures;
extern int test_checks;

#define CHECK(cond) do { \
    test_checks++; \
    if (!(cond)) { test_failures++; \
        printf("  ÉCHEC %s:%d : %s\n", __FILE__, __LINE__, #cond); } \
} while (0)

#define CHECK_EQ(a, b) do { \
    long long _a = (long long)(a), _b = (long long)(b); \
    test_checks++; \
    if (_a != _b) { test_failures++; \
        printf("  ÉCHEC %s:%d : %s == %s (%lld != %lld)\n", \
               __FILE__, __LINE__, #a, #b, _a, _b); } \
} while (0)

// Fixe l'opinion des électorats (nombre d'électorats quelconque).
#define SET_OPN(c, v) do { int _e; for (_e = 0; _e < ELEC_COUNT; _e++) (c).opinion[_e] = (s16)(v); } while (0)

#endif
