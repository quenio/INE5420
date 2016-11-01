#pragma once

#include "transforms.h"

#include <memory>
#include <sstream>

class Color
{
public:

    Color(double red, double green, double blue): _red(red), _green(green), _blue(blue) {}

    double red() const { return _red; }
    double green() const { return _green; }
    double blue() const { return _blue; }

private:

    double _red, _green, _blue;

};

const Color BLACK = Color(0, 0, 0);
const Color RED = Color(1, 0, 0);
const Color GREEN = Color(0, 1, 0);
const Color BLUE = Color(0, 0, 1);
const Color CONTROL = Color(1, 0, 1);

// Drawable area of the screen
template<class Coord>
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
template<class Coord>
class Drawable
{
public:

    // Draw something in canvas.
    virtual void draw(Canvas<Coord> &canvas) = 0;

    // Color used to draw.
    virtual Color color() const = 0;

};

// Groups of objects
template<class Coord> class Group;

// World objects
template<class Coord>
class Object: public virtual Drawable<Coord>, public Transformable<Coord>
{
    friend Group<Coord>;

public:

    Object(const Color &color = BLACK): _color(color), _regular_color(color)
    {
        _id = ++_count;
    }

    // Type used in the name
    virtual string type() const = 0;

    // Name displayed on the UI
    virtual string name() const
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
        _color = _regular_color;
    }

    // Color used to draw.
    Color color() const override
    {
        return _color;
    }

private:

    int _id;
    Color _color, _regular_color;

    static int _count;

};

// Command to be executed in order to display an output image
template<class Coord>
class DisplayCommand
{
public:

    using Object = ::Object<Coord>;

    // Render an object (image or figure) on canvas.
    virtual void render(Canvas<Coord> &canvas) = 0;

    virtual shared_ptr<Object> object() const = 0;

};

// List of commands to be executed in order to display an output image
template<class Coord>
class DisplayFile
{
public:

    using Command = DisplayCommand<Coord>;

    DisplayFile(initializer_list<shared_ptr<Command>> commands): _commands(commands) {}

    // Commands to be executed
    list<shared_ptr<Command>> commands()
    {
        return _commands;
    }

    // Render all commands on canvas.
    void render(Canvas<Coord> &canvas)
    {
        for (auto &command: _commands) command->render(canvas);
    }

private:

    // Commands to be executed
    list<shared_ptr<Command>> _commands;

};

// Groups of objects
template<class Coord>
class Group: public Transformable<Coord>
{
public:

    using Object = ::Object<Coord>;

    // Controls of all objects in group
    list<Coord *> controls() override
    {
        list<Coord *> controls;

        for (auto s: _objects)
            for (auto c: s->controls())
                controls.push_back(c);

        return controls;
    }

    // Transform all objects according to matrix.
    void transform(TMatrix matrix) override
    {
        for (auto s: _objects)
            s->transform(matrix);
    }

    // Translate by delta.
    void translate(Coord delta) override
    {
        for (auto s: _objects)
            s->translate(delta);
    }

    // Scale by factor from center.
    void scale(double factor, Coord center) override
    {
        for (auto s: _objects)
            s->scale(factor, center);
    }

    // Rotate on the x axis by degrees at center; clockwise if degrees positive; counter-clockwise if negative.
    void rotate_x(double degrees, Coord center) override
    {
        for (auto s: _objects)
            s->rotate_x(degrees, center);
    }

    // Rotate on the y axis by degrees at center; clockwise if degrees positive; counter-clockwise if negative.
    void rotate_y(double degrees, Coord center) override
    {
        for (auto s: _objects)
            s->rotate_y(degrees, center);
    }

    // Rotate on the z axis by degrees at center; clockwise if degrees positive; counter-clockwise if negative.
    void rotate_z(double degrees, Coord center) override
    {
        for (auto s: _objects)
            s->rotate_z(degrees, center);
    }

    // True if any objects is selected.
    bool not_empty()
    {
        return _objects.size() > 0;
    }

    // All objects in the group
    list<shared_ptr<Object>> objects()
    {
        return _objects;
    }

    // Add object to back of the group.
    void add(shared_ptr<Object> object)
    {
        _objects.push_back(object);
    }

    // Remove all objects in the group.
    void removeAll()
    {
        _objects.clear();
    }

private:

    list<shared_ptr<Object>> _objects;

};

// Sequence of lines drawn from the given vertices.
template<class Coord>
class Polyline: public virtual Drawable<Coord>
{
public:

    // Vertices to use when drawing the lines.
    virtual list<shared_ptr<Coord>> vertices() const = 0;

    // Initial vertex of the first line to be drawn
    // nullptr if should start from first vertex in the list of vertices
    virtual shared_ptr<Coord> initial_vertex() const
    {
        return nullptr;
    }

    // Draw the sequence of lines in canvas.
    void draw(Canvas<Coord> &canvas) override
    {
        shared_ptr<Coord> previous = initial_vertex();
        for (auto current: vertices())
        {
            if (previous != nullptr)
            {
                canvas.move(*previous);

                if (current != nullptr)
                {
                    canvas.draw_line(*current, this->color());
                }
            }

            previous = current;
        }
    }

};
