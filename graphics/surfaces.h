#pragma once

#include "transforms.h"

// Generate the vertices to represent a surface.
template<class Coord, typename CoordMaker>
inline void generate_surface_vertices(list<Coord> &coords, CoordMaker make_coord, bool transversal = false)
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
inline void generate_surface_mesh(list<Coord> &coords, CoordMaker make_coord)
{
    generate_surface_vertices(coords, make_coord);
    generate_transversal_surface_vertices(coords, make_coord);
};
