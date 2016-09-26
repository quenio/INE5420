#include "min_unit.h"

static const char * test()
{
    mu_assert("one" == "one");

    return nullptr;
}

void all_tests()
{
    mu_test(test);
}
