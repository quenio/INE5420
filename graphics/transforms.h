#pragma once

#include <vector>
#include <list>
#include <cmath>
#include <cassert>

using namespace std;

// Columns of TMatrix and representation of homogeneous coordinates
class TVector
{
public:
    constexpr static size_t count = 4;
    constexpr static size_t last_index = count - 1;

    TVector(initializer_list<double> vector): _vector(vector)
    {
        assert(_vector.size() == count);
    }

    // Retrieve the double at the i'th position.
    double operator [] (size_t i) const
    {
        return _vector[i];
    }

    // Multiply this vector by other.
    double operator * (const TVector other) const
    {
        double sum = 0;
        for (size_t i = 0; i < count; i++) sum += _vector[i] * other._vector[i];
        return sum;
    }

private:
    vector<double> _vector;
};

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

// Translation matrix: translate by dx horizontally, dy vertically.
inline TMatrix translation(double dx, double dy)
{
    return TMatrix(
        { 1.0, 0.0,  dx, 0.0 },
        { 0.0, 1.0,  dy, 0.0 },
        { 0.0, 0.0, 1.0, 0.0 },
        { 0.0, 0.0, 0.0, 0.0 }
    );
}

// Translation matrix to delta.
inline TMatrix translation(TVector delta)
{
    return translation(delta[0], delta[1]);
}

// Inverse translation matrix to delta.
inline TMatrix inverse_translation(TVector delta)
{
    return translation(-delta[0], -delta[1]);
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

// Transform coord using matrix.
template<class Coord>
// Requires Coord to have:
// - constructor: Coord(const TVector &)
// - method: TVector vector() const
inline Coord operator * (const Coord &coord, TMatrix matrix)
{
    return Coord(coord.vector() * matrix);
}

// Transform coord using matrix, and assigns to lhs.
template<class Coord>
inline Coord& operator *= (Coord &lhs, TMatrix matrix)
{
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

// Translate coord by dx horizontally, dy vertically.
template<class Coord>
inline void translate(double dx, double dy, list<Coord *> coords)
{
    transform(translation(dx, dy), coords);
}

// Scale coord by factor from center.
template<class Coord>
inline void scale(double factor, TVector center, list<Coord *> coords)
{
    transform(scaling(factor, center), coords);
}

// Rotate coord by degrees at center; clockwise if angle positive; counter-clockwise if negative.
template<class Coord>
inline void rotate(double degrees, TVector center, list<Coord *> coords)
{
    transform(rotation(degrees, center), coords);
}

// Transformable elements
template<class Coord>
class Transformable
{
public:

    // Transform according to TransformationMatrix.
    virtual void transform(TMatrix matrix) = 0;

    // Translate by dx horizontally, dy vertically.
    virtual void translate(double dx, double dy) = 0;

    // Scale by factor from center.
    virtual void scale(double factor, Coord center) = 0;

    // Rotate by degrees at center; clockwise if degrees positive; counter-clockwise if negative.
    virtual void rotate(double degrees, Coord center) = 0;

};
