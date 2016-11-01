#pragma once

#include "transforms.h"

// Coefficient matrix used to calculate a Bezier curve or surface
inline TMatrix bezier_matrix()
{
    return TMatrix(
        { -1, +3, -3, +1 },
        { +3, -6, +3,  0 },
        { -3, +3,  0,  0 },
        { +1,  0,  0,  0 }
    );
}



