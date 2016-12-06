#pragma once

#include "graphics3d.h"
#include "graphics2d.h"
#include "timer.h"

// Viewport Coordinates
class VC: public XYCoord<VC>
{
public:

    VC(double x, double y): XYCoord(x, y) {}

    VC(const TVector &vector): XYCoord(vector) {}

    list<VC *> controls() override
    {
        return { this };
    }

};

// Visible area on a canvas
class Viewport
{
public:

    constexpr static double margin_percentage = 0.025;

    Viewport(double width, double height): _width(width), _height(height), _margin(_width * margin_percentage) {}

    Coord2D topLeft() const { return Coord2D(left(), top()); }
    double top() const { return _margin; }
    double left() const { return _margin; }

    double width() const { return _width; }
    double height() const { return _height; }

    double content_width() const { return width() - 2 * margin(); }
    double content_height() const { return height() - 2 * margin(); }

    double margin() const { return _margin; }

private:

    double _width, _height, _margin;

};

enum ProjectionView { FRONT, BACK };

// Visible area of the world
template<class Coord>
class Window: public Object<Coord>, public virtual Drawable<Coord>, public ClippingArea
{
public:

    using Object = ::Object<Coord>;
    using Canvas = ::Canvas<Coord>;

    Window(Coord center, double width, double height):
        _center(center),
        _viewport_top_left(0, 0),
        _up_angle(0),
#ifdef WORLD_2D
        _viewport_width(1), _viewport_height(1)
#else
        _viewport_width(1), _viewport_height(1),
        _projection_distance(abs(_center.z()))
#endif
    {
        adjust_bounds(width, height);
    }

    Coord leftBottom() const { return _leftBottom; }
    Coord leftTop() const { return _leftTop; }
    Coord rightTop() const { return _rightTop; }
    Coord rightBottom() const { return _rightBottom; }

    double width() const { return distance(leftBottom(), rightBottom()); }
    double height() const { return distance(leftBottom(), leftTop()); }

    Coord2D window_ratios() const { return Coord2D(PPC::norm_width / width(), PPC::norm_height / height()); }
    Coord2D world_ratios() const { return Coord2D(width() / PPC::norm_width, height() / PPC::norm_height); }

    Coord2D window_ratios_for_viewport() const
    {
        return Coord2D(PPC::norm_width / _viewport_width, PPC::norm_height / _viewport_height);
    }

    Coord2D viewport_ratios() const
    {
        return Coord2D(_viewport_width / PPC::norm_width, _viewport_height / PPC::norm_height);
    }

    // True if Window contains World coord.
    bool contains(Coord2D coord) const override
    {
        PPC wc = from_world(coord);
        double x = wc.x(), y = wc.y();
        return ((x > PPC::norm_left && x < PPC::norm_right) || equals(x, PPC::norm_left) || equals(x, PPC::norm_right)) &&
               ((y > PPC::norm_bottom && y < PPC::norm_top) || equals(y, PPC::norm_bottom) || equals(y, PPC::norm_top));
    }

    // Translate coord from World to Window, where left-bottom is (-1, -1) and right-top is (1, 1).
    PPC world_to_window(Coord2D coord) const override
    {
        return from_world(coord);
    }

    // Translate coord from Window to World.
    Coord2D window_to_world(PPC coord) const override
    {
        return to_world(coord);
    }

    // Translate coord from World to Window, where left-bottom is (-1, -1) and right-top is (1, 1).
    PPC from_world(Coord2D coord) const
    {
        return coord * _from_world_matrix;
    }

    TMatrix from_world_matrix() const
    {
        return inverse_translation(_center) * z_rotation(_up_angle) * scaling(window_ratios());
    }

    // Translate coord from Window to World.
    Coord2D to_world(PPC coord) const
    {
        return coord * _to_world_matrix;
    }

    TMatrix to_world_matrix() const
    {
        return scaling(world_ratios()) * z_rotation(-_up_angle) * translation(_center);
    }

    // Translate coord from Viewport to Window.
    PPC from_viewport(VC coord, double viewport_height) const
    {
        return Coord2D(coord.x(), viewport_height - coord.y()) * _from_viewport_matrix;
    }

    TMatrix from_viewport_matrix() const
    {
        return inverse_translation(_viewport_top_left) *
               scaling(window_ratios_for_viewport()) *
               translation(Coord2D(PPC::norm_left, PPC::norm_bottom));
    }

