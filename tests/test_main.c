#include "test.h"

int test_failures = 0;
int test_checks = 0;

void test_rng(void);
void test_sim(void);
void test_events(void);

int main(void)
{
    printf("Tests du cœur de simulation\n");
    test_rng();
    test_sim();
    test_events();
    printf("%d vérification(s), %d échec(s)\n", test_checks, test_failures);
    return test_failures ? 1 : 0;
}
