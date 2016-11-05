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
    const TMatrix ddx = delta_matrix(cx), ddy = delta_matrix(cy), ddz = delta_matrix(cz);

    const double min = 0, max = 1;

    TMatrix sddx = ddx, sddy = ddy, sddz = ddz;
    for (double s = min; s < max || equals(s, max); s += fd_step)
    {
        TVector dx = sddx.column(0), dy = sddy.column(0), dz = sddz.column(0);

        generate_fd_vertices(vertices, dx, dy, dz);

        // Not drawing lines between curves.
        vertices.push_back(nullptr);

        next_s_delta(sddx);
        next_s_delta(sddy);
        next_s_delta(sddz);
    }

    TMatrix tddx = ddx, tddy = ddy, tddz = ddz;
    for (double t = min; t < max || equals(t, max); t += fd_step)
    {
        TVector dx = tddx.row(0), dy = tddy.row(0), dz = tddz.row(0);

        generate_fd_vertices(vertices, dx, dy, dz);

        // Not drawing lines between curves.
        vertices.push_back(nullptr);

        next_t_delta(tddx);
        next_t_delta(tddy);
        next_t_delta(tddz);
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




