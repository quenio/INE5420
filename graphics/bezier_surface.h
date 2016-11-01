#pragma once

#include "bezier.h"
#include "surfaces.h"

constexpr size_t control_group_size = 4;
constexpr size_t control_size = control_group_size * control_group_size;

// Create TMatrix with the coordinates of controls from 0 to 16, extracting the j-th position of each vector.
template<class Coord>
TMatrix geometry_matrix(const vector<Coord> controls, size_t j)
{
    assert(controls.size() == control_size);

    return TMatrix(
        vector_of<Coord>({ controls[0], controls[4], controls[8],  controls[12] }, j),
        vector_of<Coord>({ controls[1], controls[5], controls[9],  controls[13] }, j),
        vector_of<Coord>({ controls[2], controls[6], controls[10], controls[14] }, j),
        vector_of<Coord>({ controls[3], controls[7], controls[11], controls[15] }, j)
    );
}

// Create TMatrix for the Bezier surface, extracting the j-th position of each vector.
template<class Coord>
TMatrix bezier_surface_matrix(const vector<Coord> controls, size_t j)
{
    return bezier_matrix() * geometry_matrix(controls, j) * bezier_matrix();
}

// Generate the vertices to represent a Bezier surface.
template<class Coord>
inline list<Coord> bezier_surface_vertices(const vector<Coord> controls)
{
    static_assert(is_convertible<TVector, Coord>::value, "Coord must have constructor: Coord(const TVector &)");

    const TMatrix mx = bezier_surface_matrix(controls, 0);
    const TMatrix my = bezier_surface_matrix(controls, 1);
    const TMatrix mz = bezier_surface_matrix(controls, 2);

    list<Coord> coords;

    generate_surface_mesh(coords, [mx, my, mz] (const TVector &vs, const TVector &vt)
    {
        return Coord(
            (vs * mx) * vt,
            (vs * my) * vt,
            (vs * mz) * vt
        );
    });

    return coords;
}