    // Translate coord from Window to Viewport, leaving a margin.
    VC to_viewport(PPC coord) const
    {
        return Coord2D(coord.x() - PPC::norm_left, PPC::norm_height - (coord.y() - PPC::norm_bottom)) * _to_viewport_matrix;
    }

    TMatrix to_viewport_matrix() const
    {
        return scaling(viewport_ratios()) * translation(_viewport_top_left);
    }

    // Translate coord from world to viewport
    VC world_to_viewport(const Coord2D &coord) const
    {
        return to_viewport(from_world(coord));
    }

    // Zoom in by factor
    void zoom_in(double factor)
    {
        Object::scale(1.0 - factor, center());
    }

    // Zoom out by factor
    void zoom_out(double factor)
    {
        Object::scale(1.0 + factor, center());
    }

    // Pan left by factor
    void pan_left(double factor)
    {
        double tx = width() * factor;

        transform(::translation(TVector({ -tx, 0, 0, 1 })));
    }

    // Pan right by factor
    void pan_right(double factor)
    {
        double tx = width() * factor;

        transform(::translation(TVector({ +tx, 0, 0, 1 })));
    }

    // Pan up by factor
    void pan_up(double factor)
    {
        double ty = height() * factor;

        transform(::translation(TVector({ 0, +ty, 0, 1 })));
    }

    // Pan down by factor
    void pan_down(double factor)
    {
        double ty = height() * factor;

        transform(::translation(TVector({ 0, -ty, 0, 1 })));
    }

    // Transform according to the matrix.
    void transform(TMatrix matrix) override
    {
        Object::transform(matrix);
        _center = equidistant(_leftBottom, _rightTop);
        adjust_aspect_ratio();
    }

    // Rotate on the x axis by degrees at center; clockwise if degrees positive; counter-clockwise if negative.
    void rotate_x(double degrees, Coord center) override
    {
        Object::rotate_x(-degrees, center);

#ifdef WORLD_2D
        _up_angle += degrees;
#endif

        _center = equidistant(_leftBottom, _rightTop);
        adjust_aspect_ratio();
    }

    // Rotate on the y axis by degrees at center; counter-clockwise if degrees positive; clockwise if negative.
    void rotate_y(double degrees, Coord center) override
    {
        Object::rotate_y(-degrees, center);

        _center = equidistant(_leftBottom, _rightTop);
        adjust_aspect_ratio();
    }

    // Rotate on the z axis by degrees at center; clockwise if degrees positive; counter-clockwise if negative.
    void rotate_z(double degrees, Coord center) override
    {
        Object::rotate_z(-degrees, center);

#ifdef WORLD_3D
        if (_projection_view == BACK) degrees = -degrees;
        _up_angle += degrees;
#endif

        _center = equidistant(_leftBottom, _rightTop);
        adjust_aspect_ratio();
    }

    // Window's center
    Coord center() override
    {
        return _center;
    }

    // Type used in the name
    string type() const override
    {
        return "Window";
    }

    // Name displayed on the UI
    string name() const override
    {
        stringstream ss;
        ss << type();
        return ss.str();
    }

#ifdef WORLD_3D

    ProjectionView projection_view()
    {
        return _projection_view;
    }

    double projection_distance()
    {
        return _projection_distance;
    }

    double projection_rotation()
    {
        return _projection_rotation;
    }

    void front_projection()
    {
        _projection_view = FRONT;
        _projection_distance = abs(_projection_distance);
        _projection_rotation = 0;
        _up_angle = 0;
        _center = Coord3D(_center.x(), _center.y(), -_projection_distance);
        adjust_aspect_ratio();
    }

    void back_projection()
    {
        _projection_view = BACK;
        _projection_distance = -abs(_projection_distance);
        _projection_rotation = 180;
        _up_angle = 0;
        _center = Coord3D(_center.x(), _center.y(), -_projection_distance);
        adjust_aspect_ratio();
    }

#endif

    // Draw a square in canvas.
    void draw(Canvas &canvas) override
    {
        canvas.set_color(BLUE);

        canvas.move(leftBottom());
        canvas.draw_line(leftTop());

        canvas.move(leftTop());
        canvas.draw_line(rightTop());

        canvas.move(rightTop());
        canvas.draw_line(rightBottom());

        canvas.move(rightBottom());
        canvas.draw_line(leftBottom());
    }

