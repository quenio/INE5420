#include "min_unit.h"
#include "../clipping_cs.h"

const char * test_region()
{
    Coord north(0, +1.5);
    Coord northeast(+1.5, +1.5);
    Coord east(+1.5, 0);
    Coord southeast(+1.5, -1.5);
    Coord south(0, -1.5);
    Coord southwest(-1.5, -1.5);
    Coord west(-1.5, 0);
    Coord northwest(-1.5, +1.5);

    printf("%s\n", region_code(southeast).to_string().c_str());

    mu_assert(region(north) == ClippingRegion::NORTH);
    mu_assert(region(northeast) == ClippingRegion::NORTHEAST);
    mu_assert(region(east) == ClippingRegion::EAST);
    mu_assert(region(southeast) == ClippingRegion::SOUTHEAST);
    mu_assert(region(south) == ClippingRegion::SOUTH);
    mu_assert(region(southwest) == ClippingRegion::SOUTHWEST);
    mu_assert(region(west) == ClippingRegion::WEST);
    mu_assert(region(northwest) == ClippingRegion::NORTHWEST);

    return nullptr;
}

void all_tests()
{
    mu_test(test_region);
}
