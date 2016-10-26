// Line clipping based on the Cohen-Sutherland algorithm

#pragma once

#include "region.h"

#include <utility>

using namespace std;

// Clip point a between Window coord a and b using Cohen-Sutherland
template<class Coord>
inline Coord clip_point_in_line_using_cs(const Coord &a, const Coord &b, double m)
{
    static_assert(is_convertible<TVector, Coord>::value, "Coord must have constructor: Coord(const TVector &)");
    static_assert(is_convertible<Coord, TVector>::value, "Coord must have conversion operator: operator TVector() const");
    static_assert(is_base_of<XYCoord<Coord>, Coord>::value, "Coord must derive from XYCoord<Coord>");

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
template<class Coord>
inline pair<Coord, Coord> clip_line_using_cs(const Coord &a, const Coord &b)
{
    double m = angular_coefficient(a, b, 1, 0);

    return make_pair(
        clip_point_in_line_using_cs(a, b, m),
        clip_point_in_line_using_cs(b, a, m)
    );
}