    list<Coord *> controls() override
    {
        return { &_leftBottom, &_leftTop, &_rightTop, &_rightBottom };
    }

    void set_viewport(const Viewport &viewport)
    {
        _viewport_top_left = viewport.topLeft();
        _viewport_width = viewport.content_width();
        _viewport_height = viewport.content_height();

        adjust_aspect_ratio();
    }

    void adjust_aspect_ratio()
    {
        if (_viewport_width > _viewport_height)
        {
            double height_ratio = _viewport_height / _viewport_width;

            if (equals(height() / width(), height_ratio))
            {
                adjust_bounds(width(), height());
            }
            else
            {
                adjust_bounds(width(), width() * height_ratio);
            }
        }
        else if (_viewport_height > _viewport_width)
        {
            double width_ratio = _viewport_width / _viewport_height;

            if (equals(width() / height(), width_ratio))
            {
                adjust_bounds(width(), height());
            }
            else
            {
                adjust_bounds(height() * width_ratio, height());
            }
        }
        else
        {
            adjust_bounds(width(), height());
        }

        refresh_matrices();
    }

    void adjust_bounds(double width, double height)
    {
        const double dx = width / 2;
        const double dy = height / 2;

        _leftTop = xy_translated(_center, -dx, +dy);
        _rightTop = xy_translated(_center, +dx, +dy);
        _leftBottom = xy_translated(_center, -dx, -dy);
        _rightBottom = xy_translated(_center, +dx, -dy);

        adjust_angle();
    }

    void adjust_angle()
    {
        if (equals(_up_angle, 0)) return;

#ifdef WORLD_3D
        double degrees = _up_angle;

        if (_projection_view == FRONT) degrees = -degrees;
#else
        double degrees = -_up_angle;
#endif

        _leftTop.rotate_z(degrees, center());
        _rightTop.rotate_z(degrees, center());
        _leftBottom.rotate_z(degrees, center());
        _rightBottom.rotate_z(degrees, center());
    }

    void refresh_matrices()
    {
        _from_world_matrix = from_world_matrix();
        _to_world_matrix = to_world_matrix();
        _from_viewport_matrix = from_viewport_matrix();
        _to_viewport_matrix = to_viewport_matrix();
    }

private:

    Coord _leftBottom, _leftTop, _rightTop, _rightBottom;
    Coord _center;
    Coord2D _viewport_top_left;
    double _up_angle; // degrees
    double _viewport_width, _viewport_height;
    TMatrix _from_world_matrix, _to_world_matrix, _from_viewport_matrix, _to_viewport_matrix;

#ifdef WORLD_3D

    ProjectionView _projection_view;
    double _projection_distance;
    double _projection_rotation;

#endif

};

// Command to draw 2D objects
class Draw2DCommand: public DisplayCommand<Coord2D>
{
public:

    Draw2DCommand(shared_ptr<Drawable2D> drawable): _drawable(drawable) {}

    // Render drawable on canvas if visible.
    void render(Canvas<Coord2D> &canvas) override
    {
        ClippingArea *clipping_area = dynamic_cast<ClippingArea *>(&canvas);

        if (clipping_area == nullptr)
        {
            _drawable->draw(canvas);
        }
        else
        {
            draw_clipped(canvas, *clipping_area);
        }
    }

    void draw_clipped(Canvas<Coord2D> &canvas, ClippingArea &clipping_area) const
    {
        switch (_drawable->visibility_in(clipping_area))
        {
            case Visibility::FULL:
            {
                _drawable->draw(canvas);
            }
            break;

            case Visibility::PARTIAL:
            {
                shared_ptr<Clippable<Drawable2D>> clippable = dynamic_pointer_cast<Clippable<Drawable2D>>(_drawable);
                if (clippable == nullptr)
                    _drawable->draw(canvas);
                else
                {
                    shared_ptr<Drawable2D> clipped = clippable->clipped_in(clipping_area);
                    if (clipped->visibility_in(clipping_area) == Visibility::FULL)
                    {
                        clipped->draw(canvas);
                    }
                }
            }
            break;

            case Visibility::NONE:;
                // Nothing to draw.
        }
    }

    shared_ptr<Object> object() const override
    {
        return dynamic_pointer_cast<Object2D>(_drawable);
    }

private:

