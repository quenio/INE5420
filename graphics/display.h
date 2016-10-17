#pragma once

#include "graphics2d.h"

// Visible area on a canvas
class Viewport
{
public:

    constexpr static double margin_percentage = 0.025;

    Viewport(double width, double height): _width(width), _height(height), _margin(_width * margin_percentage) {}

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
class Window: public Object, public ClippingArea
{
public:

    constexpr static int norm_left = -1;
    constexpr static int norm_bottom = -1;
    constexpr static int norm_width = 2;
    constexpr static int norm_height = 2;

    Window(double left, double bottom, double right, double top)
        :Object(BLUE),
         _leftBottom(left, bottom), _leftTop(left, top), _rightTop(right, top), _rightBottom(right, bottom),
         _center(equidistant(_leftBottom, _rightTop)),
         _up_angle(0) {}

    Coord2D leftBottom() const { return _leftBottom; }
    Coord2D leftTop() const { return _leftTop; }
    Coord2D rightTop() const { return _rightTop; }
    Coord2D rightBottom() const { return _rightBottom; }

    double width() const { return distance(leftBottom(), rightBottom()); }
    double height() const { return distance(leftBottom(), leftTop()); }

    // True if Window contains World coord.
    bool contains(Coord2D coord) const override
    {
        Coord2D wc = from_world(coord);
        double x = wc.x(), y = wc.y();
        return ((x > -1 && x < +1) || equals(x, -1) || equals(x, +1)) &&
               ((y > -1 && y < +1) || equals(y, -1) || equals(y, +1));
    }

    // Translate coord from World to Window, where left-bottom is (-1, -1) and right-top is (1, 1).
    Coord2D world_to_window(Coord2D coord) const override
    {
        return from_world(coord);
    }

    // Translate coord from Window to World.
    Coord2D window_to_world(Coord2D coord) const override
    {
        return to_world(coord);
    }

    // Translate coord from World to Window, where left-bottom is (-1, -1) and right-top is (1, 1).
    Coord2D from_world(Coord2D coord) const
    {
        return coord *
               translation(-_center.x(), -_center.y()) *
               rotation(_up_angle) *
               scaling(norm_width / width(), norm_height / height());
    }

    // Translate coord from Window to World.
    Coord2D to_world(Coord2D coord) const
    {
        return coord *
               rotation(-_up_angle) *
               scaling(width() / norm_width, height() / norm_height) *
               translation(_center.x(), _center.y());
    }

    // Translate coord from Viewport to Window.
    Coord2D from_viewport(Coord2D coord, const Viewport &viewport) const
    {
        return Coord2D(coord.x(), viewport.height() - coord.y()) *
               translation(-viewport.left(), -viewport.top()) *
               scaling(norm_width / viewport.content_width(), norm_height / viewport.content_height()) *
               translation(norm_left, norm_bottom);
    }

    // Translate coord from Window to Viewport, leaving a margin.
    Coord2D to_viewport(Coord2D coord, const Viewport &viewport) const
    {
        return Coord2D(coord.x() - norm_left, norm_height - (coord.y() - norm_bottom)) *
               scaling(viewport.content_width() / norm_width, viewport.content_height() / norm_height) *
               translation(viewport.left(), viewport.top());
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

        translate(-tx, 0);
    }

    // Pan right by factor
    void pan_right(double factor)
    {
        double tx = width() * factor;

        translate(+tx, 0);
    }

    // Pan up by factor
    void pan_up(double factor)
    {
        double ty = height() * factor;

        translate(0, +ty);
    }

    // Pan down by factor
    void pan_down(double factor)
    {
        double ty = height() * factor;

        translate(0, -ty);
    }

    // Transform according to the matrix.
    void transform(TMatrix matrix) override
    {
        Object::transform(matrix);

        _center = equidistant(_leftBottom, _rightTop);
    }

