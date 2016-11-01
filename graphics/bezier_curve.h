#pragma once

#include "transforms.h"

// Vector used to calculate points in each step of a Bezier curve
inline TVector bezier_step_vector(double step)
{
    return vector_of_step(step) * bezier;
}

// Vector in the Bezier curve at step for geometry vectors gx and gy.
inline TVector bezier_vertex(const TVector &gx, const TVector &gy, double step)
{
    const TVector b = bezier_step_vector(step);

    return TVector(
        {
            b * gx,
            b * gy,
            1,
            1
        }
    );
}

// Generate the vertices to represent a Bezier curve.
template<class Coord>
inline list<Coord> bezier_curve_vertices(const Coord &edge1, const Coord &control1, const Coord &control2,
                                         const Coord &edge2)
{
    static_assert(is_convertible<TVector, Coord>::value, "Coord must have constructor: Coord(const TVector &)");

    const TVector gx = vector_of<Coord>({ edge1, control1, control2, edge2 }, 0);
    const TVector gy = vector_of<Coord>({ edge1, control1, control2, edge2 }, 1);

    list<Coord> coords;
    for (double step = 0; step < 1 || equals(step, 1); step += 0.025)
    {
        coords.push_back(Coord(bezier_vertex(gx, gy, step)));
    }

    return coords;
}