    shared_ptr<Drawable2D> _drawable;

};

// Command to draw 3D objects
class Draw3DCommand: public DisplayCommand<Coord3D>
{
public:

    using Drawable = ::Drawable<Coord3D>;
    using Canvas = ::Canvas<Coord3D>;
    using Object = ::Object<Coord3D>;

    Draw3DCommand(shared_ptr<Drawable> drawable): _drawable(drawable) {}

    // Render drawable on canvas if visible.
    void render(Canvas &canvas) override
    {
        printf("Draw3DCommand: %s: started\n", object()->name().c_str());
        const clock_t start = clock();
        _drawable->draw(canvas);
        const double time = elapsed_secs(start);
        printf("Draw3DCommand: %s: finished (t = %9.6lf)\n", object()->name().c_str(), time);
    }

    shared_ptr<Object> object() const override
    {
        return dynamic_pointer_cast<Object>(_drawable);
    }

private:

    shared_ptr<Drawable> _drawable;

};

// Projections
enum ProjectionMethod { ORTHOGONAL, PERSPECTIVE };

static ProjectionMethod projection_method = ORTHOGONAL;

#ifdef WORLD_3D

template<class Coord>
class ProjectionCanvas: public Canvas<Coord>
{
public:

    ProjectionCanvas(Canvas<Coord2D> &canvas): _canvas(canvas), _current(Coord2D(0, 0)) {}

    // Move from current position to destination.
    void move(const Coord &destination) override
    {
        _current = project(destination);
        _canvas.move(_current);
    }

    // Draw line from current position to destination, clipping if necessary.
    void draw_line(const Coord &destination) override
    {
        ClippingArea *clipping_area = dynamic_cast<ClippingArea *>(&_canvas);
        Coord2D projected_destination = project(destination);

        if (clipping_area == nullptr)
        {
            _canvas.draw_line(projected_destination);
        }
        else
        {
            switch (visibility(*clipping_area, _current, projected_destination))
            {
                case Visibility::FULL:
                {
                    _canvas.draw_line(projected_destination);
                }
                break;

                case Visibility::PARTIAL:
                {
                    const pair<Coord2D, Coord2D> clipped_line = clip_line(*clipping_area, _current, projected_destination);

                    if (visibility(*clipping_area, clipped_line.first, clipped_line.second) == Visibility::FULL)
                    {
                        _canvas.move(clipped_line.first);
                        _canvas.draw_line(clipped_line.second);
                    }
                }
                break;

                case Visibility::NONE:;
                    // Nothing to draw.
            }
        }
    }

    // Draw circle with the specified center, radius and color.
    void draw_circle(const Coord &center, const double radius) override
    {
        _canvas.draw_circle(project(center), radius);
    }

    // Set the color to be used when drawing.
    void set_color(const Color &color) override
    {
        _canvas.set_color(color);
    }

    virtual Coord2D project(Coord coord) const = 0;

protected:

    Canvas<Coord2D> &_canvas;
    Coord2D _current;

};

class ParallelProjection: public ProjectionCanvas<Coord3D>
{
public:

    ParallelProjection(Canvas<Coord2D> &canvas) : ProjectionCanvas(canvas) {}

    Coord2D project(Coord3D coord) const override
    {
        return TVector(coord);
    }

};

class PerspectiveProjection: public ProjectionCanvas<Coord3D>
{
public:

    PerspectiveProjection(Canvas<Coord2D> &canvas, Window<Coord3D> &window) :
        ProjectionCanvas(canvas),
        _projection(
            inverse_translation(window.center()) *
            perspective_matrix(window.projection_distance()) *
            y_rotation(window.projection_rotation()) *
            translation(window.center()))
    {
    }

    Coord2D project(Coord3D coord) const override
    {
        const Coord3D projected = (coord * _projection).homogeneous();

        return Coord2D(projected.x(), projected.y());
    }

private:

    inline TMatrix perspective_matrix(double d) const
    {
        return TMatrix(
            { 1.0, 0.0,   0.0, 0.0 },
            { 0.0, 1.0,   0.0, 0.0 },
            { 0.0, 0.0,   1.0, 0.0 },
            { 0.0, 0.0, 1.0/d, 1.0 }
        );
    }

    TMatrix _projection;

};

#endif


template<class Coord>
class World
{
public:

