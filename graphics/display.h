#pragma once

#include "graphics2d.h"
#include "graphics3d.h"

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

    constexpr static double norm_left = -1;
    constexpr static double norm_bottom = -1;
    constexpr static double norm_width = 2;
    constexpr static double norm_height = 2;

    Window(double left, double bottom, double right, double top)
        :Object2D(BLUE),
         _leftBottom(left, bottom), _leftTop(left, top), _rightTop(right, top), _rightBottom(right, bottom),
         _center(equidistant(_leftBottom, _rightTop)),
         _up_angle(0) {}

    Coord2D leftBottom() const { return _leftBottom; }
    Coord2D leftTop() const { return _leftTop; }
    Coord2D rightTop() const { return _rightTop; }
    Coord2D rightBottom() const { return _rightBottom; }

    double width() const { return distance(leftBottom(), rightBottom()); }
    double height() const { return distance(leftBottom(), leftTop()); }

    Coord2D window_ratios() const { return Coord2D(norm_width / width(), norm_height / height()); }
    Coord2D world_ratios() const { return Coord2D(width() / norm_width, height() / norm_height); }

    Coord2D window_ratios(const Viewport &viewport) const
    {
        return Coord2D(norm_width / viewport.content_width(), norm_height / viewport.content_height());
    }

    Coord2D viewport_ratios(const Viewport &viewport) const
    {
        return Coord2D(viewport.content_width() / norm_width, viewport.content_height() / norm_height);
    }

    // True if Window contains World coord.
    bool contains(Coord2D coord) const override
    {
        PPC wc = from_world(coord);
        double x = wc.x(), y = wc.y();
        return ((x > -1 && x < +1) || equals(x, -1) || equals(x, +1)) &&
               ((y > -1 && y < +1) || equals(y, -1) || equals(y, +1));
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
        return coord * inverse_translation(_center) * z_rotation(_up_angle) * scaling(window_ratios());
    }

    // Translate coord from Window to World.
    Coord2D to_world(PPC coord) const
    {
        return coord * z_rotation(-_up_angle) * scaling(world_ratios()) * translation(_center);
    }

    // Translate coord from Viewport to Window.
    PPC from_viewport(VC coord, const Viewport &viewport) const
    {
        return Coord2D(coord.x(), viewport.height() - coord.y()) *
               inverse_translation(viewport.topLeft()) *
               scaling(window_ratios(viewport)) *
               translation(Coord2D(norm_left, norm_bottom));
    }

    // Translate coord from Window to Viewport, leaving a margin.
    VC to_viewport(PPC coord, const Viewport &viewport) const
    {
        return Coord2D(coord.x() - norm_left, norm_height - (coord.y() - norm_bottom)) *
               scaling(viewport_ratios(viewport)) *
               translation(viewport.topLeft());
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
    }

    // Translate by delta.
    void translate(Coord2D delta) override
    {
        Object::translate(delta);

        _center = equidistant(_leftBottom, _rightTop);
    }

    // Scale by factor from center.
    void scale(double factor, Coord2D center) override
    {
        Object::scale(factor, center);

        _center = equidistant(_leftBottom, _rightTop);
    }

    // Rotate by degrees at center; clockwise if degrees positive; counter-clockwise if negative.
    void rotate(double degrees, Coord2D center) override
    {
        Object::rotate(-degrees, center);

        _up_angle += degrees;
        _center = equidistant(_leftBottom, _rightTop);
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
        canvas.move(leftBottom());
        canvas.draw_line(leftTop(), color());

        canvas.move(leftTop());
        canvas.draw_line(rightTop(), color());

        canvas.move(rightTop());
        canvas.draw_line(rightBottom(), color());

        canvas.move(rightBottom());
        canvas.draw_line(leftBottom(), color());
    }

    list<Coord2D *> controls() override
    {
        return { &_leftBottom, &_leftTop, &_rightTop, &_rightBottom };
    }

private:

    Coord2D _leftBottom, _leftTop, _rightTop, _rightBottom, _center;
    double _up_angle; // degrees

};

// Area on a screen to execute display commands
class ViewportCanvas: public Canvas<Coord2D>, public Viewport, public ClippingArea
{
public:

    ViewportCanvas(double width, double height, shared_ptr<Window> window, Canvas<VC> &canvas)
        : Viewport(width, height), _window(window), _canvas(canvas) {}

    // True if area contains world coord.
    bool contains(Coord2D coord) const override
    {
        return _window->contains(coord);
    }

