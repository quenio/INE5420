#pragma once

#include "transforms.h"

using namespace std;

// 2D coordinates
class Coord2D: public Transformable<Coord2D>
{
public:

    Coord2D(double x, double y): _x(x), _y(y) {}

    Coord2D(const TVector &vector): _x(vector[0]), _y(vector[1]) {}

    double x() const { return _x; }
    double y() const { return _y; }

    // Create TVector with the these coordinates.
    operator TVector() const
    {
        return { _x, _y, 1, 0 };
    }

    list<Coord2D *> controls() override
    {
        return { this };
    }

    // True if a and b match.
    friend bool operator == (Coord2D a, Coord2D b)
    {
        return equals(a._x, b._x) && equals(a._y, b._y);
    }

    // True if a and b do not match.
    friend bool operator != (Coord2D a, Coord2D b)
    {
        return !equals(a._x, b._x) || !equals(a._y, b._y);
    }

private:

    double _x, _y;

};

