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

// Matrix with initial derivatives of forward differences for step
inline TMatrix delta_step_matrix(double step)
{
    const TVector vs = vector_of_step(step);
    return transposed(TMatrix(vs, vs, vs, vs)) * delta_coefficient_matrix();
}

// Matrix with initial deltas of forward differences
inline TMatrix delta_matrix(const TMatrix &c, double s, double t)
{
    return delta_step_matrix(s) * c * transposed(delta_step_matrix(t));
}

// Vector with initial deltas of forward differences
inline TVector delta_vector(const TVector &v, double step)
{
    const TVector sv = vector_of_step(step);
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


