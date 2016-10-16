#pragma once

#include "transforms.h"

using namespace std;

// 2D coordinates
class Coord: public Transformable<Coord>
{
public:

    Coord(double x, double y): _x(x), _y(y) {}

    Coord(const TVector &vector): _x(vector[0]), _y(vector[1]) {}

    double x() const { return _x; }
    double y() const { return _y; }

    // Create TVector with the these coordinates.
    operator TVector() const
    {
        return { _x, _y, 1, 0 };
    }

    // Distance to coord.
    double distance_to(Coord coord)
    {
        return sqrt(pow(x() - coord.x(), 2) + pow(y() - coord.y(), 2));
    }

    list<Coord *> controls() override
    {
        return { this };
    }

    // True if a and b match.
    friend bool operator == (Coord a, Coord b)
    {
        return equals(a._x, b._x) && equals(a._y, b._y);
    }

    // True if a and b do not match.
    friend bool operator != (Coord a, Coord b)
    {
        return !equals(a._x, b._x) || !equals(a._y, b._y);
    }

private:

    double _x, _y;

};

// True if item is not found in container
template<class Container, class T>
inline bool missing(const Container &container, const T &item)
{
    return find(container.begin(), container.end(), item) == container.end();
}

// Center of all vertices
template<class Container>
inline Coord center(const Container &vertices)
{
    list<Coord> accounted;

    double x = 0, y = 0;
    for (const Coord &coord: vertices)
    {
        if (missing(accounted, coord))
        {
            x += coord.x();
            y += coord.y();

            accounted.push_back(coord);
        }
    }

    return Coord(x / accounted.size(), y / accounted.size());
}
