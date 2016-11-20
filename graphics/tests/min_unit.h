// Copyright (c) 2016 Quenio Cesar Machado dos Santos. All rights reserved.
// Original Code: http://www.jera.com/techinfo/jtns/jtn002.html#License

#include <stdio.h>
#include "../timer.h"

extern int tests_run;
extern int tests_failed;

#define mu_assert(test) do { if (!(test)) { return " - FAILED: " #test "\n"; } } while (0)
#define mu_test(test, ...) do { \
        const clock_t start = clock(); \
        const char *message = test(__VA_ARGS__); \
        const double time = elapsed_secs(start); \
        tests_run++; \
        printf("%s(%s)", #test, #__VA_ARGS__); \
        if (message) { tests_failed++; printf("%s", message); return; } \
        printf(" - PASSED (t = %9.6lf)\n", time); \
    } while (0)

void all_tests();


