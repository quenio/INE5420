#pragma once

#include "region.h"
#include "clipping_cs.h"
#include "clipping_lb.h"
#include "bezier_curve.h"
#include "spline_curve.h"
#include "graphics.h"

#define UNUSED __attribute__ ((unused))

// 2D coordinates
class Coord2D: public XYCoord<Coord2D>
{
public:

    Coord2D(double x, double y): XYCoord(x, y) {}

    Coord2D(const TVector &vector): XYCoord(vector) {}

    list<Coord2D *> controls() override
    {
        return { this };
    }

};

template<>
int Object<Coord2D>::_count = 0;

// Plane Projection Coordinates
class PPC: public XYCoord<PPC>
{
public:

    constexpr static double norm_left = -1;
    constexpr static double norm_right = +1;
    constexpr static double norm_bottom = -1;
    constexpr static double norm_top = +1;
    constexpr static double norm_width = 2;
    constexpr static double norm_height = 2;

    PPC(double x, double y): XYCoord(x, y) {}

    PPC(const TVector &vector): XYCoord(vector) {}

    list<PPC *> controls() override
    {
        return { this };
    }

};

enum class Visibility { FULL, PARTIAL, NONE };

// Area in world that may be clipped
class ClippingArea
{
public:

    // True if area contains World coord.
    virtual bool contains(Coord2D coord) const = 0;

    // Translate coord from World to Window, where left-bottom is (-1, -1) and right-top is (1, 1).
    virtual PPC world_to_window(Coord2D coord) const = 0;

    // Translate coord from Window to World.
    virtual Coord2D window_to_world(PPC coord) const = 0;

};

// Clippable objects
template<class Object>
class Clippable
{
public:

    // Provide clipped version of itself in area.
    virtual shared_ptr<Object> clipped_in(ClippingArea &area) = 0;

};

enum class ClippingMethod { COHEN_SUTHERLAND, LIANG_BARSKY, NONE };

static ClippingMethod clipping_method = ClippingMethod::COHEN_SUTHERLAND;

// Clip line between Window coord a and b.
inline pair<PPC, PPC> clip_line(const PPC &a, const PPC &b)
{
    switch (clipping_method)
    {
        case ClippingMethod::COHEN_SUTHERLAND: return clip_line_using_cs(a, b);
        case ClippingMethod::LIANG_BARSKY: return clip_line_using_lb(a, b);
        case ClippingMethod::NONE: return make_pair(a, b);
    }
}

// Clip line between World coord a and b into the area.
inline pair<Coord2D, Coord2D> clip_line(ClippingArea &area, const Coord2D &a, const Coord2D &b)
{
    const PPC window_a = area.world_to_window(a);
    const PPC window_b = area.world_to_window(b);

    const pair<PPC, PPC> clipped_line = clip_line(window_a, window_b);

    return make_pair(
        area.window_to_world(clipped_line.first),
        area.window_to_world(clipped_line.second)
    );
}

// Determine the visibility in area for line between a and b.
inline Visibility visibility(ClippingArea &area, const Coord2D &a, const Coord2D &b)
{
    static_assert(is_convertible<TVector, Coord2D>::value, "Coord2D must have constructor: Coord2D(const TVector &)");
    static_assert(is_convertible<Coord2D, TVector>::value, "Coord2D must have conversion operator: operator TVector() const");

    if (clipping_method == ClippingMethod::NONE) return Visibility::FULL;

    const bool a_in_area = area.contains(a);
    const bool b_in_area = area.contains(b);

    if (a_in_area && b_in_area)
    {
        return Visibility::FULL;
    }
    else if (a_in_area || b_in_area || area.contains(equidistant(a, b)))
    {
        return Visibility::PARTIAL;
    }
    else if (in_one_super_region(area.world_to_window(a), area.world_to_window(b)))
    {
        return Visibility::NONE;
    }
    else
    {
        const pair<Coord2D, Coord2D> clipped = clip_line(area, a, b);
        return area.contains(clipped.first) || area.contains(clipped.second) ? Visibility::PARTIAL : Visibility::NONE;
    }
}

// 2D drawable objects
class Drawable2D: public virtual Drawable<Coord2D>
{
public:

    // Determine the visibility in area.
    virtual Visibility visibility_in(ClippingArea UNUSED &area) const
    {
        return Visibility::FULL;
    }

};

// 2D objects
class Object2D: public Object<Coord2D>, public virtual Drawable2D
{
};

