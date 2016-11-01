#pragma once

#include "transforms.h"

// Coefficient matrix used to calculate a Spline curve
inline TMatrix spline_matrix()
{
    return TMatrix(
        { -1.0/6.0,      0.5,    -0.5, 1.0/6.0 },
        {      0.5,     -1.0,     0.5,     0.0 },
        {     -0.5,      0.0,     0.5,     0.0 },
        {  1.0/6.0,  4.0/6.0, 1.0/6.0,     0.0 }
    );
}

