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


