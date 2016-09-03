// Copyright (c) 2016 Quenio Cesar Machado dos Santos. All rights reserved.

#include "min_unit.h"

const char * test()
{
    mu_assert("one" == "one");

    return nullptr;
}

void all_tests()
{
    mu_test(test);
}
