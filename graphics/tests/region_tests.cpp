#include "min_unit.h"
#include "../region.h"

static const char * test_region()
{
    Coord north(0, +1.5);
    Coord northeast(+1.5, +1.5);
    Coord east(+1.5, 0);
    Coord southeast(+1.5, -1.5);
    Coord south(0, -1.5);
    Coord southwest(-1.5, -1.5);
    Coord west(-1.5, 0);
    Coord northwest(-1.5, +1.5);

    mu_assert(region(north) == Region::NORTH);
    mu_assert(region(northeast) == Region::NORTHEAST);
    mu_assert(region(east) == Region::EAST);
    mu_assert(region(southeast) == Region::SOUTHEAST);
    mu_assert(region(south) == Region::SOUTH);
    mu_assert(region(southwest) == Region::SOUTHWEST);
    mu_assert(region(west) == Region::WEST);
    mu_assert(region(northwest) == Region::NORTHWEST);

    return nullptr;
}

void all_tests()
{
    mu_test(test_region);
}
