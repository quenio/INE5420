// Line clipping based on the Liang-Barsky algorithm

#pragma once

#include "coord.h"

using namespace std;

constexpr size_t LB_PARAM_SIZE = 4;
typedef double LBParam[LB_PARAM_SIZE];

// Calculate p based on Window Coord a and b.
inline void lb_p(LBParam p, const Coord &a, const Coord &b)
{
    double dx = delta_x(b, a), dy = delta_y(b, a);
    p[0] = -dx;
    p[1] = dx;
    p[2] = -dy;
    p[3] = dy;
}

// Calculate q based on Window Coord a and b.
inline void lb_q(LBParam q, const Coord &a)
{
    q[0] = a.x() + 1;
    q[1] = 1 - a.x();
    q[2] = a.y() + 1;
    q[3] = 1 - a.y();
}

// Calculate zeta-one based on p and q.
inline double zeta_one(const LBParam p, const LBParam q)
{
    double result = 0;

    for (int i = 0; i < LB_PARAM_SIZE; i++)
    {
        if (p[i] < 0)
        {
            result = max(result, q[i] / p[i]);
        }
    }

    return result;
}

// Calculate zeta-two based on p and q.
inline double zeta_two(const LBParam p, const LBParam q)
{
    double result = 1;

    for (int i = 0; i < LB_PARAM_SIZE; i++)
    {
        if (p[i] > 0)
        {
            result = min(result, q[i] / p[i]);
        }
    }

    return result;
}

// Clip line between Window coord a and b using Liang-Barsky.
inline pair<Coord, Coord> clip_line_using_lb(const Coord &a, const Coord &b)
{
    LBParam p, q;
    lb_p(p, a, b);
    lb_q(q, a);

    const double z1 = zeta_one(p, q), z2 = zeta_two(p, q);

    Coord new_a = a, new_b = b;

    if (z1 < z2 || equals(z1, z2))
    {
        if (!equals(z1, 0)) new_a = at_step(z1, a, b);
        if (!equals(z2, 1)) new_b = at_step(z2, a, b);
    }

    return make_pair(new_a, new_b);
}
