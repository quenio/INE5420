#include "min_unit.h"
#include "../graphics2d.h"

static const char * test_translation()
{
    mu_assert(Coord2D(1, -2) * translation(Coord2D(2, 3)) == Coord2D(3, 1));

    return nullptr;
}

void all_tests()
{
    mu_test(test_translation);
}
