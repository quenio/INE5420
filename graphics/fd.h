#pragma once

#include "transforms.h"

// Matrix used to calculate the coefficient initial deltas of forward differences
inline TMatrix delta_coefficient_matrix()
{
    return TMatrix(
        { 0, 0, 0, 1 }, // f0
        { 1, 1, 1, 0 }, // delta f0
        { 6, 2, 0, 0 }, // delta^2 f0
        { 6, 0, 0, 0 }  // delta^3 f0
    );
}

constexpr double fd_step = 0.05;

// Matrix with initial derivatives of forward differences for step
inline TMatrix delta_step_matrix()
{
    const TVector vs = vector_of_step(fd_step);
    const TMatrix c = delta_coefficient_matrix();
    return transposed(TMatrix(c.row(0) * vs[0], c.row(1) * vs[1], c.row(2) * vs[2], c.row(3) * vs[3]));
}

// Matrix with initial deltas of forward differences
inline TMatrix delta_matrix(const TMatrix &c)
{
    const TMatrix e = delta_step_matrix();
    return transposed(e) * transposed(c) * e;
}

// Vector with initial deltas of forward differences
inline TVector delta_vector(const TVector &v)
{
    const TVector sv = vector_of_step(fd_step);
    return TVector(
        {
            sv[0] * v[0],
            sv[1] * v[1],
            sv[2] * v[2],
            sv[3] * v[3]
        }
    ) * delta_coefficient_matrix();
}

// Calculate next delta vector based on previous one.
inline TVector next_delta(const TVector &d)
{
    return TVector(
        {
            d[0],
            d[1] + d[2],
            d[2] + d[3],
            d[3],
        }
    );
}

// Calculate the next delta in s.
inline void next_s_delta(TMatrix &dd)
{
    dd = TMatrix(
        dd.column(0) + dd.column(1),
        dd.column(1) + dd.column(2),
        dd.column(2) + dd.column(3),
        dd.column(3)
    );
}

// Calculate the next delta in t.
inline void next_t_delta(TMatrix &dd)
{
    dd = transposed(TMatrix(
        dd.row(0) + dd.row(1),
        dd.row(1) + dd.row(2),
        dd.row(2) + dd.row(3),
        dd.row(3)
    ));
}

// Vector with initial coord of forward differences
inline TVector initial_fd_vector(const TVector &dx, const TVector &dy, const TVector &dz)
{
    return TVector({ dx[0], dy[0], dz[0], 0 });
}

// Next vector of forward differences
template<class Coord>
inline TVector next_fd_vector(const Coord &coord, const TVector &dx, const TVector &dy, const TVector &dz)
{
    return coord * translation(dx[1], dy[1], dz[1]);
}

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

    TVector dx = delta_vector(vx * m);
    TVector dy = delta_vector(vy * m);
    TVector dz = delta_vector(vz * m);

    generate_fd_vertices(vertices, dx, dy, dz);
}

// Generate vertices using forward-differences technique.
template<class Coord>
inline void generate_fd_vertices(
    list<shared_ptr<Coord>> &vertices,
    TVector dx, TVector dy, TVector dz)
{
    static_assert(is_convertible<TVector, Coord>::value, "Coord must have constructor: Coord(const TVector &)");

    Coord current(initial_fd_vector(dx, dy, dz));
    vertices.push_back(make_shared<Coord>(current));

    for (double t = 0.0; t <= 1; t += fd_step)
    {
        const Coord next(next_fd_vector(current, dx, dy, dz));
        vertices.push_back(make_shared<Coord>(next));

        dx = next_delta(dx);
        dy = next_delta(dy);
        dz = next_delta(dz);

        current = next;
    }
}


