#pragma once

#include <vector>
#include <list>
#include <cmath>
#include <cassert>

using namespace std;

// 2D coordinates
class Coord;

// Transformable elements
class Transformable
{
public:

    // Translate by dx horizontally, dy vertically.
    virtual void translate(double dx, double dy) = 0;

    // Scale by factor from center.
    virtual void scale(double factor, Coord center) = 0;

    // Rotate by degrees at center; clockwise if degrees positive; counter-clockwise if negative.
    virtual void rotate(double degrees, Coord center) = 0;

};

// 2D coordinates
class Coord: public Transformable
{
public:
    Coord (double x, double y): _x(x), _y(y) {}

    double x() const { return _x; }
    double y() const { return _y; }

    // Distance to coord.
    double distance_to(Coord coord)
    {
        return sqrt(pow(x() - coord.x(), 2) + pow(y() - coord.y(), 2));
    }

    // Translate by dx horizontally, dy vertically.
    void translate(double dx, double dy) override;

    // Scale by factor from center.
    void scale(double factor, Coord center) override;

    // Rotate by degrees at center; clockwise if degrees positive; counter-clockwise if negative.
    void rotate(double degrees, Coord center) override;

    // Translated by dx horizontally, dy vertically
    Coord translated(double dx, double dy)
    {
        Coord coord = *this;
        coord.translate(dx, dy);
        return coord;
    }

    // Scaled by factor from center
    Coord scaled(double factor, Coord center)
    {
        Coord coord = *this;
        coord.scale(factor, center);
        return coord;
    }

    // Rotated by degrees at center (clockwise if angle positive or counter-clockwise if negative)
    Coord rotated(double degrees, Coord center)
    {
        Coord coord = *this;
        coord.rotate(degrees, center);
        return coord;
    }

    // True if a and b match.
    friend bool operator == (Coord a, Coord b)
    {
        return a._x == b._x && a._y == b._y;
    }

    // True if a and b do not match.
    friend bool operator != (Coord a, Coord b)
    {
        return a._x != b._x || a._y != b._y;
    }

private:

    double _x, _y;

};

// Columns of TransformMatrix and representation of homogeneous coordinates
class TransformVector
{
public:
    constexpr static int count = 3;

    TransformVector(initializer_list<double> vector): _vector(vector)
    {
        assert(_vector.size() == count);
    }

    TransformVector(Coord coord): TransformVector({ coord.x(), coord.y(), 1 }) {}

    // Retrieve the double at the i'th position.
    double operator [] (int i) const
    {
        return _vector[i];
    }

    // Multiply this vector by other.
    double operator * (TransformVector other)
    {
        double sum = 0;
        for (int i = 0; i < count; i++) sum += _vector[i] * other._vector[i];
        return sum;
    }

private:
    vector<double> _vector;
};

// 2D transformations as a matrix
class TransformMatrix
{
public:
    constexpr static int column_count = TransformVector::count;
    constexpr static int row_count = TransformVector::count;

    TransformMatrix(initializer_list<double> column1, initializer_list<double> column2, initializer_list<double> column3)
    : _column { column1, column2, column3 } {}

    // Transform vector using transformation matrix.
    friend TransformVector operator * (TransformVector vector, TransformMatrix matrix)
    {
        return TransformVector({ vector * matrix.column(0), vector * matrix.column(1), vector * matrix.column(2) });
    }

    // Multiply this matrix by other
    TransformMatrix operator * (TransformMatrix other)
    {
        double m[column_count][row_count];

        for (int c = 0; c < column_count; c++)
            for (int r = 0; r < row_count; r++)
                m[c][r] = row(r) * other.column(c);

        return TransformMatrix(
           { m[0][0], m[0][1], m[0][2] },
           { m[1][0], m[1][1], m[1][2] },
           { m[2][0], m[2][1], m[2][2] }
        );
    }

private:

    // Vector representing row at the i'th position
    TransformVector row(int i)
    {
        return TransformVector({ _column[0][i], _column[1][i], _column[2][i] });
    }

    // Vector representing column at the i'th position
    TransformVector column(int i)
    {
        return _column[i];
    }

    TransformVector _column[column_count];
};

// 2D translation as a matrix: translate by dx horizontally, dy vertically.
inline TransformMatrix translation(double dx, double dy)
{
    return TransformMatrix({ 1.0, 0.0, dx }, { 0.0, 1.0, dy }, { 0.0, 0.0, 1.0 });
}

// 2D scaling as a matrix: scale x by factor sx, y by factor sy.
inline TransformMatrix scaling(double sx, double sy)
{
    return TransformMatrix({ sx, 0.0, 0.0 }, { 0.0, sy, 0.0 }, { 0.0, 0.0, 1.0 });
}

constexpr double PI = 3.14159265;

// 2D rotation as a matrix: rotate by degrees; clockwise if angle positive; counter-clockwise if negative.
inline TransformMatrix rotation(double degrees)
{
    const double rad = degrees * PI / 180.0;
    const double c = cos(rad);
    const double s = sin(rad);
    return TransformMatrix({ c, s, 0.0 }, { -s, c, 0.0 }, { 0.0, 0.0, 1.0 });
}

// Transform coord using transformation matrix.
inline Coord operator * (const Coord &coord, TransformMatrix matrix)
{
    const TransformVector vector = TransformVector(coord) * matrix;
    return Coord(vector[0], vector[1]);
}

// Transform coord using transformation matrix, and assigns to lhs.
inline Coord& operator *= (Coord &lhs, TransformMatrix matrix)
{
    lhs = lhs * matrix;
    return lhs;
}

// Transform coords according to m.
inline void transform(TransformMatrix m, list<Coord *> coords)
{
    for (auto c: coords)
        *c *= m;
}

// Translate coord by dx horizontally, dy vertically.
inline void translate(double dx, double dy, list<Coord *> coords)
{
    transform(translation(dx, dy), coords);
}

// Scale coord by factor from center.
inline void scale(double factor, Coord center, list<Coord *> coords)
{
    transform(
        translation(-center.x(), -center.y()) *
        scaling(factor, factor) *
        translation(center.x(), center.y()),
        coords
    );
}

// Rotate coord by degrees at center; clockwise if angle positive; counter-clockwise if negative.
inline void rotate(double degrees, Coord center, list<Coord *> coords)
{
    transform(
        translation(-center.x(), -center.y()) *
        rotation(degrees) *
        translation(center.x(), center.y()),
        coords
    );
}

// Translate coord by dx horizontally, dy vertically.
inline void Coord::translate(double dx, double dy)
{
    ::translate(dx, dy, { this });
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

// Absolute difference between a and b
inline double abs_diff(double a, double b)
{
    return abs(a - b);
}

// Determine if a and b are equal, accepting up to epsilon as the difference.
inline double equals(double a, double b)
{
    constexpr double epsilon = 0.000001;
    return abs_diff(a, b) < epsilon;
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