// Two-dimensional points
class Point: public Object2D
{
public:

    Point(Coord2D coord): _coord(coord) {}

    // Draw a point in canvas at position (x, y).
    void draw(Canvas<Coord2D> &canvas) override
    {
        canvas.draw_circle(_coord, 1.5);
    }

    // Type used in the name
    string type() const override
    {
        return "Point";
    }

    // Coord of the Point itself
    Coord2D center() override
    {
        return _coord;
    }

    // Determine the visibility in area.
    Visibility visibility_in(ClippingArea &area) const override
    {
        return area.contains(_coord) ? Visibility::FULL : Visibility::NONE;
    }

    list<Coord2D *> controls() override
    {
        return { &_coord };
    }

private:

    Coord2D _coord;

};

// Straight one-dimensional figure delimited by two points
class Line: public Object2D, public Clippable<Drawable2D>
{
public:

    Line(Coord2D a, Coord2D b): _a(a), _b(b) {}

    // Draw line in canvas.
    void draw(Canvas<Coord2D> &canvas) override
    {
        canvas.move(_a);
        canvas.draw_line(_b);
    }

    // Type used in the name
    string type() const override
    {
        return "Line";
    }

    // Midpoint between a and b
    Coord2D center() override
    {
        return equidistant(_a, _b);
    }

    // Determine the visibility in area.
    Visibility visibility_in(ClippingArea &area) const override
    {
        return visibility(area, _a, _b);
    }

    // Provide clipped version of itself in area.
    shared_ptr<Drawable2D> clipped_in(ClippingArea &area) override
    {
        const pair<Coord2D, Coord2D> clipped_line = clip_line(area, _a, _b);

        return make_shared<Line>(clipped_line.first, clipped_line.second);
    }

    list<Coord2D *> controls() override
    {
        return { &_a, &_b };
    }

private:

    Coord2D _a, _b;

};

// Sequence of lines drawn from the given vertices.
class Polyline2D: public virtual Drawable2D, public Polyline<Coord2D>, public Clippable<Drawable2D>
{
public:

    // New drawable from clipped_vertices
    virtual shared_ptr<Drawable2D> clipped_drawable(list<shared_ptr<Coord2D>> clipped_vertices) const = 0;

    // Determine the visibility in area.
    Visibility visibility_in(ClippingArea &area) const override
    {
        Visibility result = Visibility::NONE;

        shared_ptr<Coord2D> previous = initial_vertex();
        for (auto current: vertices())
        {
            if (previous != nullptr)
            {
                const Visibility v = visibility(area, *previous, *current);
                if (v == Visibility::PARTIAL)
                {
                    return Visibility::PARTIAL;
                }
                else
                {
                    result = v;
                }
            }

            previous = current;
        }

        return result;
    }

    // Provide clipped version of itself in area.
    shared_ptr<Drawable2D> clipped_in(ClippingArea &area) override
    {
        list<shared_ptr<Coord2D>> new_vertices;

        shared_ptr<Coord2D> previous = initial_vertex();
        for (auto current: vertices())
        {
            if (previous != nullptr)
            {
                switch (visibility(area, *previous, *current))
                {
                    case Visibility::FULL:
                    {
                        if (new_vertices.back() != previous)
                            new_vertices.push_back(previous);
                        if (new_vertices.back() != current)
                            new_vertices.push_back(current);
                    }
                        break;

                    case Visibility::PARTIAL:
                    {
                        const pair<Coord2D, Coord2D> clipped_line = clip_line(area, *previous, *current);

                        if (area.contains(clipped_line.first) && (new_vertices.back() == nullptr || *new_vertices.back() != clipped_line.first))
                        {
                            new_vertices.push_back(make_shared<Coord2D>(clipped_line.first));
                        }

                        if (area.contains(clipped_line.second) && (new_vertices.back() == nullptr || *new_vertices.back() != clipped_line.second))
                        {
                            new_vertices.push_back(make_shared<Coord2D>(clipped_line.second));
                        }
                    }
                    break;

                    case Visibility::NONE:
                    {
                        const PPC window_a = area.world_to_window(*previous);
                        const PPC window_b = area.world_to_window(*current);

                        if (region(window_a) != region(window_b))
                        {
                            // Determine closest corner
                            const double x = min(window_a.x(), window_b.x()) < PPC::norm_left ? PPC::norm_left : PPC::norm_right;
                            const double y = min(window_a.y(), window_b.y()) < PPC::norm_bottom ? PPC::norm_bottom : PPC::norm_top;
                            const Coord2D corner = area.window_to_world(PPC(x, y));

                            if (area.contains(corner) && (new_vertices.back() == nullptr || *new_vertices.back() != corner))
                                new_vertices.push_back(make_shared<Coord2D>(corner));
                        }
                    }
                }
            }

            previous = current;
        }

        return clipped_drawable(new_vertices);
    }

};

