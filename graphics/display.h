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

// Visible area of the world
class Window: public Object2D, public ClippingArea
{
public:

    Window(double left, double bottom, double right, double top):
         _leftBottom(left, bottom), _leftTop(left, top), _rightTop(right, top), _rightBottom(right, bottom),
         _center(equidistant(_leftBottom, _rightTop)),
         _viewport_top_left(0, 0),
         _up_angle(0),
         _viewport_width(1), _viewport_height(1),
         _from_world_matrix(from_world_matrix()),
         _to_world_matrix(to_world_matrix()),
         _from_viewport_matrix(from_viewport_matrix()),
         _to_viewport_matrix(to_viewport_matrix()) {}

    Coord2D leftBottom() const { return _leftBottom; }
    Coord2D leftTop() const { return _leftTop; }
    Coord2D rightTop() const { return _rightTop; }
    Coord2D rightBottom() const { return _rightBottom; }

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

    // Zoom out by factor
    void zoom_out(double factor)
    {
        scale(1.0 + factor, center());
    }

    // Zoom in by factor
    void zoom_in(double factor)
    {
        scale(1.0 - factor, center());
    }

    // Pan left by factor
    void pan_left(double factor)
    {
        double tx = width() * factor;

        translate(Coord2D(-tx, 0));
    }

    // Pan right by factor
    void pan_right(double factor)
    {
        double tx = width() * factor;

        translate(Coord2D(+tx, 0));
    }

    // Pan up by factor
    void pan_up(double factor)
    {
        double ty = height() * factor;

        translate(Coord2D(0, +ty));
    }

    // Pan down by factor
    void pan_down(double factor)
    {
        double ty = height() * factor;

        translate(Coord2D(0, -ty));
    }

    // Transform according to the matrix.
    void transform(TMatrix matrix) override
    {
        Object::transform(matrix);
        _center = equidistant(_leftBottom, _rightTop);
        adjust_aspect_ratio();
    }

    // Rotate on the x axis by degrees at center; clockwise if degrees positive; counter-clockwise if negative.
    void rotate_x(double degrees, Coord2D center) override
    {
        Object::rotate_x(-degrees, center);

        _up_angle += degrees;
        _center = equidistant(_leftBottom, _rightTop);
        adjust_aspect_ratio();
    }

    // Rotate on the y axis by degrees at center; counter-clockwise if degrees positive; clockwise if negative.
    void rotate_y(double degrees, Coord2D center) override
    {
        Object::rotate_y(-degrees, center);

        _up_angle += degrees;
        _center = equidistant(_leftBottom, _rightTop);
        adjust_aspect_ratio();
    }

    // Rotate on the z axis by degrees at center; clockwise if degrees positive; counter-clockwise if negative.
    void rotate_z(double degrees, Coord2D center) override
    {
        Object::rotate_z(-degrees, center);

        _up_angle += degrees;
        _center = equidistant(_leftBottom, _rightTop);
        adjust_aspect_ratio();
    }

    // Window's center
    Coord2D center() override
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

    // Draw a square in canvas.
    void draw(Canvas<Coord2D> &canvas) override
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

    list<Coord2D *> controls() override
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

            if (!equals(height() / width(), height_ratio))
            {
                double adjusted_height = width() * height_ratio;
                double adjusted_bottom = _center.y() - (adjusted_height / 2);
                double adjusted_top = _center.y() + (adjusted_height / 2);
                double adjusted_left = _center.x() + (width() / 2);
                double adjusted_right = _center.x() - (width() / 2);

                _leftTop = { adjusted_left, adjusted_top };
                _rightTop = { adjusted_right, adjusted_top };
                _leftBottom = { adjusted_left, adjusted_bottom };
                _rightBottom = { adjusted_right, adjusted_bottom };

                adjust_angle();
            }
        }
        else if (_viewport_height > _viewport_width)
        {
            double width_ratio = _viewport_width / _viewport_height;

            if (!equals(width() / height(), width_ratio))
            {
                double adjusted_width = height() * width_ratio;
                double adjusted_left = _center.x() - (adjusted_width / 2);
                double adjusted_right = _center.x() + (adjusted_width / 2);
                double adjusted_top = _center.y() + (height() / 2);
                double adjusted_bottom = _center.y() - (height() / 2);

                _leftBottom = { adjusted_left, adjusted_bottom };
                _leftTop = { adjusted_left, adjusted_top };
                _rightBottom = { adjusted_right, adjusted_bottom };
                _rightTop = { adjusted_right, adjusted_top };

                adjust_angle();
            }
        }

        _from_world_matrix = from_world_matrix();
        _to_world_matrix = to_world_matrix();
        _from_viewport_matrix = from_viewport_matrix();
        _to_viewport_matrix = to_viewport_matrix();
    }

    void adjust_angle()
    {
        _leftTop.rotate_z(-_up_angle, center());
        _rightTop.rotate_z(-_up_angle, center());
        _leftBottom.rotate_z(-_up_angle, center());
        _rightBottom.rotate_z(-_up_angle, center());
    }

private:

    Coord2D _leftBottom, _leftTop, _rightTop, _rightBottom;
    Coord2D _center;
    Coord2D _viewport_top_left;
    double _up_angle; // degrees
    double _viewport_width, _viewport_height;
    TMatrix _from_world_matrix, _to_world_matrix, _from_viewport_matrix, _to_viewport_matrix;

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
enum ProjectionMethod { PARALLEL, PERSPECTIVE };

static ProjectionMethod projection_method = PERSPECTIVE;

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

    static constexpr double d = 500;

    PerspectiveProjection(Canvas<Coord2D> &canvas, Coord3D center) :
        ProjectionCanvas(canvas),
        _projection(inverse_translation(center) * perspective_matrix() * translation(center))
    {
    }

    Coord2D project(Coord3D coord) const override
    {
        const TVector projected = coord * _projection;

        return Coord2D(projected.homogeneous());
    }

private:

    inline TMatrix perspective_matrix() const
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


template<class Coord>
class World
{
public:

    using DisplayFile = ::DisplayFile<Coord>;
    using Object = ::Object<Coord>;

    World(shared_ptr<Window> window, DisplayFile display_file)
        : _window(window), _display_file(display_file) {}

    shared_ptr<Window> window() { return _window; }

    DisplayFile & display_file() { return _display_file; }

    // Objects from command list
    vector<shared_ptr<Object>> objects() {
        vector<shared_ptr<Object>> vector;

#ifdef WORLD_2D
        vector.push_back(_window);
#endif

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
