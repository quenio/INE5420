#pragma once

#include "transforms.h"

constexpr size_t surface_geometry_matrix_size = TMatrix::cell_count;

// Create TMatrix with the coordinates of controls from 0 to 16, extracting the j-th position of each vector.
template<class Coord>
TMatrix surface_geometry_matrix(const vector<Coord> controls, size_t j)
{
    static_assert(is_convertible<Coord, TVector>::value, "Coord must have conversion operator: operator TVector() const");

    assert(controls.size() == surface_geometry_matrix_size);
    assert(j >= TVector::first_index && j <= TVector::last_index);

    return TMatrix(
        vector_of<Coord>({ controls[0], controls[4], controls[8],  controls[12] }, j),
        vector_of<Coord>({ controls[1], controls[5], controls[9],  controls[13] }, j),
        vector_of<Coord>({ controls[2], controls[6], controls[10], controls[14] }, j),
        vector_of<Coord>({ controls[3], controls[7], controls[11], controls[15] }, j)
    );
}

// Create TMatrix for surface, from controls i to i+16, extracting the j-th position of each vector.
template<class Coord>
TMatrix surface_matrix(const TMatrix &curve_matrix, const vector<Coord> controls, size_t j)
{
    return transposed(curve_matrix) * surface_geometry_matrix(controls, j) * curve_matrix;
}

// Generate the vertices to represent a surface.
template<class Coord, typename CoordMaker>
inline void generate_surface_vertices(list<shared_ptr<Coord>> &coords, CoordMaker make_coord, bool transversal = false)
{
    static_assert(is_convertible<TVector, Coord>::value, "Coord must have constructor: Coord(const TVector &)");

    double min = 0, max = 1;
    double ss = min, sf = max, ds = 0.05;
    double ts = min, tf = max, dt = ds;

    if (transversal)
    {
        swap(ss, sf);
        ds = -ds;
    }

    for (double s = ss; (s > min && s < max) || equals(s, min) || equals(s, max); s += ds)
    {
        const TVector vs = vector_of_step(s);

        for (double t = ts; (t > min && t < max) || equals(t, min) || equals(t, max); t += dt)
        {
            const TVector vt = vector_of_step(t);

            coords.push_back(transversal ? make_coord(vt, vs) : make_coord(vs, vt));
        }

        swap(ts, tf);
        dt = -dt;
    }
}

// Generate the transversal vertices to represent a surface.
template<class Coord, typename CoordMaker>
inline void generate_transversal_surface_vertices(list<Coord> &coords, CoordMaker make_coord)
{
    generate_surface_vertices(coords, make_coord, true);
};

// Generate the transversal vertices to represent a surface.
template<class Coord, typename CoordMaker>
inline void generate_surface_mesh(list<shared_ptr<Coord>> &coords, CoordMaker make_coord)
{
    generate_surface_vertices(coords, make_coord);
    generate_transversal_surface_vertices(coords, make_coord);
};

// Generate the vertices to represent a surface.
template<class Coord>
inline list<shared_ptr<Coord>> surface_vertices(const TMatrix &curve_matrix, const vector<vector<Coord>> controls)
{
    static_assert(is_convertible<TVector, Coord>::value, "Coord must have constructor: Coord(const TVector &)");

    list<shared_ptr<Coord>> coords;

    for (auto &c: controls)
    {
        assert(c.size() == surface_geometry_matrix_size);

        const TMatrix mx = surface_matrix(curve_matrix, c, 0);
        const TMatrix my = surface_matrix(curve_matrix, c, 1);
        const TMatrix mz = surface_matrix(curve_matrix, c, 2);

        generate_surface_mesh(coords, [mx, my, mz] (const TVector &vs, const TVector &vt)
        {
            return make_shared<Coord>(
                (vs * mx) * vt,
                (vs * my) * vt,
                (vs * mz) * vt
            );
        });

        // Not drawing lines between surface meshes.
        coords.push_back(nullptr);
    }

    return coords;
}