// Plane figure bound by a set of lines - the sides - meeting in a set of points - the vertices
class Polygon: public Object2D, public Polyline2D
{
public:

    Polygon(list<shared_ptr<Coord2D>> vertices): _vertices(vertices) {}

    Polygon(list<Coord2D> vertices)
    {
        for (auto &v: vertices)
            _vertices.push_back(make_shared<Coord2D>(v));
    }

    // Vertices to use when drawing the lines.
    list<shared_ptr<Coord2D>> vertices() const override
    {
        return _vertices;
    }

    // Initial vertex of the first line to be drawn.
    shared_ptr<Coord2D> initial_vertex() const override
    {
        return _vertices.back();
    }

    // Type used in the name
    string type() const override
    {
        return "Polygon";
    }

    // New drawable from clipped_vertices
    shared_ptr<Drawable2D> clipped_drawable(list<shared_ptr<Coord2D>> clipped_vertices) const override
    {
        return make_shared<Polygon>(clipped_vertices);
    }

    list<Coord2D *> controls() override
    {
        list<Coord2D *> vertices;

        for (auto v: _vertices)
            vertices.push_back(v.get());

        return vertices;
    }

private:

    list<shared_ptr<Coord2D>> _vertices;

};

class ClippedPolyline: public Polyline2D
{
public:

    ClippedPolyline(list<shared_ptr<Coord2D>> vertices): _vertices(vertices) {}

    // Vertices to use when drawing the lines.
    list<shared_ptr<Coord2D>> vertices() const override
    {
        return _vertices;
    }

    // New drawable from clipped_vertices
    shared_ptr<Drawable2D> clipped_drawable(list<shared_ptr<Coord2D>> clipped_vertices) const override
    {
        return make_shared<ClippedPolyline>(clipped_vertices);
    }

private:

    list<shared_ptr<Coord2D>> _vertices;

};

// Curve defined by two edge coords and two internal control points
class BezierCurve: public Object2D, public Polyline2D
{
public:

    BezierCurve(Coord2D edge1, Coord2D control1, Coord2D edge2, Coord2D control2)
        : _edge1(edge1), _control1(control1), _edge2(edge2), _control2(control2) {}

    // Type used in the name
    string type() const override
    {
        return "BezierCurve";
    }

    // Midpoint between both edges
    Coord2D center() override
    {
        return equidistant(_edge1, _edge2);
    }

    // Vertices to use when drawing the lines.
    list<shared_ptr<Coord2D>> vertices() const override
    {
        return bezier_curve_vertices(_edge1, _control1, _control2, _edge2);
    }

    // New drawable from clipped_vertices
    shared_ptr<Drawable2D> clipped_drawable(list<shared_ptr<Coord2D>> clipped_vertices) const override
    {
        return make_shared<ClippedPolyline>(clipped_vertices);
    }

    list<Coord2D *> controls() override
    {
        return { &_edge1, &_control1, &_edge2, &_control2 };
    }

private:

    Coord2D _edge1, _control1;
    Coord2D _edge2, _control2;

};

// B-Spline curve defined by a list of control coords.
class SplineCurve: public Object2D, public Polyline2D
{
public:

    SplineCurve(vector<Coord2D> controls): _controls(controls) {}

    // Type used in the name
    string type() const override
    {
        return "SplineCurve";
    }

    // Vertices to use when drawing the lines
    list<shared_ptr<Coord2D>> vertices() const override
    {
        return spline_curve_vertices(_controls);
    }

    // New drawable from clipped_vertices
    shared_ptr<Drawable2D> clipped_drawable(list<shared_ptr<Coord2D>> clipped_vertices) const override
    {
        return make_shared<ClippedPolyline>(clipped_vertices);
    }

    list<Coord2D *> controls() override
    {
        list<Coord2D *> result;
        for (auto coord = _controls.begin(); coord != _controls.end(); ++coord)
        {
            result.push_back(&(*coord));
        }
        return result;
    }

private:

    vector<Coord2D> _controls;

};



