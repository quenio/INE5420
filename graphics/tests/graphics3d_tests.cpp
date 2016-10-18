#include "min_unit.h"
#include "../graphics3d.h"

static const char * at_index()
{
    TVector v;
    v = Coord3D(4, 2, 3);

    mu_assert(equals(v[0], 4));
    mu_assert(equals(v[1], 2));
    mu_assert(equals(v[2], 3));
    mu_assert(equals(v[3], 1));

    return nullptr;
}

static const char * sum()
{
    mu_assert(TVector(Coord3D(1, 2, 3)) + Coord3D(2, 3, 4) == Coord3D(3, 5, 7));

    TVector v;
    v = Coord3D(1, 2, 3); // + 1
    v += Coord3D(2, 3, 4); // + 1
    mu_assert(v == Coord3D(3, 5, 7));
    mu_assert(equals(v.sum(), 17)); // (3 + 5 + 7) + (1 + 1)

    return nullptr;
}

static const char * diff()
{
    mu_assert(TVector(Coord3D(1, 2, 3)) - Coord3D(4, 3, 2) == Coord3D(-3, -1, 1));

    TVector v;
    v = Coord3D(1, 2, 3);
    v -= Coord3D(4, 3, 2);
    mu_assert(v == Coord3D(-3, -1, 1));

    return nullptr;
}

static const char * mul()
{
    mu_assert(equals(TVector(Coord3D(1, 2, 3)) * Coord3D(2, -3, 4), 9)); // (1 * 2) + (2 * -3) + (3 * 4) + (1 * 1)

    return nullptr;
}

static const char * div()
{
    mu_assert(TVector(Coord3D(1, -2, 3)) / 2 == Coord3D(0.5, -1, 1.5));

    return nullptr;
}

static const char * pow()
{
    mu_assert(TVector(Coord3D(1, -2, 3)).pow(2) == Coord3D(1, 4, 9));

    return nullptr;
}

static const char * equidistant()
{
    mu_assert(equidistant(Coord3D(1, 2, 5), Coord3D(2, -3, 4)) == Coord3D(1 + (2 - 1)/2.0, -3 + (2 + 3)/2.0, 4 + (5 - 4)/2.0));

    return nullptr;
}

static const char * delta()
{
    Coord3D a(1, 2, 5);
    Coord3D b(2, -3, 4);

    mu_assert(equals(delta(a, b, 0), -1));
    mu_assert(equals(delta(a, b, 1), 5));
    mu_assert(equals(delta(a, b, 2), 1));
    mu_assert(equals(delta(a, b, 3), 0));

    return nullptr;
}

void all_tests()
{
    mu_test(at_index);
    mu_test(sum);
    mu_test(diff);
    mu_test(mul);
    mu_test(div);
    mu_test(pow);
    mu_test(equidistant);
    mu_test(delta);
}

