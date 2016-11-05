#pragma once

#include "surfaces.h"
#include "spline_curve.h"

// Generate the vertices to represent a surface using forward-differences.
template<class Coord>
inline void generate_fd_surface_vertices(
    list<shared_ptr<Coord>> &vertices,
    const TMatrix &curve_matrix,
    const vector<Coord> &controls)
{
    static_assert(is_convertible<TVector, Coord>::value, "Coord must have constructor: Coord(const TVector &)");

    assert(controls.size() == surface_geometry_matrix_size);

    const TMatrix cx = surface_matrix(curve_matrix, controls, 0);
    const TMatrix cy = surface_matrix(curve_matrix, controls, 1);
    const TMatrix cz = surface_matrix(curve_matrix, controls, 2);

    static_assert(is_convertible<TVector, Coord>::value, "Coord must have constructor: Coord(const TVector &)");

    const double min = 0, max = 1;

    TMatrix ddx = delta_matrix(cx), ddy = delta_matrix(cy), ddz = delta_matrix(cz);

    for (double s = min; s < max || equals(s, max); s += fd_step)
    {
        TVector dx = ddx.column(0), dy = ddy.column(0), dz = ddz.column(0);

        generate_fd_vertices(vertices, dx, dy, dz);

        // Not drawing lines between curves.
        vertices.push_back(nullptr);

        next_s_delta(ddx);
        next_s_delta(ddy);
        next_s_delta(ddz);
    }
}

// Generate the vertices to represent a surface.
template<class Coord>
inline list<shared_ptr<Coord>> fd_surface_vertices(const TMatrix &curve_matrix, const vector<vector<Coord>> &controls)
{
    static_assert(is_convertible<TVector, Coord>::value, "Coord must have constructor: Coord(const TVector &)");

    list<shared_ptr<Coord>> vertices;

    for (auto &c: controls)
    {
        generate_fd_surface_vertices(vertices, curve_matrix, c);
    }

    return vertices;
}




