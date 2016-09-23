#pragma once

#include "clipping_cs.h"
#include "clipping_lb.h"

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
template<typename T>
class Clippable
{
public:

    // Provide clipped version of itself in area.
    virtual shared_ptr<T> clipped_in(ClippingArea &area) = 0;

};

// Clip line between World coord a and b.
inline pair<Coord, Coord> clip_line(ClippingArea &area, Coord a, Coord b)
{
    const Coord window_a = area.world_to_window(a);
    const Coord window_b = area.world_to_window(b);

    const pair<Coord, Coord> clipped_line = clip_line_using_lb(window_a, window_b);

    return make_pair(
        area.window_to_world(clipped_line.first),
        area.window_to_world(clipped_line.second)
    );
};

// Determine the visibility in area for line between a and b.
Visibility visibility(ClippingArea &area, const Coord &a, const Coord &b)
{
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


