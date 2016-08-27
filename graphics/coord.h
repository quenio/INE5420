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
    TransformMatrix(initializer_list<double> column1, initializer_list<double> column2, initializer_list<double> column3)
        : _column1(column1), _column2(column2), _column3(column3) {}

    // Transform vector using transformation matrix.
    friend TransformVector operator * (TransformVector vector, TransformMatrix matrix)
    {
        return TransformVector({ vector * matrix._column1, vector * matrix._column2, vector * matrix._column3 });
    }

private:
    TransformVector _column1, _column2, _column3;
};

// 2D translation as a matrix: move by dx horizontally, dy vertically.
TransformMatrix translation(double dx, double dy)
{
    return TransformMatrix({ 1.0, 0.0, dx }, { 0.0, 1.0, dy }, { 0.0, 0.0, 1.0 });
}

// 2D scaling as a matrix: scale x by factor sx, y by factor sy.
TransformMatrix scaling(double sx, double sy)
{
    return TransformMatrix({ sx, 0.0, 0.0 }, { 0.0, sy, 0.0 }, { 0.0, 0.0, 1.0 });
}

// 2D rotation as a matrix: rotate by angle; clockwise if angle positive; counter-clockwise if negative.
TransformMatrix rotation(double angle)
{
    const double c = cos(angle);
    const double s = sin(angle);
    return TransformMatrix({ c, s, 0.0 }, { -s, c, 0.0 }, { 0.0, 0.0, 1.0 });
}

// Transform coord using transformation matrix.
Coord operator * (const Coord &coord, TransformMatrix matrix)
{
    const TransformVector vector = TransformVector(coord) * matrix;
    return Coord(vector[0], vector[1]);
}

Coord& operator *= (Coord &lhs, TransformMatrix matrix)
{
    lhs = lhs * matrix;
    return lhs;
}