    // Translate by dx horizontally, dy vertically.
    void translate(double dx, double dy) override
    {
        Object::translate(dx, dy);

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
    void draw(Canvas &canvas) override
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
class ViewportCanvas: public Canvas, public Viewport, public ClippingArea
{
public:

    ViewportCanvas(double width, double height, shared_ptr<Window> window, Canvas &canvas)
        : Viewport(width, height), _window(window), _canvas(canvas) {}

    // True if area contains world coord.
    bool contains(Coord2D coord) const override
    {
        return _window->contains(coord);
    }

    // Translate coord from World to Window, where left-bottom is (-1, -1) and right-top is (1, 1).
    Coord2D world_to_window(Coord2D coord) const override
    {
        return _window->world_to_window(coord);
    }

    // Translate coord from Window to World.
    Coord2D window_to_world(Coord2D coord) const override
    {
        return _window->window_to_world(coord);
    }

    // Translate coord from world to viewport
    Coord2D translate(const Coord2D &coord) const
    {
        return _window->to_viewport(_window->from_world(coord), *this);
    }

    // Move to destination.
    void move(const Coord2D &destination) override
    {
        _canvas.move(translate(destination));
    }

    // Draw line from current position to destination.
    void draw_line(const Coord2D &destination, const Color &color) override
    {
        _canvas.draw_line(translate(destination), color);
    }

    // Draw circle with the specified center, radius and color.
    void draw_circle(const Coord2D &center, const double radius, const Color &color) override
    {
        _canvas.draw_circle(translate(center), radius, color);
    }

private:

    shared_ptr<Window> _window;
    Canvas &_canvas;

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

    DrawCommand(shared_ptr<Drawable> drawable): _drawable(drawable) {}

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
                shared_ptr<Clippable<Drawable>> clippable = dynamic_pointer_cast<Clippable<Drawable>>(_drawable);
                if (clippable == nullptr)
                    _drawable->draw(canvas);
                else
                {
                    shared_ptr<Drawable> clipped = clippable->clipped_in(canvas);
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

    shared_ptr<Object> object()
    {
        return dynamic_pointer_cast<Object>(_drawable);
    }

private:
    shared_ptr<Drawable> _drawable;
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
    vector<shared_ptr<Object>> objects() {
        vector<shared_ptr<Object>> vector;

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

        shared_ptr<Object> object = objects().at(index);
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
        for (shared_ptr<Object> object: _selected_objects)
        {
            object->translate(dx, dy);
            _center = object->center();
        }
    }

    // Scale the selected objects by factor.
    void scale_selected(double factor)
    {
        for (shared_ptr<Object> object: _selected_objects)
            object->scale(factor, _center);
    }

    // Rotate the selected objects by degrees at world center; clockwise if degrees positive; counter-clockwise if negative.
    void rotate_selected(double degrees)
    {
        for (shared_ptr<Object> object: _selected_objects)
            object->rotate(degrees, _center);
    }

    // Set the new center from viewport coordinates
    void set_center_from_viewport(Coord2D center, const Viewport &viewport)
    {
        _center = _window->to_world(_window->from_viewport(center, viewport));
    }

private:

    // Render a cross at center with radius, using color.
    void render_cross(Canvas &canvas, const Coord2D &coord, double radius, const Color &color)
    {
        // Horizontal bar
        canvas.move(translated<Coord2D>(coord, -radius, 0));
        canvas.draw_line(translated<Coord2D>(coord, +radius, 0), color);

        // Vertical bar
        canvas.move(translated<Coord2D>(coord, 0, -radius));
        canvas.draw_line(translated<Coord2D>(coord, 0, +radius), color);
    }

    // Render the x axis and y axis.
    void render_axis(Canvas &canvas)
    {
        const Coord2D center = Coord2D(0, 0);
        const int radius = 10000;

        render_cross(canvas, center, radius, GREEN);
    }

    // Render controls of selected objects.
    void render_controls(Canvas &canvas)
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
    void render_center(Canvas &canvas)
    {
        const int radius = 2;

        render_cross(canvas, _center, radius, GREEN);
    }

    shared_ptr<Window> _window;
    DisplayFile _display_file;
    list<shared_ptr<Object>> _selected_objects;
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
