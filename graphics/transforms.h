#pragma once

#include "doubles.h"

#include <vector>
#include <list>
#include <cassert>

using namespace std;

// Columns of TMatrix and representation of homogeneous coordinates
class TVector
{
public:
    constexpr static size_t count = 4;
    constexpr static size_t first_index = 0;
    constexpr static size_t last_index = count - 1;

    TVector(): TVector({ 0, 0, 0, 0}) {}

    TVector(initializer_list<double> vector): _vector(vector)
    {
        assert(_vector.size() == count);
    }

    // Sum all components of this vector.
    double sum() const
    {
        double sum = 0;
        for (size_t i = 0; i < count; i++) sum += _vector[i];
        return sum;
    }

    // Calculate the power of each component of this vector.
    TVector pow(double n) const
    {
        TVector p;
        for (size_t i = 0; i < count; i++) p._vector[i] = ::pow(_vector[i], n);
        return p;
    }

    // Retrieve the double at the i'th position.
    double operator [] (size_t i) const
    {
        return _vector[i];
    }

    // Sum this vector with another.
    TVector operator + (const TVector &other) const
    {
        TVector sum;
        for (size_t i = 0; i < count; i++) sum._vector[i] = _vector[i] + other._vector[i];
        return sum;
    }

    // Difference of this vector from another.
    TVector operator - (const TVector &other) const
    {
        TVector diff;
        for (size_t i = 0; i < count; i++) diff._vector[i] = _vector[i] - other._vector[i];
        return diff;
    }

    // Divide this vector by divisor.
    TVector operator / (double divisor) const
    {
        TVector quotient;
        for (size_t i = 0; i < count; i++) quotient._vector[i] = _vector[i] / divisor;
        return quotient;
    }

    // Multiply this vector by other.
    double operator * (const TVector &other) const
    {
        double sum = 0;
        for (size_t i = 0; i < count; i++) sum += _vector[i] * other._vector[i];
        return sum;
    }

private:
    vector<double> _vector;
};

// Sum rhs to lhs.
inline TVector& operator += (TVector &lhs, const TVector &rhs)
{
    lhs = lhs + rhs;
    return lhs;
}

// Diff rhs from lhs.
inline TVector& operator -= (TVector &lhs, const TVector &rhs)
{
    lhs = lhs - rhs;
    return lhs;
}

// Distance between a and b.
inline double distance(const TVector &a, const TVector &b)
{
    return sqrt((a - b).pow(2).sum());
}

// Equidistant vector between a and b
inline TVector equidistant(TVector a, TVector b)
{
    return TVector(
        {
           equidistant(a[0], b[0]),
           equidistant(a[1], b[1]),
           equidistant(a[2], b[2]),
           1
        }
    );
}

// Difference between a and b at the i'th position.
inline double delta(const TVector &a, const TVector &b, size_t i)
{
    assert(i >= TVector::first_index && i <= TVector::last_index);

    return a[i] - b[i];
}

// Angular coefficient of line between a and b from axis in slot from_axis on plane formed with axis in slot on_axis.
inline double angular_coefficient(const TVector &a, const TVector &b, size_t on_axis, size_t from_axis)
{
    assert(on_axis >= TVector::first_index && on_axis < TVector::last_index);
    assert(from_axis >= TVector::first_index && from_axis < TVector::last_index);

    return delta(a, b, on_axis) / delta(a, b, from_axis);
}

// Determine point in line between point a and b.
inline TVector at_step(double step, const TVector &start, const TVector &end)
{
    return TVector(
        {
            start[0] + (step * delta(end, start, 0)),
            start[1] + (step * delta(end, start, 1)),
            start[2] + (step * delta(end, start, 2)),
            1
        }
    );
}

// Transformations as a matrix
class TMatrix
{
public:
    constexpr static size_t column_count = TVector::count;
    constexpr static size_t row_count = TVector::count;

    TMatrix(
        initializer_list<double> column1,
        initializer_list<double> column2,
        initializer_list<double> column3,
        initializer_list<double> column4)
        : _column { column1, column2, column3, column4 } {}

    // Transform vector using transformation matrix.
    friend TVector operator * (TVector vector, TMatrix matrix)
    {
        return TVector({
                                   vector * matrix.column(0),
                                   vector * matrix.column(1),
                                   vector * matrix.column(2),
                                   vector * matrix.column(3)
                               });
    }

    // Multiply this matrix by other
    TMatrix operator * (TMatrix other)
    {
        double m[column_count][row_count];

        for (size_t c = 0; c < column_count; c++)
            for (size_t r = 0; r < row_count; r++)
                m[c][r] = row(r) * other.column(c);

        return TMatrix(
            { m[0][0], m[0][1], m[0][2], m[0][3] },
            { m[1][0], m[1][1], m[1][2], m[1][3] },
            { m[2][0], m[2][1], m[2][2], m[2][3] },
            { m[3][0], m[3][1], m[3][2], m[3][3] }
        );
    }

private:

    // Vector representing row at the i'th position
    TVector row(size_t i)
    {
        return TVector({ _column[0][i], _column[1][i], _column[2][i], _column[3][i] });
    }

