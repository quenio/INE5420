#pragma once

#include "fd.h"

// Generate vertices using forward-differences technique.
template<class Coord>
inline void generate_fd_vertices(
    list<shared_ptr<Coord>> &vertices,
    const TVector &vx,
    const TVector &vy,
    const TVector &vz,
    const TMatrix &m)
{
    static_assert(is_convertible<TVector, Coord>::value, "Coord must have constructor: Coord(const TVector &)");

    constexpr double step = 0.025;

    TVector dx = delta_vector(vx * m, step);
    TVector dy = delta_vector(vy * m, step);
    TVector dz = delta_vector(vz * m, step);

    Coord current(initial_fd_vector(dx, dy, dz));
    vertices.push_back(make_shared<Coord>(current));

    for (double t = 0.0; t <= 1; t += step)
    {
        const Coord next(next_fd_vector(current, dx, dy, dz));
        vertices.push_back(make_shared<Coord>(next));

        dx = next_delta(dx);
        dy = next_delta(dy);
        dz = next_delta(dz);

        current = next;
    }
}

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
