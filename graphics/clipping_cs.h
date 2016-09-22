// Line clipping based on the Cohen-Sutherland algorithm

#pragma once

#include <bitset>
#include <map>
#include <utility>
#include "coord.h"

using namespace std;

enum class ClippingRegion { NORTH, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST, NONE };

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

constexpr int REGION_CODE_SIZE = 4;
typedef bitset<REGION_CODE_SIZE> ClippingRegionCode;

// Position of the super region in the region bit set.
enum SuperRegionIndex: size_t {
    NORTH = 1, SOUTH = 2, EAST = 3, WEST = 4
};

// Determine actual super region index based on the size of the bitset.
inline size_t bitset_index(size_t index)
{
    return REGION_CODE_SIZE - index; // bitset has indexes in reserve order - LSB order.
}

// Determine the clipping region code based on the Window coord.
inline ClippingRegionCode region_code(const Coord &coord)
{
    const double x = coord.x(), y = coord.y();

    ClippingRegionCode code { "0000" };

    if (x < -1) code.set(bitset_index(WEST)); else if (x > +1) code.set(bitset_index(EAST));
    if (y < -1) code.set(bitset_index(SOUTH)); else if (y > +1) code.set(bitset_index(NORTH));

    return code;
}

// Determine the clipping region based on the code.
inline ClippingRegion region(ClippingRegionCode code)
{
    for (auto &item: clipping_region_mapping)
        if (code == ClippingRegionCode { item.first })
            return item.second;

    return ClippingRegion::NONE;
}

// Determine the clipping region based on Window coord.
inline ClippingRegion region(const Coord &coord)
{
    return region(region_code(coord));
}

// Determine if coord is located in super region i.
inline bool in_super_region(SuperRegionIndex i, Coord coord)
{
    return region_code(coord).test(bitset_index(i));
}

// Determine if line between a and b is fully located in super region i.
inline bool in_super_region(SuperRegionIndex i, Coord a, Coord b)
{
    return in_super_region(i, a) && in_super_region(i, b);
}

// Determine if line between a and b is fully located in one super region.
inline bool in_one_super_region(Coord a, Coord b)
{
    return in_super_region(NORTH, a, b) || in_super_region(SOUTH, a, b) ||
           in_super_region(EAST, a, b)  || in_super_region(WEST, a, b);
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

// Clip line between Window coord a and b using Cohen-Sutherland.
inline pair<Coord, Coord> clip_line_using_cs(Coord a, Coord b)
{
    double m = angular_coefficient(a, b);

    return make_pair(
        clip_point_in_line_using_cs(a, b, m),
        clip_point_in_line_using_cs(b, a, m)
    );
}

