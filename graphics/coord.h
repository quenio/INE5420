#pragma once

#include "transforms.h"
#include "doubles.h"

#include <algorithm>

using namespace std;

// 2D coordinates
class Coord: public Transformable<Coord>
{
public:
    Coord (double x, double y): _x(x), _y(y) {}

    double x() const { return _x; }
    double y() const { return _y; }

    // Create TVector with the these coordinates.
    TVector vector() const
    {
        return { _x, _y, 1, 0 };
    }

    // Distance to coord.
    double distance_to(Coord coord)
    {
        return sqrt(pow(x() - coord.x(), 2) + pow(y() - coord.y(), 2));
    }

    // Transform according to TransformationMatrix.
    void transform(TMatrix m) override;

    // Translate by dx horizontally, dy vertically.
    void translate(double dx, double dy) override;

    // Scale by factor from center.
    void scale(double factor, Coord center) override;

    // Rotate by degrees at center; clockwise if degrees positive; counter-clockwise if negative.
    void rotate(double degrees, Coord center) override;

    // New coord translated by dx horizontally, dy vertically
    Coord translated(double dx, double dy) const
    {
        Coord coord = *this;
        coord.translate(dx, dy);
        return coord;
    }

    // New coord scaled by factor from center
    Coord scaled(double factor, Coord center) const
    {
        Coord coord = *this;
        coord.scale(factor, center);
        return coord;
    }

    // New coord rotated by degrees at center (clockwise if angle positive or counter-clockwise if negative)
    Coord rotated(double degrees, Coord center) const
    {
        Coord coord = *this;
        coord.rotate(degrees, center);
        return coord;
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

// Create TVector with the x coordinates of a, b, c and d.
static inline TVector vector_of_x(const Coord &a, const Coord &b, const Coord &c, const Coord &d)
{
    return { a.x(), b.x(), c.x(), d.x() };
}

// Create TVector with the y coordinates of a, b, c and d.
static inline TVector vector_of_y(const Coord &a, const Coord &b, const Coord &c, const Coord &d)
{
    return { a.y(), b.y(), c.y(), d.y() };
}

// Create TVector with the x coordinates of controls from i-3 to i.
static inline TVector vector_of_x(const vector<Coord> &controls, size_t i)
{
    assert(controls.size() >= TVector::count);
    assert(i >= TVector::last_index && i < controls.size());

    return { controls[i-3].x(), controls[i-2].x(), controls[i-1].x(), controls[i].x() };
}

// Create TVector with the y coordinates of controls from i-3 to i.
static inline TVector vector_of_y(const vector<Coord> &controls, size_t i)
{
    assert(controls.size() >= TVector::count);
    assert(i >= TVector::last_index && i < controls.size());

    return { controls[i-3].y(), controls[i-2].y(), controls[i-1].y(), controls[i].y() };
}

// Create TVector of step.
static inline TVector vector_of_step(double step)
{
    return { pow(step, 3), pow(step, 2), step, 1 };
}

// Transform coord using transformation matrix.
inline Coord operator * (const Coord &coord, TMatrix matrix)
{
    const TVector vector = coord.vector() * matrix;
    return Coord(vector[0], vector[1]);
}

// Transform coord using transformation matrix, and assigns to lhs.
inline Coord& operator *= (Coord &lhs, TMatrix matrix)
{
    lhs = lhs * matrix;
    return lhs;
}

// Scale coord by factor from center.
inline void scale(double factor, Coord center, list<Coord *> coords)
{
    scale(factor, center.vector(), coords);
}

// Rotate coord by degrees at center; clockwise if angle positive; counter-clockwise if negative.
inline void rotate(double degrees, Coord center, list<Coord *> coords)
{
    rotate(degrees, center.vector(), coords);
}

// Transform according to the matrix.
inline void Coord::transform(TMatrix matrix)
{
    ::transform<Coord>(matrix, { this });
}

// Translate coord by dx horizontally, dy vertically.
inline void Coord::translate(double dx, double dy)
{
    ::translate<Coord>(dx, dy, { this });
}

// Scale coord by factor from center.
inline void Coord::scale(double factor, Coord center)
{
    ::scale(factor, center, { this });
}

// Rotate coord by degrees at center; clockwise if angle positive; counter-clockwise if negative.
inline void Coord::rotate(double degrees, Coord center)
{
    ::rotate(degrees, center, { this });
}

// Equidistant double between a and b.
inline double equidistant(double a, double b)
{
    return min(a, b) + (abs_diff(a, b) / 2);
}

// Equidistant coord between a and b
inline Coord equidistant(Coord a, Coord b)
{
    return Coord(equidistant(a.x(), b.x()), equidistant(a.y(), b.y()));
}

// Difference between a.x() and b.x()
inline double delta_x(const Coord &a, const Coord &b)
{
    return a.x() - b.x();
}

// Difference between a.y() and b.y()
inline double delta_y(const Coord &a, const Coord &b)
{
    return a.y() - b.y();
}

// Angular coefficient of line between a and b.
inline double angular_coefficient(const Coord &a, const Coord &b)
{
    return delta_y(a, b) / delta_x(a, b);
}

// Determine point in line at x based on start and the angular coefficient m between start and the new point.
inline Coord at_x(double x, const Coord &start, double m)
{
    return Coord(x, start.y() + (m * (x - start.x())));
}

// Determine point in line at y based on start and the angular coefficient m between start and the new point.
inline Coord at_y(double y, const Coord &start, double m)
{
    return Coord(start.x() + ((1/m) * (y - start.y())), y);
}

// Determine point in line between point a and b.
inline Coord at_step(double step, const Coord &start, const Coord &end)
{
    return Coord(
        start.x() + (step * delta_x(end, start)),
        start.y() + (step * delta_y(end, start))
    );
}

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
