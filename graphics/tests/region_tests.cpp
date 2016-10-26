#include "min_unit.h"
#include "../region.h"
#include "../graphics2d.h"

static const char * test_region()
{
    Coord2D north(0, +1.5);
    Coord2D northeast(+1.5, +1.5);
    Coord2D east(+1.5, 0);
    Coord2D southeast(+1.5, -1.5);
    Coord2D south(0, -1.5);
    Coord2D southwest(-1.5, -1.5);
    Coord2D west(-1.5, 0);
    Coord2D northwest(-1.5, +1.5);

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
