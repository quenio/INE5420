#pragma once

#include "graphics2d.h"

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
        return coord * inverse_translation(_center) * rotation(_up_angle) * scaling(window_ratios());
    }

    // Translate coord from Window to World.
    Coord2D to_world(PPC coord) const
    {
        return coord * rotation(-_up_angle) * scaling(world_ratios()) * translation(_center);
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

// Command to be executed in order to display an output image
class DisplayCommand
{
public:

    // Render an object (image or figure) on canvas.
    virtual void render(ViewportCanvas &canvas) = 0;

};

// Commands to draw objects
class DrawCommand: public DisplayCommand
{
public:

    DrawCommand(shared_ptr<Drawable2D> drawable): _drawable(drawable) {}

    // Render drawable on canvas if visible.
    void render(ViewportCanvas &canvas) override
    {
        switch (_drawable->visibility_in(canvas))
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
                    shared_ptr<Drawable2D> clipped = clippable->clipped_in(canvas);
                    if (clipped->visibility_in(canvas) == Visibility::FULL)
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

    shared_ptr<Object2D> object()
    {
        return dynamic_pointer_cast<Object2D>(_drawable);
    }

private:
    shared_ptr<Drawable2D> _drawable;
};

// List of commands to be executed in order to display an output image
class DisplayFile
{
public:

    DisplayFile(initializer_list<shared_ptr<DisplayCommand>> commands): _commands(commands) {}

    // Commands to be executed
    list<shared_ptr<DisplayCommand>> commands()
    {
        return _commands;
    }

    // Render all commands on canvas.
    void render(ViewportCanvas &canvas)
    {
        for (auto &command: _commands) command->render(canvas);
    }

private:
    // Commands to be executed
    list<shared_ptr<DisplayCommand>> _commands;
};

class World
{
public:
    World(shared_ptr<Window> window, DisplayFile display_file)
        : _window(window), _display_file(display_file), _center(0, 0) {}

    shared_ptr<Window> window() { return _window; }

    // Objects from command list
    vector<shared_ptr<Object2D>> objects() {
        vector<shared_ptr<Object2D>> vector;

        vector.push_back(_window);

        for (auto &command: _display_file.commands())
        {
            shared_ptr<DrawCommand> drawCommand = dynamic_pointer_cast<DrawCommand>(command);
            if (drawCommand && drawCommand->object())
            {
                vector.push_back(drawCommand->object());
            }
        }

        return vector;
    }

    // Render DisplayFile, the center, the x axis and y axis on canvas.
    void render(ViewportCanvas &canvas)
    {
        render_axis(canvas);
        _display_file.render(canvas);
        render_controls(canvas);
        render_center(canvas);
        _window->draw(canvas);
    }

    // Select the object at index.
    void select_object_at(size_t index)
    {
        assert(index >= 0 && index < objects().size());

        shared_ptr<Object2D> object = objects().at(index);
        object->highlight_on();
        _selected_objects.push_back(object);
        _center = object->center();
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
        for (shared_ptr<Object2D> object: _selected_objects)
        {
            object->translate(Coord2D(dx, dy));
            _center = object->center();
        }
    }

    // Scale the selected objects by factor.
    void scale_selected(double factor)
    {
        for (shared_ptr<Object2D> object: _selected_objects)
            object->scale(factor, _center);
    }

    // Rotate the selected objects by degrees at world center; clockwise if degrees positive; counter-clockwise if negative.
    void rotate_selected(double degrees)
    {
        for (shared_ptr<Object2D> object: _selected_objects)
            object->rotate(degrees, _center);
    }

    // Set the new center from viewport coordinates
    void set_center_from_viewport(VC center, const Viewport &viewport)
    {
        _center = _window->to_world(_window->from_viewport(center, viewport));
    }

private:

    // Render a cross at center with radius, using color.
    void render_cross(Canvas<Coord2D> &canvas, const Coord2D &center, double radius, const Color &color)
    {
        // Horizontal bar
        canvas.move(translated<Coord2D>(center, Coord2D(-radius, 0)));
        canvas.draw_line(translated<Coord2D>(center, Coord2D(+radius, 0)), color);

        // Vertical bar
        canvas.move(translated<Coord2D>(center, Coord2D(0, -radius)));
        canvas.draw_line(translated<Coord2D>(center, Coord2D(0, +radius)), color);
    }

    // Render the x axis and y axis.
    void render_axis(Canvas<Coord2D> &canvas)
    {
        const Coord2D center = Coord2D(0, 0);
        const int radius = 10000;

        render_cross(canvas, center, radius, GREEN);
    }

    // Render controls of selected objects.
    void render_controls(Canvas<Coord2D> &canvas)
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
    list<shared_ptr<Object2D>> _selected_objects;
    Coord2D _center;

};

inline shared_ptr<DrawCommand> draw_point(Coord2D a)
{
    return make_shared<DrawCommand>(make_shared<Point>(a));
}

inline shared_ptr<DrawCommand> draw_line(Coord2D a, Coord2D b)
{
    return make_shared<DrawCommand>(make_shared<Line>(a, b));
}

inline shared_ptr<DrawCommand> draw_square(Coord2D a, Coord2D b, Coord2D c, Coord2D d)
{
    return make_shared<DrawCommand>(make_shared<Polygon>(Polygon({ a, b, c, d })));
}

inline shared_ptr<DrawCommand> draw_bezier(Coord2D edge1, Coord2D control1, Coord2D edge2, Coord2D control2)
{
    return make_shared<DrawCommand>(make_shared<Bezier>(Bezier(edge1, control1, edge2, control2)));
}

inline shared_ptr<DrawCommand> draw_spline(initializer_list<Coord2D> controls)
{
    return make_shared<DrawCommand>(make_shared<Spline>(Spline(controls)));
}
