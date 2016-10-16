#pragma once

#include "clipping_cs.h"
#include "clipping_lb.h"
#include "coord.h"

#include <memory>

using namespace std;

enum class Visibility { FULL, PARTIAL, NONE };

// Area in world that may be clipped
class ClippingArea
{
public:

    // True if area contains World coord.
    virtual bool contains(Coord coord) const = 0;

    // Translate coord from World to Window, where left-bottom is (-1, -1) and right-top is (1, 1).
    virtual Coord world_to_window(Coord coord) const = 0;

    // Translate coord from Window to World.
    virtual Coord window_to_world(Coord coord) const = 0;

};

// Clippable objects
template<class Object>
class Clippable
{
public:

    // Provide clipped version of itself in area.
    virtual shared_ptr<Object> clipped_in(ClippingArea &area) = 0;

};

enum class ClippingMethod { COHEN_SUTHERLAND, LIANG_BARSKY, NONE };

static ClippingMethod clipping_method = ClippingMethod::COHEN_SUTHERLAND;

// Clip line between World coord a and b.
inline pair<Coord, Coord> clip_line(const Coord &a, const Coord &b)
{
    switch (clipping_method)
    {
        case ClippingMethod::COHEN_SUTHERLAND: return clip_line_using_cs(a, b);
        case ClippingMethod::LIANG_BARSKY: return clip_line_using_lb(a, b);
        case ClippingMethod::NONE: return make_pair(a, b);
    }
}

// Clip line between World coord a and b into the area.
inline pair<Coord, Coord> clip_line(ClippingArea &area, const Coord &a, const Coord &b)
{
    const Coord window_a = area.world_to_window(a);
    const Coord window_b = area.world_to_window(b);

    const pair<Coord, Coord> clipped_line = clip_line(window_a, window_b);

    return make_pair(
        area.window_to_world(clipped_line.first),
        area.window_to_world(clipped_line.second)
    );
}

// Determine the visibility in area for line between a and b.
inline Visibility visibility(ClippingArea &area, const Coord &a, const Coord &b)
{
    static_assert(is_convertible<TVector, Coord>::value, "Coord must have constructor: Coord(const TVector &)");
    static_assert(is_convertible<Coord, TVector>::value, "Coord must have conversion operator: operator TVector() const");

    if (clipping_method == ClippingMethod::NONE) return Visibility::FULL;

    const bool a_in_area = area.contains(a);
    const bool b_in_area = area.contains(b);

    if (a_in_area && b_in_area)
    {
        return Visibility::FULL;
    }
    else if (a_in_area || b_in_area || area.contains(equidistant(a, b)))
    {
        return Visibility::PARTIAL;
    }
    else if (in_one_super_region(area.world_to_window(a), area.world_to_window(b)))
    {
        return Visibility::NONE;
    }
    else
    {
        const pair<Coord, Coord> clipped = clip_line(area, a, b);
        return area.contains(clipped.first) || area.contains(clipped.second) ? Visibility::PARTIAL : Visibility::NONE;
    }
}


