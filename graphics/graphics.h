#pragma once

#include <memory>
#include <vector>
#include <list>
#include <sstream>
#include "coord.h"

using namespace std;

class Color
{
public:

    Color(double red, double green, double blue): _red(red), _green(green), _blue(blue) {}

    double const red() const { return _red; }
    double const green() const { return _green; }
    double const blue() const { return _blue; }

private:

    double _red, _green, _blue;

};

const Color BLACK = Color(0, 0, 0);
const Color RED = Color(1, 0, 0);
const Color BLUE = Color(0, 0, 1);

// Drawable area of the screen
class Canvas
{
public:

    // Move to destination.
    virtual void move(const Coord &destination) = 0;

    // Draw line from current position to destination.
    virtual void draw_line(const Coord &destination, const Color &color) = 0;

    // Draw circle with the specified center, radius and color.
    virtual void draw_circle(const Coord &center, const double radius, const Color &color) = 0;

};

// Drawable objects
class Drawable
{
public:

    // Draw something in canvas.
    virtual void draw(Canvas &canvas) = 0;

};

// World objects
class Object: public Drawable
{
public:

    Object(): _color(BLACK)
    {
        _id = ++_count;
    }

    virtual void draw(Canvas &canvas) = 0;

    virtual string type() = 0;

    virtual string name()
    {
        stringstream ss;
        ss << type() << _id;
        return ss.str();
    }

    void highlight_on()
    {
        _color = RED;
    }

    void highlight_off()
    {
        _color = BLACK;
    }

    Color color()
    {
        return _color;
    }

    // Translate by dx horizontally, dy vertically.
    virtual void translate(double dx, double dy) = 0;

    // Scale by factor.
    virtual void scale(double factor) = 0;

    // Scale by factor from center.
    virtual void scale(double factor, Coord center) = 0;

    // Rotate by degrees at the world origin; clockwise if degrees positive; counter-clockwise if negative.
    virtual void rotate(double degrees) = 0;

    // Rotate by degrees at center; clockwise if degrees positive; counter-clockwise if negative.
    virtual void rotate(double degrees, Coord center) = 0;

    // Object's center
    virtual Coord center() = 0;

private:

    int _id;
    Color _color;

    static int _count;

};

int Object::_count = 0;

// Two-dimensional points
class Point: public Object
{
public:

    Point(Coord coord): _coord(coord) {}

    // Draw a point in canvas at position (x, y).
    void draw(Canvas &canvas)
    {
        canvas.draw_circle(_coord, 1.5, color());
    }

    virtual string type()
    {
        return "Point";
    }

    // Translate by dx horizontally, dy vertically.
    virtual void translate(double dx, double dy)
    {
        ::translate(_coord, dx, dy);
    }

    // Scale by factor.
    virtual void scale(double factor)
    {
        ::scale(_coord, factor);
    }

    // Scale by factor from center.
    virtual void scale(double factor, Coord center)
    {
        ::scale(_coord, factor, center);
    }

    // Rotate by degrees at the world origin; clockwise if degrees positive; counter-clockwise if negative.
    virtual void rotate(double degrees)
    {
        ::rotate(_coord, degrees);
    }

    // Rotate by degrees at center; clockwise if degrees positive; counter-clockwise if negative.
    virtual void rotate(double degrees, Coord center)
    {
        ::rotate(_coord, degrees, center);
    }

    // Coord of the Point itself
    virtual Coord center()
    {
        return _coord;
    }

private:

    Coord _coord;

};

// Straight one-dimensional figure delimited by two points
class Line: public Object
{
public:

    Line(Coord a, Coord b): _a(a), _b(b) {}

    // Draw line in canvas.
    void draw(Canvas &canvas)
    {
        canvas.move(_a);
        canvas.draw_line(_b, color());
    }

    virtual string type()
    {
        return "Line";
    }

    // Translate by dx horizontally, dy vertically.
    virtual void translate(double dx, double dy)
    {
        ::translate(_a, dx, dy);
        ::translate(_b, dx, dy);
    }

    // Scale by factor.
    virtual void scale(double factor)
    {
        ::scale(_a, factor);
        ::scale(_b, factor);
    }

    // Scale by factor from center.
    virtual void scale(double factor, Coord center)
    {
        ::scale(_a, factor, center);
        ::scale(_b, factor, center);
    }

    // Rotate by degrees at the world origin; clockwise if degrees positive; counter-clockwise if negative.
    virtual void rotate(double degrees)
    {
        ::rotate(_a, degrees);
        ::rotate(_b, degrees);
    }

    // Rotate by degrees at center; clockwise if degrees positive; counter-clockwise if negative.
    virtual void rotate(double degrees, Coord center)
    {
        ::rotate(_a, degrees, center);
        ::rotate(_b, degrees, center);
    }

    // Midpoint between a and b
    virtual Coord center()
    {
        return Coord((_a.x() + _b.x()) / 2, (_a.y() + _b.y()) / 2);
    }

private:
    Coord _a, _b;
};