    using DisplayFile = ::DisplayFile<Coord>;
    using Object = ::Object<Coord>;
    using Window = ::Window<Coord>;

    World(shared_ptr<Window> window, DisplayFile display_file)
        : _window(window), _display_file(display_file) {}

    shared_ptr<Window> window() { return _window; }

    DisplayFile & display_file() { return _display_file; }

    // Objects from command list
    vector<shared_ptr<Object>> objects()
    {
        vector<shared_ptr<Object>> vector;

        for (auto &command: _display_file.commands())
        {
            if (command->object())
            {
                vector.push_back(command->object());
            }
        }

        return vector;
    }

    // Removes all objects from this world.
    void clear_display_file()
    {
        _display_file.clear_display_file();
    }

    void add_object(shared_ptr<::DisplayCommand<Coord>> object)
    {
        _display_file.add_command(object);
    }

private:

    shared_ptr<Window> _window;
    DisplayFile _display_file;

};

inline shared_ptr<Draw2DCommand> draw_point(Coord2D a)
{
    return make_shared<Draw2DCommand>(make_shared<Point>(a));
}

inline shared_ptr<Draw2DCommand> draw_line(Coord2D a, Coord2D b)
{
    return make_shared<Draw2DCommand>(make_shared<Line>(a, b));
}

inline shared_ptr<Draw2DCommand> draw_square(Coord2D a, Coord2D b, Coord2D c, Coord2D d)
{
    return make_shared<Draw2DCommand>(make_shared<Polygon>(Polygon({ a, b, c, d })));
}

inline shared_ptr<Draw2DCommand> draw_bezier_curve(Coord2D edge1, Coord2D control1, Coord2D edge2, Coord2D control2)
{
    return make_shared<Draw2DCommand>(make_shared<BezierCurve>(BezierCurve(edge1, control1, edge2, control2)));
}

inline shared_ptr<Draw2DCommand> draw_spline_curve(initializer_list<Coord2D> controls)
{
    return make_shared<Draw2DCommand>(make_shared<SplineCurve>(SplineCurve(controls)));
}

inline Segment3D x_segment(Coord3D start, double length)
{
    return Segment3D(start, start * translation(length, 0, 0));
}

inline Segment3D y_segment(Coord3D start, double length)
{
    return Segment3D(start, start * translation(0, length, 0));
}

inline Segment3D z_segment(Coord3D start, double length)
{
    return Segment3D(start, start * translation(0, 0, length));
}

inline shared_ptr<Draw3DCommand> draw_cube(Coord3D base, double length)
{
    Coord3D base1 = base;
    Coord3D base2 = base * translation(length, length, length);
    Coord3D base3 = base * translation(0.0, length, 0.0);
    Coord3D base4 = base * translation(0.0, 0.0, length);
    Coord3D base5 = base * translation(length, 0.0, length);
    Coord3D base6 = base * translation(length, length, 0.0);

    Object3D cube({
        x_segment(base1, length),
        y_segment(base1, length),
        z_segment(base1, length),
        x_segment(base2, -length),
        y_segment(base2, -length),
        z_segment(base2, -length),
        x_segment(base3, length),
        y_segment(base3, -length),
        z_segment(base3, length),
        x_segment(base4, length),
        y_segment(base4, length),
        z_segment(base4, -length),
        x_segment(base5, -length),
        y_segment(base5, length),
        z_segment(base5, -length),
        x_segment(base6, -length),
        y_segment(base6, -length),
        z_segment(base6, length)
    });

//    cube.transform(x_rotation(30) * y_rotation(-30) * z_rotation(30));

    return make_shared<Draw3DCommand>(make_shared<Object3D>(cube));
}

inline shared_ptr<Draw3DCommand> draw_bezier_surface(vector<vector<Coord3D>> controls)
{
    BezierSurface surface(controls);

    surface.transform(y_rotation(60) * translation(0, -10, 20) * x_rotation(10));

    return make_shared<Draw3DCommand>(make_shared<BezierSurface>(surface));
}

inline shared_ptr<Draw3DCommand> draw_spline_surface(vector<vector<Coord3D>> controls)
{
    SplineSurface surface(controls);

    surface.transform(scaling(3, 3, 3) * y_rotation(-60) * translation(+40, -60, -40));

    return make_shared<Draw3DCommand>(make_shared<SplineSurface>(surface));
}
