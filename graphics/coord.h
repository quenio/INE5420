#pragma once

#include <vector>
#include <cmath>
#include <cassert>

using namespace std;

// 2D coordinates
class Coord
{
public:
    Coord (double x, double y): _x(x), _y(y) {}

    double x() const { return _x; }
    double y() const { return _y; }

    // Translate by dx horizontally, dy vertically.
    Coord translate(double dx, double dy);

    // Scale by factor.
    Coord scale(double factor);

    // Scale by factor from center.
    Coord scale(double factor, Coord center);

    // Rotate by degrees; clockwise if angle positive; counter-clockwise if negative.
    Coord rotate(double degrees);

    // Rotate by degrees at center; clockwise if angle positive; counter-clockwise if negative.
    Coord rotate(double degrees, Coord center);

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
TransformMatrix translation(double dx, double dy)
{
    return TransformMatrix({ 1.0, 0.0, dx }, { 0.0, 1.0, dy }, { 0.0, 0.0, 1.0 });
}

// 2D scaling as a matrix: scale x by factor sx, y by factor sy.
TransformMatrix scaling(double sx, double sy)
{
    return TransformMatrix({ sx, 0.0, 0.0 }, { 0.0, sy, 0.0 }, { 0.0, 0.0, 1.0 });
}

constexpr double PI = 3.14159265;

// 2D rotation as a matrix: rotate by degrees; clockwise if angle positive; counter-clockwise if negative.
TransformMatrix rotation(double degrees)
{
    const double rad = degrees * PI / 180.0;
    const double c = cos(rad);
    const double s = sin(rad);
    return TransformMatrix({ c, s, 0.0 }, { -s, c, 0.0 }, { 0.0, 0.0, 1.0 });
}

// Transform coord using transformation matrix.
Coord operator * (const Coord &coord, TransformMatrix matrix)
{
    const TransformVector vector = TransformVector(coord) * matrix;
    return Coord(vector[0], vector[1]);
}

// Transform coord using transformation matrix, and assigns to lhs.
Coord& operator *= (Coord &lhs, TransformMatrix matrix)
{
    lhs = lhs * matrix;
    return lhs;
}

// Translate coord by dx horizontally, dy vertically.
void translate(Coord &coord, double dx, double dy)
{
    coord *= translation(dx, dy);
}

// Scale coord by factor.
void scale(Coord &coord, double factor)
{
    coord *= scaling(factor, factor);
}

// Scale coord by factor from center.
void scale(Coord &coord, double factor, Coord center)
{
    coord *= translation(-center.x(), -center.y()) * scaling(factor, factor) * translation(center.x(), center.y());
}

// Rotate coord by degrees at the world origin; clockwise if angle positive; counter-clockwise if negative.
void rotate(Coord & coord, double degrees)
{
    coord *= rotation(degrees);
}

// Rotate coord by degrees at center; clockwise if angle positive; counter-clockwise if negative.
void rotate(Coord & coord, double degrees, Coord center)
{
    coord *= translation(-center.x(), -center.y()) * rotation(degrees) * translation(center.x(), center.y());
}

// Translate coord by dx horizontally, dy vertically.
inline Coord Coord::translate(double dx, double dy)
{
    Coord coord = *this;
    ::translate(coord, dx, dy);
    return coord;
}

// Scale coord by factor.
inline Coord Coord::scale(double factor)
{
    Coord coord = *this;
    ::scale(coord, factor);
    return coord;
}

// Scale coord by factor from center.
inline Coord Coord::scale(double factor, Coord center)
{
    Coord coord = *this;
    ::scale(coord, factor, center);
    return coord;
}

// Rotate coord by degrees at the world origin; clockwise if angle positive; counter-clockwise if negative.
inline Coord Coord::rotate(double degrees)
{
    Coord coord = *this;
    ::rotate(coord, degrees);
    return coord;
}

// Rotate coord by degrees at center; clockwise if angle positive; counter-clockwise if negative.
inline Coord Coord::rotate(double degrees, Coord center)
{
    Coord coord = *this;
    ::rotate(coord, degrees, center);
    return coord;
}