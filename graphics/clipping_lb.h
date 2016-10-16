// Line clipping based on the Liang-Barsky algorithm

#pragma once

#include "transforms.h"

using namespace std;

constexpr size_t LB_PARAM_SIZE = 4;
typedef double LBParam[LB_PARAM_SIZE];

// Calculate p based on Window vector a and b.
inline void lb_p(LBParam p, const TVector &a, const TVector &b)
{
    double dx = delta(b, a, 0), dy = delta(b, a, 1);
    p[0] = -dx;
    p[1] = dx;
    p[2] = -dy;
    p[3] = dy;
}

// Calculate q based on Window vector a and b.
inline void lb_q(LBParam q, const TVector &a)
{
    q[0] = a[0] + 1;
    q[1] = 1 - a[0];
    q[2] = a[1] + 1;
    q[3] = 1 - a[1];
}

// Calculate zeta-one based on p and q.
inline double zeta_one(const LBParam p, const LBParam q)
{
    double result = 0;

    for (size_t i = 0; i < LB_PARAM_SIZE; i++)
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

    for (size_t i = 0; i < LB_PARAM_SIZE; i++)
    {
        if (p[i] > 0)
        {
            result = min(result, q[i] / p[i]);
        }
    }

    return result;
}

// Clip line between Window coord a and b using Liang-Barsky.
template<class Coord>
inline pair<Coord, Coord> clip_line_using_lb(const Coord &a, const Coord &b)
{
    static_assert(is_convertible<TVector, Coord>::value, "Coord must have constructor: Coord(const TVector &)");
    static_assert(is_convertible<Coord, TVector>::value, "Coord must have conversion operator: operator TVector() const");

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