// Plane figure bound by a set of lines - the sides - meeting in a set of points - the vertices
class Polygon: public Object
{
public:

    Polygon(initializer_list<Coord> vertices): _vertices(vertices) {}

    void draw(Canvas &canvas)
    {
        Coord previous = _vertices.back();
        for (auto &current: _vertices)
        {
            canvas.move(previous);
            canvas.draw_line(current, color());
            previous = current;
        }
    }

    virtual string type()
    {
        return "Polygon";
    }

    // Translate by dx horizontally, dy vertically.
    virtual void translate(double dx, double dy)
    {
        for (Coord &coord: _vertices)
            ::translate(coord, dx, dy);
    }

    // Scale by factor.
    virtual void scale(double factor)
    {
        for (Coord &coord: _vertices)
            ::scale(coord, factor);
    }

    // Scale by factor from center.
    virtual void scale(double factor, Coord center)
    {
        for (Coord &coord: _vertices)
            ::scale(coord, factor, center);
    }

    // Rotate by degrees at the world origin; clockwise if degrees positive; counter-clockwise if negative.
    virtual void rotate(double degrees)
    {
        for (Coord &coord: _vertices)
            ::rotate(coord, degrees);
    }

    // Rotate by degrees at center; clockwise if degrees positive; counter-clockwise if negative.
    virtual void rotate(double degrees, Coord center)
    {
        for (Coord &coord: _vertices)
            ::rotate(coord, degrees, center);
    }

    // Midpoint between a and b
    virtual Coord center()
    {
        double x = 0, y = 0;

        for (Coord &coord: _vertices)
        {
            x += coord.x();
            y += coord.y();
        }

        return Coord(x / _vertices.size(), y / _vertices.size());
    }

private:
    list<Coord> _vertices;
};


// Visible area of the world
class Window
{
public:
    constexpr static int norm_left = -1;
    constexpr static int norm_bottom = -1;
    constexpr static int norm_width = 2;
    constexpr static int norm_height = 2;

    Window(double left, double bottom, double right, double top)
        :_leftBottom(left, bottom), _rightTop(right, top), _center(equidistant(_leftBottom, _rightTop)), _up_angle(0) {}

    double left() const { return _leftBottom.x(); }
    double bottom() const { return _leftBottom.y(); }
    double right() const { return _rightTop.x(); }
    double top() const { return _rightTop.y(); }

    double width() const { return right() - left(); }
    double height() const { return top() - bottom(); }

    // Translate coord from world to window, where left-bottom is (-1, -1) and right-top is (1, 1).
    Coord from_world(Coord coord) const
    {
        return coord *
            translation(-_center.x(), -_center.y()) *
            rotation(_up_angle) *
            scaling(norm_width / width(), norm_height / height());
    }

    // Translate coord from window to world.
    Coord to_world(Coord coord) const
    {
        return coord *
            rotation(-_up_angle) *
            scaling(width() / norm_width, height() / norm_height) *
            translation(_center.x(), _center.y());
    }

    // Translate coord from viewport to window.
    Coord from_viewport(Coord coord, double viewport_width, double viewport_height) const
    {
        return Coord(coord.x(), viewport_height - coord.y()) *
               scaling(norm_width / viewport_width, norm_height / viewport_height) *
               translation(norm_left, norm_bottom);
    }

    // Translate coord from window to viewport.
    Coord to_viewport(Coord coord, double viewport_width, double viewport_height) const
    {
        return Coord(coord.x() - norm_left, norm_height - (coord.y() - norm_bottom)) *
            scaling(viewport_width / norm_width, viewport_height / norm_height);
    }

    // Zoom out by factor
    void zoom_out(double factor)
    {
        double tx = width() * factor;
        double ty = height() * factor;

        _leftBottom *= translation(+tx, +ty);
        _rightTop *= translation(-tx, -ty);
    }

    // Zoom in by factor
    void zoom_in(double factor)
    {
        double tx = width() * factor;
        double ty = height() * factor;

        _leftBottom *= translation(-tx, -ty);
        _rightTop *= translation(+tx, +ty);
    }

    // Pan left by factor
    void pan_left(double factor)
    {
        double tx = width() * factor;

        _leftBottom *= translation(-tx, 0);
        _rightTop *= translation(-tx, 0);

        _center = equidistant(_leftBottom, _rightTop);
    }

    // Pan right by factor
    void pan_right(double factor)
    {
        double tx = width() * factor;

        _leftBottom *= translation(+tx, 0);
        _rightTop *= translation(+tx, 0);

        _center = equidistant(_leftBottom, _rightTop);
    }

    // Pan up by factor
    void pan_up(double factor)
    {
        double ty = height() * factor;

        _leftBottom *= translation(0, +ty);
        _rightTop *= translation(0, +ty);

        _center = equidistant(_leftBottom, _rightTop);
    }

