#pragma once

#include "fd.h"

// Generate the vertices to represent a Spline curve.
template<class Coord>
inline list<shared_ptr<Coord>> spline_curve_vertices(vector<Coord> controls)
{
    constexpr size_t start = TVector::last_index;

    assert(controls.size() > start);

    list<shared_ptr<Coord>> result;

    for (size_t i = start; i < controls.size(); i++)
    {
        generate_fd_vertices(
            result,
            vector_of(controls, i, 0),
            vector_of(controls, i, 1),
            vector_of(controls, i, 2),
            spline);
    }

    return result;
}
