#pragma once

#include <tuple>
#include <cmath>

using namespace std;

// Two-dimensional coordinates
class Coord
{
public:
    Coord (double x, double y): _x(x), _y(y) {}

    double x() const { return _x; }
    double y() const { return _y; }

    // Scale x by factor fx, y by factor fy.
    Coord scale(double fx, double fy)
    {
        return Coord(x() * fx, y() * fy);
    }

    // Move by dx horizontally, dy vertically.
    Coord translate(double dx, double dy)
    {
        return Coord(x() + dx, y() + dy);
    }

private:
    double _x, _y;
};

// 2D transformations as a matrix
class TransformMatrix
{
public:
    typedef tuple<double, double, double> Column;

    TransformMatrix(Column column1, Column column2)
        : TransformMatrix(column1, column2, { 0.0, 0.0, 1.0 }) {}

    TransformMatrix(Column column1, Column column2, Column column3)
        : _column1(column1), _column2(column2), _column3(column3) {}

private:
    Column _column1, _column2, _column3;
};

// 2D translation as a matrix
TransformMatrix translation(double dx, double dy)
{
    return TransformMatrix({ 1.0, 0.0, dx }, { 0.0, 1.0, dy });
}

// 2D scaling as a matrix
TransformMatrix scaling(double sx, double sy)
{
    return TransformMatrix({ sx, 0.0, 0.0 }, { 0.0, sy, 0.0 });
}

// 2D rotation as a matrix
TransformMatrix rotation(double angle)
{
    const double c = cos(angle);
    const double s = sin(angle);
    return TransformMatrix({ c, s, 0.0 }, { -s, c, 0.0 });
}
