#pragma once

#include "bezier.h"

// Vector in the Bezier surface at step for geometry vectors gx, gy, and gz.
inline TVector bezier_vertex(const TVector &gx, const TVector &gy, const TVector &gz, double step)
{
    const TVector b = bezier_step_vector(step);

    return TVector(
        {
            b * gx,
            b * gy,
            b * gz,
            1
        }
    );
}

// Generate the vertices to represent a Bezier surface.
template<class Coord>
inline list<Coord> bezier_surface_vertices(const Coord &edge1, const Coord &control1, const Coord &control2, const Coord &edge2)
{
    static_assert(is_convertible<TVector, Coord>::value, "Coord must have constructor: Coord(const TVector &)");

    const TVector vx = vector_of<Coord>({ edge1, control1, control2, edge2 }, 0);
    const TVector vy = vector_of<Coord>({ edge1, control1, control2, edge2 }, 1);
    const TVector vz = vector_of<Coord>({ edge1, control1, control2, edge2 }, 2);

    list<Coord> coords;
    for (double step = 0; step < 1 || equals(step, 1); step += 0.025)
    {
        coords.push_back(Coord(bezier_vertex(vx, vy, vz, step)));
    }

    return coords;
}