    // Translate coord from World to Window, where left-bottom is (-1, -1) and right-top is (1, 1).
    PPC world_to_window(Coord2D coord) const override
    {
        return _window->world_to_window(coord);
    }

    // Translate coord from Window to World.
    Coord2D window_to_world(PPC coord) const override
    {
        return _window->window_to_world(coord);
    }

    // Translate coord from world to viewport
    VC world_to_viewport(const Coord2D &coord) const
    {
        return _window->to_viewport(_window->from_world(coord), *this);
    }

    // Move to destination.
    void move(const Coord2D &destination) override
    {
        _canvas.move(world_to_viewport(destination));
    }

    // Draw line from current position to destination.
    void draw_line(const Coord2D &destination, const Color &color) override
    {
        _canvas.draw_line(world_to_viewport(destination), color);
    }

    // Draw circle with the specified center, radius and color.
    void draw_circle(const Coord2D &center, const double radius, const Color &color) override
    {
        _canvas.draw_circle(world_to_viewport(center), radius, color);
    }

private:

    shared_ptr<Window> _window;
    Canvas<VC> &_canvas;

};

// Command to draw 2D objects
class Draw2DCommand: public DisplayCommand<Coord2D>
{
public:

    Draw2DCommand(shared_ptr<Drawable2D> drawable): _drawable(drawable) {}

    // Render drawable on canvas if visible.
    void render(Canvas<Coord2D> &canvas) override
    {
        ViewportCanvas *viewportCanvas = dynamic_cast<ViewportCanvas *>(&canvas);

        if (viewportCanvas == nullptr)
        {
            _drawable->draw(canvas);
        }
        else
        {
            render_on_viewport(*viewportCanvas);
        }
    }

