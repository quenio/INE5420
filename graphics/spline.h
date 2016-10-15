#pragma once

#include "coord.h"

// Coefficient matrix used to calculate a Spline curve
inline TransformMatrix spline_matrix()
{
    return TransformMatrix(
        { -1.0/6.0,      0.5,    -0.5, 1.0/6.0 },
        {      0.5,     -1.0,     0.5,     0.0 },
        {     -0.5,      0.0,     0.5,     0.0 },
        {  1.0/6.0,  4.0/6.0, 1.0/6.0,     0.0 }
    );
}

// Matrix used to calculate initial deltas of forward differences
inline TransformMatrix delta_matrix()
{
    return TransformMatrix(
        { 1, 1, 1, 0 },
        { 6, 2, 0, 0 },
        { 6, 0, 0, 0 },
        { 0, 0, 0, 1 }
    );
}

// Vector with initial deltas of forward differences
inline TransformVector delta_vector(const TransformVector &v, double step)
{
    const TransformVector sv = TransformVector::of_step(step);
    return TransformVector(
        {
            sv[0] * v[0],
            sv[1] * v[1],
            sv[2] * v[2],
            sv[3] * v[3]
        }
    ) * delta_matrix();
}

// Calculate next delta vector based on previous one.
inline TransformVector next_delta(const TransformVector &d)
{
    return TransformVector(
        {
            d[0] + d[1],
            d[1] + d[2],
            d[2],
            d[3]
        }
    );
}

// Generate vertices using forward-differences technique.
inline void generate_fd_vertices(
    list<Coord> &vertices,
    const TransformVector &vx,
    const TransformVector &vy,
    const TransformMatrix &m)
{
    constexpr double step = 0.025;

    TransformVector dx = delta_vector(vx * m, step);
    TransformVector dy = delta_vector(vy * m, step);

    Coord current(dx[3], dy[3]);
    vertices.push_back(current);

    for (double t = 0.0; t <= 1; t += step)
    {
        const Coord next = current.translated(dx[0], dy[0]);
        vertices.push_back(next);

        dx = next_delta(dx);
        dy = next_delta(dy);

        current = next;
    }
}

// Generate the vertices to represent a Spline curve.
inline list<Coord> spline_vertices(vector<Coord> controls)
{
    constexpr size_t start = TransformVector::last_index;

    assert(controls.size() > start);

    list<Coord> result;

    for (size_t i = start; i < controls.size(); i++)
    {
        generate_fd_vertices(
            result,
            TransformVector::of_x(controls, i),
            TransformVector::of_y(controls, i),
            spline_matrix());
    }

    return result;
}
