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
    Coord translated(double dx, double dy) const
    {
        Coord coord = *this;
        coord.translate(dx, dy);
        return coord;
    }

    // Scaled by factor from center
    Coord scaled(double factor, Coord center) const
    {
        Coord coord = *this;
        coord.scale(factor, center);
        return coord;
    }

    // Rotated by degrees at center (clockwise if angle positive or counter-clockwise if negative)
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

// Columns of TransformMatrix and representation of homogeneous coordinates
class TransformVector
{
public:
    constexpr static size_t count = 4;
    constexpr static size_t last_index = count - 1;

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

    // Create TransformVector with the x coordinates of controls from i-3 to i.
    static inline TransformVector of_x(const vector<Coord> &controls, size_t i)
    {
        assert(controls.size() >= count);
        assert(i >= last_index && i < controls.size());

        return { controls[i-3].x(), controls[i-2].x(), controls[i-1].x(), controls[i].x() };
    }

    // Create TransformVector with the y coordinates of controls from i-3 to i.
    static inline TransformVector of_y(const vector<Coord> &controls, size_t i)
    {
        assert(controls.size() >= count);
        assert(i >= last_index && i < controls.size());

        return { controls[i-3].y(), controls[i-2].y(), controls[i-1].y(), controls[i].y() };
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

// Matrix used to calculate a Bezier curve
inline TransformMatrix bezier_matrix()
{
    return TransformMatrix(
        { -1, +3, -3, +1 },
        { +3, -6, +3,  0 },
        { -3, +3,  0,  0 },
        { +1,  0,  0,  0 }
    );
}

// Vector used to calculate a Bezier curve
inline TransformVector bezier_vector(double step)
{
    return TransformVector::of_step(step) * bezier_matrix();
}

// Matrix used to calculate a Spline curve
inline TransformMatrix spline_matrix()
{
    return TransformMatrix(
        { -1.0/6.0,      0.5,    -0.5, 1.0/6.0 },
        {      0.5,     -1.0,     0.5,     0.0 },
        {     -0.5,      0.0,     0.5,     0.0 },
        {  1.0/6.0,  4.0/6.0, 1.0/6.0,     0.0 }
    );
}

// Matrix used to calculate initial deltas of forward differences
inline TransformMatrix delta_matrix()
{
    return TransformMatrix(
        { 1.0, 1.0, 1.0, 0.0 },
        { 6.0, 2.0, 0.0, 0.0 },
        { 6.0, 0.0, 0.0, 0.0 },
        { 0.0, 0.0, 0.0, 1.0 }
    );
}

// Vector with initial deltas of forward differences
inline TransformVector delta_vector(const TransformVector &v, double step)
{
    const TransformVector sv = TransformVector::of_step(step);
    return TransformVector(
        {
          sv[0] * v[0],
          sv[1] * v[1],
          sv[2] * v[2],
          sv[3] * v[3]
        }
    ) * delta_matrix();
}

// Calculate next delta vector based on previous one.
inline TransformVector next_delta(const TransformVector &d)
{
    return TransformVector(
        {
            d[0] + d[1],
            d[1] + d[2],
            d[2],
            d[3]
        }
    );
}

// Generate vertices using forward-differences technique.
inline void generate_fd_vertices(
    list<Coord> &vertices,
    const TransformVector &vx,
    const TransformVector &vy,
    const TransformMatrix &m)
{
    constexpr double step = 0.025;

    TransformVector dx = delta_vector(vx * m, step);
    TransformVector dy = delta_vector(vy * m, step);

    Coord current(dx[3], dy[3]);
    vertices.push_back(current);

    for (double t = 0.0; t <= 1; t += step)
    {
        const Coord next = current.translated(dx[0], dy[0]);
        vertices.push_back(next);

        dx = next_delta(dx);
        dy = next_delta(dy);

        current = next;
    }
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
        const TransformVector b = bezier_vector(step);
        coords.push_back(Coord(b * vx, b * vy));
    }

    return coords;
}

// Generate the vertices to represent a Spline curve.
inline list<Coord> spline_vertices(vector<Coord> controls)
{
    constexpr size_t start = TransformVector::last_index;

    assert(controls.size() > start);

    list<Coord> result;

    for (size_t i = start; i < controls.size(); i++)
    {
        generate_fd_vertices(
            result,
            TransformVector::of_x(controls, i),
            TransformVector::of_y(controls, i),
            spline_matrix());
    }

    return result;
}
