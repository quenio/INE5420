#pragma once

#include "bezier_surface.h"
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

// Surface defined by bezier curves
class BezierSurface: public Object<Coord3D>, public Polyline<Coord3D>
{
public:

    BezierSurface(Coord3D edge1, Coord3D control1, Coord3D edge2, Coord3D control2)
        : _edge1(edge1), _control1(control1), _edge2(edge2), _control2(control2) {}

    // Type used in the name
    string type() const override
    {
        return "Bezier";
    }

    // Midpoint between both edges
    Coord3D center() override
    {
        return equidistant(_edge1, _edge2);
    }

    // Vertices to use when drawing the lines.
    list<Coord3D> vertices() const override
    {
        return bezier_surface_vertices(_edge1, _control1, _control2, _edge2);
    }

    list<Coord3D *> controls() override
    {
        return { &_edge1, &_control1, &_edge2, &_control2 };
    }

private:

    Coord3D _edge1, _control1;
    Coord3D _edge2, _control2;

};