    void render_on_viewport(ViewportCanvas &viewportCanvas) const
    {
        switch (_drawable->visibility_in(viewportCanvas))
        {
            case Visibility::FULL:
            {
                _drawable->draw(viewportCanvas);
            }
            break;

            case Visibility::PARTIAL:
            {
                shared_ptr<Clippable<Drawable2D>> clippable = dynamic_pointer_cast<Clippable<Drawable2D>>(_drawable);
                if (clippable == nullptr)
                    _drawable->draw(viewportCanvas);
                else
                {
                    shared_ptr<Drawable2D> clipped = clippable->clipped_in(viewportCanvas);
                    if (clipped->visibility_in(viewportCanvas) == Visibility::FULL)
                    {
                        clipped->draw(viewportCanvas);
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

    Draw3DCommand(shared_ptr<Drawable> drawable): _drawable(drawable) {}

    // Render drawable on canvas if visible.
    void render(Canvas &canvas) override
    {
        _drawable->draw(canvas);
    }


    shared_ptr<Object> object() const override
    {
        return dynamic_pointer_cast<Object3D>(_drawable);
    }

private:

    shared_ptr<Drawable> _drawable;

};

// Projections
enum class ProjectionMethod { PARALLEL, PERSPECTIVE };

static ProjectionMethod projection_method = ProjectionMethod::PARALLEL;

class ProjectionCanvas: public Canvas<Coord3D>
{
public:

    ProjectionCanvas(Canvas<Coord2D> &canvas): _canvas(canvas) {}

    // Move to destination.
    void move(const Coord3D &destination) override
    {
        _canvas.move(project(destination));
    }

    // Draw line from current position to destination.
    void draw_line(const Coord3D &destination, const Color &color) override
    {
        _canvas.draw_line(project(destination), color);
    }

    // Draw circle with the specified center, radius and color.
    void draw_circle(const Coord3D &center, const double radius, const Color &color) override
    {
        _canvas.draw_circle(project(center), radius, color);
    }

    virtual Coord2D project(Coord3D coord) const = 0;

private:

    Canvas<Coord2D> &_canvas;

};

class ParallelProjection: public ProjectionCanvas
{
public:

    ParallelProjection(Canvas<Coord2D> &canvas) : ProjectionCanvas(canvas) {}

    Coord2D project(Coord3D coord) const override
    {
        return TVector(coord);
    }

};

class PerspectiveProjection: public ProjectionCanvas
{
public:

    PerspectiveProjection(Canvas<Coord2D> &canvas) : ProjectionCanvas(canvas) {}

    Coord2D project(UNUSED Coord3D coord) const override
    {
        //TODO Implement perspective projection.
    }

};


// Render a cross at center with radius, using color.
template<class Coord>
void render_cross(Canvas<Coord> &canvas, const Coord &center, double radius, const Color &color)
{
    // Horizontal bar
    canvas.move(translated<Coord>(center, TVector(Coord2D(-radius, 0))));
    canvas.draw_line(translated<Coord>(center, TVector(Coord2D(+radius, 0))), color);

    // Vertical bar
    canvas.move(translated<Coord>(center, TVector(Coord2D(0, -radius))));
    canvas.draw_line(translated<Coord>(center, TVector(Coord2D(0, +radius))), color);
}

template<class Coord>
class World
{
public:

    using DisplayFile = ::DisplayFile<Coord>;
    using Object = ::Object<Coord>;

    World(shared_ptr<Window> window, DisplayFile display_file)
        : _window(window), _display_file(display_file), _center(0, 0) {}

    shared_ptr<Window> window() { return _window; }

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

    // Render DisplayFile, the center, the x axis and y axis on canvas.
    void render(Canvas<Coord2D> &canvas)
    {
        render_axis(canvas);

#ifdef WORLD_2D
        _display_file.render(canvas);
        render_controls(canvas);
#endif

#ifdef WORLD_3D
        if (projection_method == ProjectionMethod::PARALLEL)
        {
            ParallelProjection projectionCanvas(canvas);
            _display_file.render(projectionCanvas);
            render_controls(projectionCanvas);
        }
        else
        {
            PerspectiveProjection projectionCanvas(canvas);
            _display_file.render(projectionCanvas);
            render_controls(projectionCanvas);
        }
#endif

        render_center(canvas);
        _window->draw(canvas);
    }

    // Select the object at index.
    void select_object_at(size_t index)
    {
        assert(index >= 0 && index < objects().size());

        shared_ptr<Object> object = objects().at(index);
        object->highlight_on();
        _selected_objects.push_back(object);
        _center = TVector(object->center());
    }

    // Remove all from the list of selected objects.
    void clear_selection()
    {
        for(auto &object: _selected_objects) object->highlight_off();
        _selected_objects.clear();
        _center = Coord2D(0, 0);
    }

    // True if any objects is selected.
    bool has_selected_objects()
    {
        return _selected_objects.size() > 0;
    }

    // Move the selected objects by dx horizontally, dy vertically.
    void translate_selected(double dx, double dy)
    {
        for (shared_ptr<Object> object: _selected_objects)
        {
            object->translate(TVector(Coord2D(dx, dy)));
            _center = TVector(object->center());
        }
    }

    // Scale the selected objects by factor.
    void scale_selected(double factor)
    {
        for (shared_ptr<Object> object: _selected_objects)
            object->scale(factor, TVector(_center));
    }

    // Rotate the selected objects by degrees at world center; clockwise if degrees positive; counter-clockwise if negative.
    void rotate_selected(double degrees)
    {
        for (shared_ptr<Object> object: _selected_objects)
            object->rotate(degrees, TVector(_center));
    }

    // Set the new center from viewport coordinates
    void set_center_from_viewport(VC center, const Viewport &viewport)
    {
        _center = TVector(_window->to_world(_window->from_viewport(center, viewport)));
    }

private:

    // Render the x axis and y axis.
    void render_axis(Canvas<Coord2D> &canvas)
    {
        const Coord2D center = Coord2D(0, 0);
        const int radius = 10000;

        render_cross(canvas, center, radius, GREEN);
    }

    // Render controls of selected objects.
    void render_controls(Canvas<Coord> &canvas)
    {
        const int radius = 2;

        for (auto obj: _selected_objects)
        {
            for (auto control: obj->controls())
            {
                render_cross(canvas, *control, radius, CONTROL);
            }
        }
    }

    // Render the center as a little cross.
    void render_center(Canvas<Coord2D> &canvas)
    {
        const int radius = 2;

        render_cross(canvas, _center, radius, GREEN);
    }

    shared_ptr<Window> _window;
    DisplayFile _display_file;
    list<shared_ptr<Object>> _selected_objects;
    Coord2D _center;

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

inline shared_ptr<Draw2DCommand> draw_bezier(Coord2D edge1, Coord2D control1, Coord2D edge2, Coord2D control2)
{
    return make_shared<Draw2DCommand>(make_shared<Bezier>(Bezier(edge1, control1, edge2, control2)));
}

inline shared_ptr<Draw2DCommand> draw_spline(initializer_list<Coord2D> controls)
{
    return make_shared<Draw2DCommand>(make_shared<Spline>(Spline(controls)));
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

    cube.transform(x_rotation(30) * y_rotation(30) * z_rotation(30));

    return make_shared<Draw3DCommand>(make_shared<Object3D>(cube));
}
