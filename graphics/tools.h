#pragma once

#include "display.h"

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

// Axis of rotation selected by the user
enum RotationAxis
{
    X_AXIS, Y_AXIS, Z_AXIS
};

// Selection of world objects that can be manipulated by UI tools
template<class Coord>
class Selection
{
public:

    using Object = ::Object<Coord>;
    using Group = ::Group<Coord>;
    using World = ::World<Coord>;
    using DisplayFile = ::DisplayFile<Coord>;

    Selection(World &world)
    : _world(world), _center(0, 0) {}

    World & world() { return _world; }

    DisplayFile & display_file() { return _world.display_file(); }

    shared_ptr<Window> window() { return _world.window(); }

    // Select the world object at index.
    void select_object_at(size_t index)
    {
        assert(index >= 0 && index < _world.objects().size());

        shared_ptr<Object> object = _world.objects().at(index);
        object->highlight_on();
        _selected_group.add(object);
        _center = TVector(object->center());
    }

    // Remove all from the list of selected objects.
    void clear()
    {
        for(auto object: _selected_group.objects()) object->highlight_off();
        _selected_group.removeAll();
        _center = Coord2D(0, 0);
    }

    // True if any objects are selected.
    bool not_empty()
    {
        return _selected_group.not_empty();
    }

    // Move the selected objects by delta.
    void translate(Coord delta)
    {
        _selected_group.translate(delta);
        _center = TVector(_selected_group.center());
    }

    // Scale the selected objects by factor.
    void scale(double factor)
    {
        _selected_group.scale(factor, TVector(_center));
    }

    // Rotate the selected objects by degrees at world center; clockwise if degrees positive; counter-clockwise if negative.
    void rotate(double degrees)
    {
        switch(_rotation_axis)
        {
            case X_AXIS: _selected_group.rotate_x(degrees, TVector(_center)); break;
            case Y_AXIS: _selected_group.rotate_y(degrees, TVector(_center)); break;
            case Z_AXIS: _selected_group.rotate_z(degrees, TVector(_center)); break;
        }
    }

    // Select the rotation according to axis.
    void select_rotation_axis(RotationAxis axis)
    {
        _rotation_axis = axis;
    }

    // Set the new center from viewport coordinates
    void set_center_from_viewport(VC center, const Viewport &viewport)
    {
        const Window &window = *_world.window();
        _center = TVector(window.to_world(window.from_viewport(center, viewport)));
    }

    // Render controls of selected objects.
    void render_controls(Canvas<Coord> &canvas)
    {
        const int radius = 2;

        for (auto control: _selected_group.controls())
        {
            render_cross(canvas, *control, radius, CONTROL);
        }
    }

    // Render the center as a little cross.
    void render_center(Canvas<Coord2D> &canvas)
    {
        const int radius = 2;

        render_cross(canvas, _center, radius, GREEN);
    }

private:

    World &_world;
    Group _selected_group;
    Coord2D _center;
    RotationAxis _rotation_axis = Z_AXIS;

};

// Area on a screen to execute display commands
template<class Coord>
class ViewportCanvas: public Canvas<Coord2D>, public Viewport, public ClippingArea
{
public:

    ViewportCanvas(double width, double height, shared_ptr<Window> window, Canvas<VC> &canvas)
        : Viewport(width, height), _window(window), _canvas(canvas) {}

    // Render DisplayFile, the center, the x axis and y axis on canvas.
    void render(DisplayFile<Coord> &display_file, Selection<Coord> &selection)
    {
        render_axis();

#ifdef WORLD_2D
        Canvas<Coord2D> *projection_canvas = this;
#endif

#ifdef WORLD_3D
        shared_ptr<ProjectionCanvas<Coord3D>> projection_canvas;
        if (projection_method == ProjectionMethod::PARALLEL)
        {
            projection_canvas = make_shared<ParallelProjection>(*this, _window);
        }
        else
        {
            projection_canvas = make_shared<PerspectiveProjection>(*this, _window);
        }
#endif

        display_file.render(*projection_canvas);
//        selection.render_controls(*projection_canvas);

        selection.render_center(*this);
        _window->draw(*this);
    }

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

    // Render the x axis and y axis.
    void render_axis()
    {
        const Coord2D center = Coord2D(0, 0);
        const int radius = 10000;

        render_cross(*this, center, radius, GREEN);
    }

    shared_ptr<Window> _window;
    Canvas<VC> &_canvas;

};

