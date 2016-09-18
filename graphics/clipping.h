#pragma once

#include <bitset>
#include <map>
#include <utility>
#include "coord.h"

using namespace std;

enum  ClippingRegion { NORTH, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST, NONE };

static map<string, ClippingRegion> clipping_region_mapping
{
    { "1000", ClippingRegion::NORTH },
    { "1010", ClippingRegion::NORTHEAST },
    { "0010", ClippingRegion::EAST },
    { "0110", ClippingRegion::SOUTHEAST },
    { "0100", ClippingRegion::SOUTH },
    { "0101", ClippingRegion::SOUTHWEST },
    { "0001", ClippingRegion::WEST },
    { "1001", ClippingRegion::NORTHWEST },
    { "0000", ClippingRegion::NONE },
};

typedef bitset<4> ClippingRegionCode;

// Determine the clipping region based on the code.
inline ClippingRegion region(ClippingRegionCode code)
{
    for (auto &item: clipping_region_mapping)
        if (code == ClippingRegionCode { item.first })
            return item.second;

    return ClippingRegion::NONE;
}

// Determine the clipping region code based on the Window coord.
inline ClippingRegionCode region_code(Coord &coord)
{
    const double x = coord.x(), y = coord.y();

    ClippingRegionCode code { "0000" };

    if (x < -1) code.set(4); else if (x > +1) code.set(3);
    if (y < -1) code.set(2); else if (y > +1) code.set(1);

    return code;
}

// Determine the clipping region based on Window coord.
inline ClippingRegion region(Coord &coord)
{
    return region(region_code(coord));
}

// Determine which one between a nd b is in bounds.
inline Coord choose_in_bounds(Coord a, Coord b)
{
    if (region(a) == ClippingRegion::NONE) return a; else return b;
}

// Clip point a between Window coord a and b using Cohen-Sutherland
inline Coord clip_point_in_line_using_cs(Coord &a, Coord &b, double m)
{
    switch (region(a))
    {
        case ClippingRegion::NORTH: return at_y(+1, b, m);
        case ClippingRegion::EAST: return at_x(+1, b, m);
        case ClippingRegion::SOUTH: return at_y(-1, b, m);
        case ClippingRegion::WEST: return at_x(-1, b, m);

        case ClippingRegion::NORTHEAST: return choose_in_bounds(at_y(+1, b, m), at_x(+1, b, m));
        case ClippingRegion::SOUTHEAST: return choose_in_bounds(at_y(-1, b, m), at_x(+1, b, m));
        case ClippingRegion::SOUTHWEST: return choose_in_bounds(at_y(-1, b, m), at_x(-1, b, m));
        case ClippingRegion::NORTHWEST: return choose_in_bounds(at_y(+1, b, m), at_x(-1, b, m));

        case ClippingRegion::NONE: return a;
    }

}

// Clip line between Window coord a and b using Cohen-Sutherland
inline pair<Coord, Coord> clip_line_using_cs(Coord a, Coord b)
{
    double m = angular_coefficient(a, b);

    return make_pair(
        clip_point_in_line_using_cs(a, b, m),
        clip_point_in_line_using_cs(b, a, m)
    );
};

