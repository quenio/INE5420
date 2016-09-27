#include "min_unit.h"

static const char * test()
{
    mu_assert(1 == 1);

    return nullptr;
}

void all_tests()
{
    mu_test(test);
}
