#pragma once

#include <vector>
#include <list>
#include <cmath>
#include <cassert>

using namespace std;

#define UNUSED __attribute__ ((unused))

// Absolute difference between a and b
inline double abs_diff(double a, double b)
{
    return abs(a - b);
}

// Determine if a and b are equal, accepting up to epsilon as the difference.
inline bool equals(double a, double b)
{
    constexpr double epsilon = 0.000001;
    return abs_diff(a, b) < epsilon;
}

// 2D coordinates
class Coord;

// 2D transformations as a matrix
class TransformMatrix;

// Transformable elements
class Transformable
{
public:

    // Transform according to TransformationMatrix.
    virtual void transform(TransformMatrix m) = 0;

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

    double x(double x) { this->_x = x; }
    double y(double y) { this->_y = y; }

    // Distance to coord.
    double distance_to(Coord coord)
    {
        return sqrt(pow(x() - coord.x(), 2) + pow(y() - coord.y(), 2));
    }

    // Transform according to TransformationMatrix.
    void transform(TransformMatrix m) override;

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

// Columns of TransformMatrix and representation of homogeneous coordinates
class TransformVector
{
public:
    constexpr static int count = 4;

    TransformVector(initializer_list<double> vector): _vector(vector)
    {
        assert(_vector.size() == count);
    }

    TransformVector(Coord coord): TransformVector({ coord.x(), coord.y(), 1, 0 }) {}

    // Create TransformVector with the x coordinates of a, b, c and d.
    static inline TransformVector of_x(const Coord &a, const Coord &b, const Coord &c, const Coord &d)
    {
        return { a.x(), b.x(), c.x(), d.x() };
    }

    // Create TransformVector with the y coordinates of a, b, c and d.
    static inline TransformVector of_y(const Coord &a, const Coord &b, const Coord &c, const Coord &d)
    {
        return { a.y(), b.y(), c.y(), d.y() };
    }

    // Create TransformVector of step.
    static inline TransformVector of_step(double step)
    {
        return { pow(step, 3), pow(step, 2), step, 1 };
    }

    // Retrieve the double at the i'th position.
    double operator [] (size_t i) const
    {
        return _vector[i];
    }

    // Multiply this vector by other.
    double operator * (const TransformVector other) const
    {
        double sum = 0;
        for (size_t i = 0; i < count; i++) sum += _vector[i] * other._vector[i];
        return sum;
    }

private:
    vector<double> _vector;
};

// Transformations as a matrix
class TransformMatrix
{
public:
    constexpr static size_t column_count = TransformVector::count;
    constexpr static size_t row_count = TransformVector::count;

    TransformMatrix(
        initializer_list<double> column1,
        initializer_list<double> column2,
        initializer_list<double> column3,
        initializer_list<double> column4)
    : _column { column1, column2, column3, column4 } {}

    // Transform vector using transformation matrix.
    friend TransformVector operator * (TransformVector vector, TransformMatrix matrix)
    {
        return TransformVector({
            vector * matrix.column(0),
            vector * matrix.column(1),
            vector * matrix.column(2),
            vector * matrix.column(3)
        });
    }

    // Multiply this matrix by other
    TransformMatrix operator * (TransformMatrix other)
    {
        double m[column_count][row_count];

        for (size_t c = 0; c < column_count; c++)
            for (size_t r = 0; r < row_count; r++)
                m[c][r] = row(r) * other.column(c);

        return TransformMatrix(
           { m[0][0], m[0][1], m[0][2], m[0][3] },
           { m[1][0], m[1][1], m[1][2], m[1][3] },
           { m[2][0], m[2][1], m[2][2], m[2][3] },
           { m[3][0], m[3][1], m[3][2], m[3][3] }
        );
    }

private:

    // Vector representing row at the i'th position
    TransformVector row(size_t i)
    {
        return TransformVector({ _column[0][i], _column[1][i], _column[2][i], _column[3][i] });
    }

    // Vector representing column at the i'th position
    TransformVector column(size_t i)
    {
        return _column[i];
    }

