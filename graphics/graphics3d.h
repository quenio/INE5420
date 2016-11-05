#pragma once

#include "surfaces.h"
#include "fd_surfaces.h"
#include "graphics.h"

// 3D coordinates
class Coord3D: public XYZCoord<Coord3D>
{
public:

    Coord3D(double x, double y, double z): XYZCoord(x, y, z) {}

    Coord3D(const TVector &vector): XYZCoord(vector) {}

    list<Coord3D *> controls() override
    {
        return { this };
    }

};

template<>
int Object<Coord3D>::_count = 0;

// 3D line segment
class Segment3D: public Transformable<Coord3D>
{
public:

    Segment3D(Coord3D a, Coord3D b): _a(a), _b(b) {}

    // Midpoint between a and b
    Coord3D center() override
    {
        return equidistant(_a, _b);
    }

    list<Coord3D *> controls() override
    {
        return { &_a, &_b };
    }

    // Draw line in canvas.
    void draw(Canvas<Coord3D> &canvas, Color color)
    {
        canvas.move(_a);
        canvas.draw_line(_b, color);
    }

private:

    Coord3D _a, _b;

};

// 3D objects
class Object3D: public Object<Coord3D>
{
public:

    Object3D(initializer_list<Segment3D> segments): _segments(segments) {}

    // Type used in the name
    string type() const override
    {
        return "Object3D";
    }

    // Draw the sequence of segments in canvas.
    void draw(Canvas<Coord3D> &canvas) override
    {
        for (auto &s: _segments)
        {
            s.draw(canvas, color());
        }
    }

    list<Coord3D *> controls() override
    {
        list<Coord3D *> controls;

        for (auto &s: _segments)
            for (auto c: s.controls())
                controls.push_back(c);

        return controls;
    }

private:

    list<Segment3D> _segments;

};

// Surface defined by some type of curve
class Surface: public Object<Coord3D>, public Polyline<Coord3D>
{
public:

    Surface(vector<vector<Coord3D>> controls): _controls(controls)
    {
        for (auto &c: controls)
            assert(c.size() >= surface_geometry_matrix_size);
    }

    virtual TMatrix & curve() const = 0;

    // Vertices to use when drawing the lines.
    list<shared_ptr<Coord3D>> vertices() const override
    {
        return fd_surface_vertices(curve(), _controls);
    }

    // Control coords
    list<Coord3D *> controls() override
    {
        list<Coord3D *> vertices;

        for (auto &c: _controls)
            for (auto &v: c)
                vertices.push_back(&v);

        return vertices;
    }

private:

    vector<vector<Coord3D>> _controls;

};

// Surface defined by Bezier curves
class BezierSurface: public Surface
{
public:

    BezierSurface(vector<vector<Coord3D>> controls): Surface(controls) {}

    // Type used in the name
    string type() const override
    {
        return "BezierSurface";
    }

    TMatrix & curve() const override
    {
        return bezier;
    }

};

// Surface defined by Spline curves
class SplineSurface: public Surface
{
public:

    SplineSurface(vector<vector<Coord3D>> controls): Surface(controls) {}

    // Type used in the name
    string type() const override
    {
        return "SplineSurface";
    }

    TMatrix & curve() const override
    {
        return spline;
    }

};

