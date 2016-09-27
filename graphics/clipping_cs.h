// Line clipping based on the Cohen-Sutherland algorithm

#pragma once

#include "region.h"

#include <utility>

using namespace std;

// Determine which one between a nd b is in bounds.
inline Coord choose_in_bounds(const Coord &a, const Coord &b)
{
    if (region(a) == Region::CENTRAL) return a; else return b;
}

// Clip point a between Window coord a and b using Cohen-Sutherland
inline Coord clip_point_in_line_using_cs(const Coord &a, const Coord &b, double m)
{
    switch (region(a))
    {
        case Region::NORTH: return at_y(+1, b, m);
        case Region::EAST: return at_x(+1, b, m);
        case Region::SOUTH: return at_y(-1, b, m);
        case Region::WEST: return at_x(-1, b, m);

        case Region::NORTHEAST: return choose_in_bounds(at_y(+1, b, m), at_x(+1, b, m));
        case Region::SOUTHEAST: return choose_in_bounds(at_y(-1, b, m), at_x(+1, b, m));
        case Region::SOUTHWEST: return choose_in_bounds(at_y(-1, b, m), at_x(-1, b, m));
        case Region::NORTHWEST: return choose_in_bounds(at_y(+1, b, m), at_x(-1, b, m));

        case Region::CENTRAL: return a;
    }
}

// Clip line between Window coord a and b using Cohen-Sutherland.
inline pair<Coord, Coord> clip_line_using_cs(const Coord &a, const Coord &b)
{
    double m = angular_coefficient(a, b);

    return make_pair(
        clip_point_in_line_using_cs(a, b, m),
        clip_point_in_line_using_cs(b, a, m)
    );
}

