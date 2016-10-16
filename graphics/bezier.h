#pragma once

#include "transforms.h"

// Coefficient matrix used to calculate a Bezier curve
inline TMatrix bezier_matrix()
{
    return TMatrix(
        { -1, +3, -3, +1 },
        { +3, -6, +3,  0 },
        { -3, +3,  0,  0 },
        { +1,  0,  0,  0 }
    );
}

// Vector used to calculate points in each step of a Bezier curve
inline TVector bezier_step_vector(double step)
{
    return vector_of_step(step) * bezier_matrix();
}

// Vector in the Bezier curve at step.
inline TVector bezier_vertex(const TVector &vx, const TVector &vy, double step)
{
    const TVector b = bezier_step_vector(step);

    return TVector(
        {
            b * vx,
            b * vy,
            0,
            0
        }
    );
}

// Generate the vertices to represent a Bezier curve.
template<class Coord>
inline list<Coord> bezier_vertices(const Coord &edge1, const Coord &control1, const Coord &control2, const Coord &edge2)
{
    const TVector vx = vector_of<Coord>({ edge1, control1, control2, edge2 }, 0);
    const TVector vy = vector_of<Coord>({ edge1, control1, control2, edge2 }, 1);

    list<Coord> coords;
    for (double step = 0; step < 1 || equals(step, 1); step += 0.025)
    {
        coords.push_back(Coord(bezier_vertex(vx, vy, step)));
    }

    return coords;
}

