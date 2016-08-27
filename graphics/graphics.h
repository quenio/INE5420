#pragma once

#include <memory>
#include <vector>
#include <list>
#include <sstream>
#include "coord.h"

using namespace std;

// Drawable area of the screen
class Canvas
{
public:

    // Move to destination.
    virtual void move(const Coord &destination) = 0;

    // Draw line from current position to destination.
    virtual void draw_line(const Coord &destination) = 0;

};

// Drawable objects
class Drawable
{
public:

    // Draw something in canvas.
    virtual void draw(Canvas &canvas) = 0;

};

// World objects
class Object
{
public:

    Object()
    {
        _id = ++_count;
    }

    virtual string type() = 0;

    virtual string name()
    {
        stringstream ss;
        ss << type() << _id;
        return ss.str();
    }

    // Move by dx horizontally, dy vertically.
    virtual void move(double dx, double dy) = 0;

    // Scale by factor.
    virtual void scale(double factor) = 0;

    // Rotate by degrees at world center; clockwise if degrees positive; counter-clockwise if negative.
    virtual void rotate(double degrees) = 0;

private:
    int _id;

    static int _count;
};

int Object::_count = 0;

// Two-dimensional points
class Point: public Drawable, public Object
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
        canvas.draw_line(current);
        canvas.move(current);

        current *= translation(thickness, 0);
        canvas.draw_line(current);
        canvas.move(current);

        current *= translation(0, -thickness);
        canvas.draw_line(current);
        canvas.move(current);

        current *= translation(-thickness, 0);
        canvas.draw_line(current);
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
class Line: public Drawable, public Object
{
public:

    Line(Coord a, Coord b): _a(a), _b(b) {}

    // Draw line in canvas.
    void draw(Canvas &canvas)
    {
        canvas.move(_a);
        canvas.draw_line(_b);
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
class Polygon: public Drawable, public Object
{
public:

    Polygon(initializer_list<Coord> vertices): _vertices(vertices) {}

    void draw(Canvas &canvas)
    {
        Coord previous = _vertices.back();
        for (auto &current: _vertices)
        {
            canvas.move(previous);
            canvas.draw_line(current);
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
    virtual void draw_line(const Coord &destination)
    {
        _canvas.draw_line(translate(destination));
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

    void render(Viewport &viewport)
    {
        render_axis(viewport);
        _display_file.render(viewport);
    }

    // Move by dx horizontally, dy vertically.
    virtual void move(double dx, double dy)
    {
        for (shared_ptr<Object> object: objects())
            object->move(dx, dy);
    }

    // Scale by factor.
    virtual void scale(double factor)
    {
        for (shared_ptr<Object> object: objects())
            object->scale(factor);
    }

    // Rotate by degrees at world center; clockwise if degrees positive; counter-clockwise if negative.
    virtual void rotate(double degrees)
    {
        for (shared_ptr<Object> object: objects())
            object->rotate(degrees);
    }

private:

    void render_axis(Viewport &viewport)
    {
        // x axis
        viewport.move(Coord(-1000, 0));
        viewport.draw_line(Coord(+1000, 0));

        // y axis
        viewport.move(Coord(0, -1000));
        viewport.draw_line(Coord(0, +1000));
    }

    Window _window;
    DisplayFile _display_file;
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
    return make_shared<DrawCommand>(make_shared<Polygon>(Polygon { a, b, c, d }));
}
