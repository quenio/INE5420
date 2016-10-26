#pragma once

#include "transforms.h"

#include <bitset>
#include <map>

using namespace std;

// Regions according to the World Window
enum class Region { NORTH, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST, CENTRAL };

// Bitset to region mapping
static map<string, Region> region_mapping
    {
        { "1000", Region::NORTH },
        { "1010", Region::NORTHEAST },
        { "0010", Region::EAST },
        { "0110", Region::SOUTHEAST },
        { "0100", Region::SOUTH },
        { "0101", Region::SOUTHWEST },
        { "0001", Region::WEST },
        { "1001", Region::NORTHWEST },
        { "0000", Region::CENTRAL },
    };

constexpr int REGION_CODE_SIZE = 4;
typedef bitset<REGION_CODE_SIZE> RegionCode;

// Position of the super region in the region bit set.
enum SuperRegionIndex: size_t {
    NORTH = 1, SOUTH = 2, EAST = 3, WEST = 4
};

// Determine actual super region index based on the size of the bitset.
inline size_t bitset_index(size_t index)
{
    return REGION_CODE_SIZE - index; // bitset has indexes in reserve order - LSB order.
}

// Determine the region code based on the Window vector.
inline RegionCode region_code(const TVector &v)
{
    const double x = v[0], y = v[1];

    RegionCode code { "0000" };

    if (x < -1) code.set(bitset_index(WEST)); else if (x > +1) code.set(bitset_index(EAST));
    if (y < -1) code.set(bitset_index(SOUTH)); else if (y > +1) code.set(bitset_index(NORTH));

    return code;
}

// Determine the region based on the code.
inline Region region(RegionCode code)
{
    for (auto &item: region_mapping)
        if (code == RegionCode { item.first })
            return item.second;

    return Region::CENTRAL;
}

// Determine the region based on Window vector.
inline Region region(const TVector &v)
{
    return region(region_code(v));
}

// Determine if vector is located in super region i.
inline bool in_super_region(SuperRegionIndex i, const TVector &v)
{
    return region_code(v).test(bitset_index(i));
}

// Determine if line between a and b is fully located in super region i.
inline bool in_super_region(SuperRegionIndex i, const TVector &a, const TVector &b)
{
    return in_super_region(i, a) && in_super_region(i, b);
}

// Determine if line between a and b is fully located in one super region.
inline bool in_one_super_region(const TVector &a, const TVector &b)
{
    return in_super_region(NORTH, a, b) || in_super_region(SOUTH, a, b) ||
           in_super_region(EAST, a, b)  || in_super_region(WEST, a, b);
}

// Determine which one between a nd b is in bounds.
inline TVector choose_in_bounds(const TVector &a, const TVector &b)
{
    if (region(a) == Region::CENTRAL) return a; else return b;
}