    // Pan down by factor
    void pan_down(double factor)
    {
        double ty = height() * factor;

        _leftBottom *= translation(0, -ty);
        _rightTop *= translation(0, -ty);

        _center = equidistant(_leftBottom, _rightTop);
    }

private:
    Coord _leftBottom, _rightTop, _center;
    double _up_angle; // degrees
};

// Area on a screen to execute display commands
class Viewport: public Canvas
{
public:

    Viewport(double width, double height, Window &window, Canvas &canvas)
        : _width(width), _height(height), _window(window), _canvas(canvas) {}

    // Translate coord from world to viewport.
    Coord translate(const Coord &coord) const
    {
        return _window.to_viewport(_window.from_world(coord), _width, _height);
    }

    // Move to destination.
    virtual void move(const Coord &destination)
    {
        _canvas.move(translate(destination));
    }

    // Draw line from current position to destination.
    virtual void draw_line(const Coord &destination, const Color &color)
    {
        _canvas.draw_line(translate(destination), color);
    }

    // Draw circle with the specified center, radius and color.
    virtual void draw_circle(const Coord &center, const double radius, const Color &color)
    {
        _canvas.draw_circle(translate(center), radius, color);
    }

private:
    double _width, _height;
    Window &_window;
    Canvas &_canvas;
};

// Command to be executed in order to display an output image
class DisplayCommand
{
public:

    // Applies an object (image or figure) to the viewport.
    virtual void render(Viewport &viewport) = 0;

};

// Commands to draw objects
class DrawCommand: public DisplayCommand
{
public:

    DrawCommand(shared_ptr<Drawable> drawable): _drawable(drawable) {}

    // Applies drawable to the viewport.
    virtual void render(Viewport &viewport)
    {
        _drawable->draw(viewport);
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

    // Apply all commands to the viewport.
    void render(Viewport &viewport)
    {
        for (auto &command: _commands) command->render(viewport);
    }

private:
    // Commands to be executed
    list<shared_ptr<DisplayCommand>> _commands;
};

class World
{
public:
    World(Window window, DisplayFile display_file)
        : _window(window), _display_file(display_file), _center(0, 0) {}

    Window& window() { return _window; }

    // Objects from command list
    vector<shared_ptr<Object>> objects() {
        vector<shared_ptr<Object>> vector;

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

    // Render DisplayFile, the center, the x axis and y axis to viewport.
    void render(Viewport &viewport)
    {
        render_axis(viewport);
        render_center(viewport);
        _display_file.render(viewport);
    }

    // Select the object at index.
    void select_object_at(int index)
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
        _center = Coord(0, 0);
    }

    // True if any objects is selected.
    bool has_selected_objects()
    {
        return _selected_objects.size() > 0;
    }

    // Move the selected objects by dx horizontally, dy vertically.
    virtual void translate_selected(double dx, double dy)
    {
        for (shared_ptr<Object> object: _selected_objects)
        {
            object->translate(dx, dy);
            _center = object->center();
        }
    }

    // Scale the selected objects by factor.
    virtual void scale_selected(double factor)
    {
        for (shared_ptr<Object> object: _selected_objects)
            object->scale(factor, _center);
    }

    // Rotate the selected objects by degrees at world center; clockwise if degrees positive; counter-clockwise if negative.
    virtual void rotate_selected(double degrees)
    {
        for (shared_ptr<Object> object: _selected_objects)
            object->rotate(degrees, _center);
    }

    // Set the new center from viewport coordinates
    void set_center_from_viewport(Coord center, double viewport_width, double viewport_height)
    {
        _center = _window.to_world(_window.from_viewport(center, viewport_width, viewport_height));
    }

private:

    // Render the x axis and y axis.
    void render_axis(Viewport &viewport)
    {
        const double length = 10000;

        // x axis
        viewport.move(Coord(-length, 0));
        viewport.draw_line(Coord(+length, 0), BLUE);

        // y axis
        viewport.move(Coord(0, -length));
        viewport.draw_line(Coord(0, +length), BLUE);
    }

    // Render the center as a little cross
    void render_center(Viewport &viewport)
    {
        const double radius = 2;

        // Horizontal bar
        viewport.move(_center.translate(-radius, 0));
        viewport.draw_line(_center.translate(+radius, 0), BLUE);

        // Horizontal bar
        viewport.move(_center.translate(0, -radius));
        viewport.draw_line(_center.translate(0, +radius), BLUE);
    }

    Window _window;
    DisplayFile _display_file;
    list<shared_ptr<Object>> _selected_objects;
    Coord _center;

};

shared_ptr<DrawCommand> draw_point(Coord a)
{
    return make_shared<DrawCommand>(make_shared<Point>(a));
}

shared_ptr<DrawCommand> draw_line(Coord a, Coord b)
{
    return make_shared<DrawCommand>(make_shared<Line>(a, b));
}

shared_ptr<DrawCommand> draw_square(Coord a, Coord b, Coord c, Coord d)
{
    return make_shared<DrawCommand>(make_shared<Polygon>(Polygon({ a, b, c, d })));
}
