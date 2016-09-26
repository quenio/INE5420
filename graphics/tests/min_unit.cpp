// Copyright (c) 2016 Quenio Cesar Machado dos Santos. All rights reserved.
// Original Code: http://www.jera.com/techinfo/jtns/jtn002.html#License

#include "min_unit.h"

#define UNUSED __attribute__ ((unused))

int tests_run = 0;
int tests_failed = 0;

int main(int UNUSED argc, char UNUSED **argv)
{
    printf("\n");

    all_tests();

    if (tests_failed == 0)
    {
        printf("\nTests Run: %d\n", tests_run);
        printf("ALL TESTS PASSED\n");
    }
    else
    {
        printf("\nTests Run: %d - Tests Failed: %d\n", tests_run, tests_failed);
    }

    return tests_failed;
}
