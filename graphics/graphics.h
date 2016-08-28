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

    // Move by dx horizontally, dy vertically.
    virtual void move(double dx, double dy) = 0;

    // Scale by factor.
    virtual void scale(double factor) = 0;

    // Rotate by degrees at world center; clockwise if degrees positive; counter-clockwise if negative.
    virtual void rotate(double degrees) = 0;

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
        const double thickness = 0.3;

        Coord current = _coord;
        canvas.move(current);

        current *= translation(0, thickness);
        canvas.draw_line(current, color());
        canvas.move(current);

        current *= translation(thickness, 0);
        canvas.draw_line(current, color());
        canvas.move(current);

        current *= translation(0, -thickness);
        canvas.draw_line(current, color());
        canvas.move(current);

        current *= translation(-thickness, 0);
        canvas.draw_line(current, color());
    }

    virtual string type()
    {
        return "Point";
    }

    // Move by dx horizontally, dy vertically.
    virtual void move(double dx, double dy)
    {
        ::move(_coord, dx, dy);
    }

    // Scale by factor.
    virtual void scale(double factor)
    {
        ::scale(_coord, factor);
    }

    // Rotate by degrees at world center; clockwise if degrees positive; counter-clockwise if negative.
    virtual void rotate(double degrees)
    {
        ::rotate(_coord, degrees);
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

    // Move by dx horizontally, dy vertically.
    virtual void move(double dx, double dy)
    {
        ::move(_a, dx, dy);
        ::move(_b, dx, dy);
    }

    // Scale by factor.
    virtual void scale(double factor)
    {
        ::scale(_a, factor);
        ::scale(_b, factor);
    }

    // Rotate by degrees at world center; clockwise if degrees positive; counter-clockwise if negative.
    virtual void rotate(double degrees)
    {
        ::rotate(_a, degrees);
        ::rotate(_b, degrees);
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

    // Move by dx horizontally, dy vertically.
    virtual void move(double dx, double dy)
    {
        for (Coord &coord: _vertices)
            ::move(coord, dx, dy);
    }

    // Scale by factor.
    virtual void scale(double factor)
    {
        for (Coord &coord: _vertices)
            ::scale(coord, factor);
    }

    // Rotate by degrees at world center; clockwise if degrees positive; counter-clockwise if negative.
    virtual void rotate(double degrees)
    {
        for (Coord &coord: _vertices)
            ::rotate(coord, degrees);
    }

private:
    list<Coord> _vertices;
};


// Visible area of the world
class Window
{
public:

    Window(double left, double bottom, double right, double top)
        :_leftBottom(left, bottom), _rightTop(right, top) {}

    double left() const { return _leftBottom.x(); }
    double bottom() const { return _leftBottom.y(); }
    double right() const { return _rightTop.x(); }
    double top() const { return _rightTop.y(); }

    double width() const { return right() - left(); }
    double height() const { return top() - bottom(); }

    // Normalize point to window dimensions
    Coord normalize(const Coord &point) const
    {
        return Coord(
            (point.x() - left()) / width(),
            1.0 - ((point.y() - bottom()) / height())
        );
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

    // Span left by factor
    void span_left(double factor)
    {
        double tx = width() * factor;

        _leftBottom *= translation(-tx, 0);
        _rightTop *= translation(-tx, 0);
    }

    // Span right by factor
    void span_right(double factor)
    {
        double tx = width() * factor;

        _leftBottom *= translation(+tx, 0);
        _rightTop *= translation(+tx, 0);
    }

    // Span up by factor
    void span_up(double factor)
    {
        double ty = height() * factor;

        _leftBottom *= translation(0, +ty);
        _rightTop *= translation(0, +ty);
    }

    // Span down by factor
    void span_down(double factor)
    {
        double ty = height() * factor;

        _leftBottom *= translation(0, -ty);
        _rightTop *= translation(0, -ty);
    }

private:
    Coord _leftBottom, _rightTop;
};

// Area on a screen to execute display commands
class Viewport: public Canvas
{
public:

    Viewport(double width, double height, Window &window, Canvas &canvas)
        : _width(width), _height(height), _window(window), _canvas(canvas) {}

    // Translate p from window to canvas.
    Coord translate(const Coord &p) const
    {
        return _window.normalize(p) * scaling(_width, _height);
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
    World(Window window, DisplayFile display_file): _window(window), _display_file(display_file) {}

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

    // Render DisplayFile to viewport, and the x axis and y axis.
    void render(Viewport &viewport)
    {
        render_axis(viewport);
        _display_file.render(viewport);
    }

    // Select the object at index.
    void select_object_at(int index)
    {
        assert(index >= 0 && index < objects().size());

        shared_ptr<Object> object = objects().at(index);
        object->highlight_on();
        _selected_objects.push_back(object);
    }

    // Remove all from the list of selected objects.
    void clear_selection()
    {
        for(auto &object: _selected_objects) object->highlight_off();
        _selected_objects.clear();
    }

    // True if any objects are selected.
    bool has_selected_objects()
    {
        return _selected_objects.size() > 0;
    }

    // Move the selected objects by dx horizontally, dy vertically.
    virtual void move_selected(double dx, double dy)
    {
        for (shared_ptr<Object> object: _selected_objects)
            object->move(dx, dy);
    }

    // Scale the selected objects by factor.
    virtual void scale_selected(double factor)
    {
        for (shared_ptr<Object> object: _selected_objects)
            object->scale(factor);
    }

    // Rotate the selected objects by degrees at world center; clockwise if degrees positive; counter-clockwise if negative.
    virtual void rotate_selected(double degrees)
    {
        for (shared_ptr<Object> object: _selected_objects)
            object->rotate(degrees);
    }

private:

    // Render the x axis and y axis.
    void render_axis(Viewport &viewport)
    {
        // x axis
        viewport.move(Coord(-1000, 0));
        viewport.draw_line(Coord(+1000, 0), BLUE);

        // y axis
        viewport.move(Coord(0, -1000));
        viewport.draw_line(Coord(0, +1000), BLUE);
    }

    Window _window;
    DisplayFile _display_file;
    list<shared_ptr<Object>> _selected_objects;

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