    // Vector representing column at the i'th position
    TVector column(size_t i)
    {
        return _column[i];
    }

    TVector _column[column_count];
};

// Translation matrix: translate by dx horizontally, dy vertically, dz in depth.
inline TMatrix translation(double dx, double dy, double dz)
{
    return TMatrix(
        { 1.0, 0.0, 0.0,  dx },
        { 0.0, 1.0, 0.0,  dy },
        { 0.0, 0.0, 1.0,  dz },
        { 0.0, 0.0, 0.0, 0.0 }
    );
}

// Translation matrix to delta.
inline TMatrix translation(TVector delta)
{
    return translation(delta[0], delta[1], delta[2]);
}

// Inverse translation matrix to delta.
inline TMatrix inverse_translation(TVector delta)
{
    return translation(-delta[0], -delta[1], -delta[2]);
}

// Scaling matrix: scale x by factor sx, y by factor sy.
inline TMatrix scaling(double sx, double sy)
{
    return TMatrix(
        {  sx, 0.0, 0.0, 0.0 },
        { 0.0,  sy, 0.0, 0.0 },
        { 0.0, 0.0, 1.0, 0.0 },
        { 0.0, 0.0, 0.0, 0.0 }
    );
}

// Scaling matrix by factor from center.
inline TMatrix scaling(double factor, TVector center)
{
    return inverse_translation(center) * scaling(factor, factor) * translation(center);
}

constexpr double PI = 3.14159265;

// Rotation matrix: rotate by degrees; clockwise if angle positive; counter-clockwise if negative.
inline TMatrix rotation(double degrees)
{
    const double rad = degrees * PI / 180.0;
    const double c = cos(rad);
    const double s = sin(rad);
    return TMatrix(
        {   c,   s, 0.0, 0.0 },
        {  -s,   c, 0.0, 0.0 },
        { 0.0, 0.0, 1.0, 0.0 },
        { 0.0, 0.0, 0.0, 0.0 }
    );
}

// Rotation matrix by degrees at center; clockwise if angle positive; counter-clockwise if negative.
inline TMatrix rotation(double degrees, TVector center)
{
    return inverse_translation(center) * rotation(degrees) * translation(center);
}

// Transform coord using matrix, and assigns to lhs.
template<class Coord>
inline Coord& operator *= (Coord &lhs, TMatrix matrix)
{
    static_assert(is_convertible<TVector, Coord>::value, "Coord must have constructor: Coord(const TVector &)");
    static_assert(is_convertible<Coord, TVector>::value, "Coord must have conversion operator: operator TVector() const");

    lhs = lhs * matrix;
    return lhs;
}

// Transform coords according to m.
template<class Coord>
inline void transform(TMatrix m, list<Coord *> coords)
{
    for (auto c: coords)
        *c *= m;
}

// Translate coord by dx horizontally, dy vertically, dz in depth.
template<class Coord>
inline void translate(Coord delta, list<Coord *> coords)
{
    transform(translation(delta), coords);
}

// Scale coord by factor from center.
template<class Coord>
inline void scale(double factor, Coord center, list<Coord *> coords)
{
    static_assert(is_convertible<TVector, Coord>::value, "Coord must have constructor: Coord(const TVector &)");

    transform(scaling(factor, center), coords);
}

// Rotate coord by degrees at center; clockwise if angle positive; counter-clockwise if negative.
template<class Coord>
inline void rotate(double degrees, Coord center, list<Coord *> coords)
{
    static_assert(is_convertible<TVector, Coord>::value, "Coord must have constructor: Coord(const TVector &)");

    transform(rotation(degrees, center), coords);
}

// Create TVector with the coordinates of controls from i-3 to i, in the j-th position.
template<class Coord>
static inline TVector vector_of(const vector<Coord> &controls, size_t i, size_t j)
{
    static_assert(is_convertible<Coord, TVector>::value, "Coord must have conversion operator: operator TVector() const");

    assert(controls.size() >= TVector::count);
    assert(i >= TVector::last_index && i < controls.size());
    assert(j >= TVector::first_index && j <= TVector::last_index);

    return {
        TVector(controls[i-3])[j],
        TVector(controls[i-2])[j],
        TVector(controls[i-1])[j],
        TVector(controls[i])[j]
    };
}

// Create TVector with the coordinates of controls from 0 to 3, in the j-th position.
template<class Coord>
static inline TVector vector_of(const vector<Coord> &controls, size_t j)
{
    return vector_of(controls, TVector::last_index, j);
}

// Create TVector of step.
static inline TVector vector_of_step(double step)
{
    return { pow(step, 3), pow(step, 2), step, 1 };
}

// True if item is not found in container
template<class Container, class Item>
inline bool missing(const Container &container, const Item &item)
{
    return find(container.begin(), container.end(), item) == container.end();
}

