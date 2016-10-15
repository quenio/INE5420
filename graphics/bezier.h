#pragma once

#include "coord.h"

// Coefficient matrix used to calculate a Bezier curve
inline TransformMatrix bezier_matrix()
{
    return TransformMatrix(
        { -1, +3, -3, +1 },
        { +3, -6, +3,  0 },
        { -3, +3,  0,  0 },
        { +1,  0,  0,  0 }
    );
}

// Vector used to calculate points in each step of a Bezier curve
inline TransformVector bezier_vector(double step)
{
    return TransformVector::of_step(step) * bezier_matrix();
}

// Generate the vertices to represent a Bezier curve.
inline list<Coord> bezier_vertices(const Coord &edge1, const Coord &control1, const Coord &control2, const Coord &edge2)
{
    const TransformVector vx = TransformVector::of_x(edge1, control1, control2, edge2);
    const TransformVector vy = TransformVector::of_y(edge1, control1, control2, edge2);

    list<Coord> coords;
    for (double step = 0; step < 1 || equals(step, 1); step += 0.025)
    {
        const TransformVector b = bezier_vector(step);
        coords.push_back(Coord(b * vx, b * vy));
    }

    return coords;
}