    TransformVector _column[column_count];
};

// Translation as a matrix: translate by dx horizontally, dy vertically.
inline TransformMatrix translation(double dx, double dy)
{
    return TransformMatrix(
        { 1.0, 0.0,  dx, 0.0 },
        { 0.0, 1.0,  dy, 0.0 },
        { 0.0, 0.0, 1.0, 0.0 },
        { 0.0, 0.0, 0.0, 0.0 }
    );
}

// Scaling as a matrix: scale x by factor sx, y by factor sy.
inline TransformMatrix scaling(double sx, double sy)
{
    return TransformMatrix(
        {  sx, 0.0, 0.0, 0.0 },
        { 0.0,  sy, 0.0, 0.0 },
        { 0.0, 0.0, 1.0, 0.0 },
        { 0.0, 0.0, 0.0, 0.0 }
    );
}

// Scaling coord by factor from center.
inline TransformMatrix scaling(double factor, Coord center)
{
    return translation(-center.x(), -center.y()) *
           scaling(factor, factor) *
           translation(center.x(), center.y());
}

constexpr double PI = 3.14159265;

// Rotation as a matrix: rotate by degrees; clockwise if angle positive; counter-clockwise if negative.
inline TransformMatrix rotation(double degrees)
{
    const double rad = degrees * PI / 180.0;
    const double c = cos(rad);
    const double s = sin(rad);
    return TransformMatrix(
        {   c,   s, 0.0, 0.0 },
        {  -s,   c, 0.0, 0.0 },
        { 0.0, 0.0, 1.0, 0.0 },
        { 0.0, 0.0, 0.0, 0.0 }
    );
}

// Rotation coord by degrees at center; clockwise if angle positive; counter-clockwise if negative.
inline TransformMatrix rotation(double degrees, Coord center)
{
    return translation(-center.x(), -center.y()) *
           rotation(degrees) *
           translation(center.x(), center.y());
}

// Bezier matrix
inline TransformMatrix bezier()
{
    return TransformMatrix(
        { -1, +3, -3, +1 },
        { +3, -6, +3,  0 },
        { -3, +3,  0,  0 },
        { +1,  0,  0,  0 }
    );
}

// Bezier vector
inline TransformVector bezier(double step)
{
    return TransformVector::of_step(step) * bezier();
}

// Spline matrix
inline TransformMatrix spline()
{
    return TransformMatrix(
            { -1/6, +1/2, -1/2, +1/6 },
            { +1/2, -1  ,  0  , +4/6 },
            { -1/2, +1/2, +1/2, +1/6 },
            { +1/6,  0  ,  0  ,  0   }
    );
}

// Spline delta matrix
inline TransformMatrix spline(double step)
{
    double step2 = step * step;
    double step3 = step2 * step;

    return TransformMatrix(
            { step3, 6 * step3, 6 * step3, 0},
            { step2, 0        , 2 * step2, 0},
            { step , 0        , 0        , 0},
            { 0    , 0        , 0        , 0}
    );
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
    transform(scaling(factor, center), coords);
}

// Rotate coord by degrees at center; clockwise if angle positive; counter-clockwise if negative.
inline void rotate(double degrees, Coord center, list<Coord *> coords)
{
    transform(rotation(degrees, center), coords);
}

// Transform according to TransformationMatrix.
inline void Coord::transform(TransformMatrix m)
{
    ::transform(m, { this });
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

// Generate the vertices to represent a Bezier curve.
inline list<Coord> bezier_vertices(const Coord &edge1, const Coord &control1, const Coord &control2, const Coord &edge2)
{
    const TransformVector vx = TransformVector::of_x(edge1, control1, control2, edge2);
    const TransformVector vy = TransformVector::of_y(edge1, control1, control2, edge2);

    list<Coord> coords;
    for (double step = 0; step < 1 || equals(step, 1); step += 0.025)
    {
        const TransformVector b = bezier(step);
        coords.push_back(Coord(b * vx, b * vy));
    }

    return coords;
}

// Generate the vertices to represent a Spline curve.
inline list<Coord> spline_vertices(vector<Coord> controls, double step)
{
    list<Coord> result;

    if (controls.size() < 4)
    {
        //TODO Houston we have a problem.
        return result;
    }

    long nCurves = controls.size() - 3;
    
    for (int curve = 0; curve < nCurves; curve++)
    {
        auto c1 = controls[curve];
        auto c2 = controls[curve + 1];
        auto c3 = controls[curve + 2];
        auto c4 = controls[curve + 3];
        
        TransformVector vx = TransformVector::of_x(c1, c2, c3, c4) * spline();
        TransformVector vy = TransformVector::of_y(c1, c2, c3, c4) * spline();

        TransformVector dx = vx * spline(step);
        TransformVector dy = vy * spline(step);

        Coord oldC = *new Coord(vx[3], vy[3]);
        result.push_back(oldC);

        double deltaX = dx[0];
        double deltaX2 = dx[1];
        double deltaX3 = dx[2];

        double deltaY = dy[0];
        double deltaY2 = dy[1];
        double deltaY3 = dy[2];
        
        for (double t = 0; t <= 1; t += step)
        {
            Coord newC = oldC;
            newC.x(newC.x() + deltaX);
            newC.y(newC.y() + deltaY);

            deltaX = deltaX + deltaX2;
            deltaX2 = deltaX2 + deltaX3;

            deltaY = deltaY + deltaY2;
            deltaY2 = deltaY2 + deltaY3;

            result.push_back(newC);
            oldC = newC;
        }
    }
}