// Center of all vertices
template<class Coord>
inline Coord center(const list<Coord *> &vertices)
{
    static_assert(is_convertible<TVector, Coord>::value, "Coord must have constructor: Coord(const TVector &)");
    static_assert(is_convertible<Coord, TVector>::value, "Coord must have conversion operator: operator TVector() const");

    list<Coord> accounted;

    TVector sum;
    for (const Coord *coord: vertices)
    {
        if (missing(accounted, *coord))
        {
            sum += *coord;

            accounted.push_back(*coord);
        }
    }

    return Coord(sum / accounted.size());
}

// Transformable elements
template<class Coord>
class Transformable
{
public:

    Transformable()
    {
        static_assert(is_convertible<TVector, Coord>::value, "Coord must have constructor: Coord(const TVector &)");
        static_assert(is_convertible<Coord, TVector>::value, "Coord must have conversion operator: operator TVector() const");
    }

    // Control coords
    virtual list<Coord *> controls() = 0;

    // Center of all controls
    virtual Coord center()
    {
        return ::center(controls());
    }

    // Transform according to matrix.
    virtual void transform(TMatrix matrix)
    {
        ::transform(matrix, controls());
    }

    // Translate by delta.
    virtual void translate(Coord delta)
    {
        ::translate(delta, controls());
    }

    // Scale by factor from center.
    virtual void scale(double factor, Coord center)
    {
        ::scale(factor, center, controls());
    }

    // Rotate by degrees at center; clockwise if degrees positive; counter-clockwise if negative.
    virtual void rotate(double degrees, Coord center)
    {
        ::rotate(degrees, center, controls());
    }

};

// New object translated by delta
template<class Coord, class Object>
Object translated(const Object &object, Coord delta)
{
    static_assert(is_base_of<Transformable<Coord>, Object>::value, "Object must derive from Transformable<Coord>.");

    Object new_object = object;
    new_object.translate(delta);

    return new_object;
}

// New object scaled by factor from center
template<class Coord, class Object>
Object scaled(const Object &object, double factor, Coord center)
{
    static_assert(is_base_of<Transformable<Coord>, Object>::value, "Object must derive from Transformable<Coord>.");

    Object new_object = object;
    new_object.scale(factor, center);

    return new_object;
}

// New object rotated by degrees at center (clockwise if angle positive or counter-clockwise if negative)
template<class Coord, class Object>
Object rotated(const Object &object, double degrees, Coord center)
{
    static_assert(is_base_of<Transformable<Coord>, Object>::value, "Object must derive from Transformable<Coord>.");

    Object new_object = object;
    new_object.rotate(degrees, center);

    return new_object;
}

// (x, y) coordinates
template<class Coord>
class XYCoord: public Transformable<Coord>
{
public:

    XYCoord(double x, double y): _x(x), _y(y)
    {
        static_assert(is_base_of<XYCoord<Coord>, Coord>::value, "Coord must derive from XYCoord<Coord>");
    }

    XYCoord(const TVector &vector): _x(vector[0]), _y(vector[1])
    {
        static_assert(is_base_of<XYCoord<Coord>, Coord>::value, "Coord must derive from XYCoord<Coord>");
    }

    double x() const { return _x; }
    double y() const { return _y; }

    // Create TVector with the these coordinates.
    operator TVector() const
    {
        return { _x, _y, 1, 1 };
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

// Determine vector in segment at x based on start and the angular coefficient m between start and the new point.
template<class Coord>
inline Coord at_x(double x, const Coord &start, double m)
{
    static_assert(is_base_of<XYCoord<Coord>, Coord>::value, "Coord must derive from XYCoord<Coord>");

    return { x, start.y() + (m * (x - start.x())) };
}

// Determine point in line at y based on start and the angular coefficient m between start and the new point.
template<class Coord>
inline Coord at_y(double y, const Coord &start, double m)
{
    static_assert(is_base_of<XYCoord<Coord>, Coord>::value, "Coord must derive from XYCoord<Coord>");

    return { start.x() + ((1/m) * (y - start.y())), y };
}

// (x, y, z) coordinates
template<class Coord>
class XYZCoord: public Transformable<Coord>
{
public:

    XYZCoord(double x, double y, double z): _x(x), _y(y), _z(z)
    {
        static_assert(is_base_of<XYZCoord<Coord>, Coord>::value, "Coord must derive from XYZCoord<Coord>");
    }

    XYZCoord(const TVector &vector): _x(vector[0]), _y(vector[1]), _z(vector[2])
    {
        static_assert(is_base_of<XYZCoord<Coord>, Coord>::value, "Coord must derive from XYZCoord<Coord>");
    }

    double x() const { return _x; }
    double y() const { return _y; }
    double z() const { return _z; }

    // Create TVector with the these coordinates.
    operator TVector() const
    {
        return { _x, _y, _z, 1 };
    }

    // True if a and b match.
    friend bool operator == (Coord a, Coord b)
    {
        return equals(a._x, b._x) && equals(a._y, b._y) && equals(a._z, b._z);
    }

    // True if a and b do not match.
    friend bool operator != (Coord a, Coord b)
    {
        return !equals(a._x, b._x) || !equals(a._y, b._y) || !equals(a._z, b._z);
    }

private:

    double _x, _y, _z;

};
