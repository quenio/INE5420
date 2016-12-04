#pragma once

#include "display.h"
#include "timer.h"

// Render a cross at center with radius, using color.
template<class Coord>
void render_cross(Canvas<Coord> &canvas, const Coord &center, double radius, const Color &h_color, const Color &v_color)
{
    // Horizontal bar
    canvas.set_color(h_color);
    canvas.move(translated<Coord>(center, TVector(Coord2D(-radius, 0))));
    canvas.draw_line(translated<Coord>(center, TVector(Coord2D(+radius, 0))));

    // Vertical bar
    canvas.set_color(v_color);
    canvas.move(translated<Coord>(center, TVector(Coord2D(0, -radius))));
    canvas.draw_line(translated<Coord>(center, TVector(Coord2D(0, +radius))));
}

// Axis of rotation selected by the user
enum TransformAxis
{
    X_AXIS, Y_AXIS, Z_AXIS, Z_ALL
};

enum Tool { NONE, TRANSLATE, SCALE, ROTATE };

// Selection of world objects that can be manipulated by UI tools
template<class Coord>
class Selection: public RenderingListener<Coord>
{
public:

    using Object = ::Object<Coord>;
    using Group = ::Group<Coord>;
    using World = ::World<Coord>;
    using DisplayFile = ::DisplayFile<Coord>;
    using Command = ::DisplayCommand<Coord>;
    using Canvas = ::Canvas<Coord>;

    Selection(World &world)
    : _world(world), _center(0, 0) {}

    World & world() { return _world; }

    DisplayFile & display_file() { return _world.display_file(); }

    shared_ptr<Window> window() { return _world.window(); }

    Coord2D center() { return _center; }

    // Tool used on selected objects.
    Tool tool() { return _tool; }

    // Select the tool to be used on selected objects.
    void select_tool(Tool tool)
    {
        if (not_empty())
        {
            _tool = tool;
        }
        else
        {
            _tool = NONE;
        }
    }

    // Select all objects if none selected, or unselect objects if any.
    void toggle_full_selection()
    {
        if (not_empty())
        {
            clear();
        }
        else
        {
            const size_t size = _world.objects().size();
            for (size_t i = 0; i < size; i++)
            {
                select_object_at(i);
            }
        }
    }

    // Select the world object at index.
    void select_object_at(size_t index)
    {
        assert(index >= 0 && index < _world.objects().size());

        shared_ptr<Object> object = _world.objects().at(index);
        _selected_group.add(object);
        _center = TVector(object->center());
    }

    // Remove all from the list of selected objects.
    void clear()
    {
        _tool = NONE;
        _selected_group.removeAll();
        _center = Coord2D(0, 0);
    }

    // True if any objects are selected.
    bool not_empty()
    {
        return _selected_group.not_empty();
    }

    // Move the selected objects by delta x, y and z.
    void translate(double delta_x, double delta_y, double delta_z)
    {
        switch(_transform_axis)
        {
            case X_AXIS: _selected_group.translate(TVector({ delta_x, 0, 0, 1 })); break;
            case Y_AXIS: _selected_group.translate(TVector({ 0, delta_y, 0, 1 })); break;
            case Z_AXIS: _selected_group.translate(TVector({ 0, 0, delta_z, 1 })); break;
            case Z_ALL: _selected_group.translate(TVector({ delta_x, delta_y, delta_z, 1 })); break;
        }
        _center = TVector(_selected_group.center());
    }

    // Scale the selected objects by factor.
    void scale(double factor)
    {
        switch(_transform_axis)
        {
            case X_AXIS: _selected_group.scale_x(factor, TVector(_center)); break;
            case Y_AXIS: _selected_group.scale_y(factor, TVector(_center)); break;
            case Z_AXIS: _selected_group.scale_z(factor, TVector(_center)); break;
            case Z_ALL: _selected_group.scale(factor, TVector(_center)); break;
        }
    }

    // Rotate the selected objects by degrees at world center; clockwise if degrees positive; counter-clockwise if negative.
    void rotate(double degrees)
    {
        switch(_transform_axis)
        {
            case X_AXIS: _selected_group.rotate_x(degrees, TVector(_center)); break;
            case Y_AXIS: _selected_group.rotate_y(degrees, TVector(_center)); break;
            case Z_AXIS: _selected_group.rotate_z(degrees, TVector(_center)); break;
            case Z_ALL: break;
        }
    }

    // Select the axis to be transformed.
    void select_transform_axis(TransformAxis axis)
    {
        _transform_axis = axis;
    }

    // Set the new center from viewport coordinates
    void set_center_from_viewport(VC center, double viewport_height)
    {
        const Window &window = *_world.window();
        _center = TVector(window.to_world(window.from_viewport(center, viewport_height)));
    }

    // Render controls of selected objects.
    void render_controls(::Canvas<Coord2D> &canvas)
    {
        const int radius = 2;

        for (auto control: _selected_group.controls())
        {
            render_cross(canvas, *control, radius, CONTROL, CONTROL);
        }
    }

    // Render the center as a little cross.
    void render_center(::Canvas<Coord2D> &canvas)
    {
        if (not_empty())
        {
            const int radius = 2;

            canvas.move(_center);
            canvas.set_color(ORANGE);
            canvas.draw_circle(_center, radius);
        }
    }

    void beforeRendering(const Command &command, Canvas &canvas) override
    {
        canvas.set_color(_selected_group.contains(command.object()) ? (_tool == NONE ? ORANGE : WHITE) : BLACK);
    }

private:

    World &_world;
    Group _selected_group;
    Coord2D _center;
    TransformAxis _transform_axis = Z_ALL;
    Tool _tool = NONE;

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
        _window->set_viewport(*this);

        render_axis();

#ifdef WORLD_2D
        Canvas<Coord2D> *projection_canvas = this;
#endif

#ifdef WORLD_3D
        shared_ptr<ProjectionCanvas<Coord3D>> projection_canvas;
        if (projection_method == ProjectionMethod::PARALLEL)
        {
            projection_canvas = make_shared<ParallelProjection>(*this);
        }
        else
        {
            projection_canvas = make_shared<PerspectiveProjection>(
                *this,
                Coord3D(_window->center().x(), _window->center().y(), 0)
            );
        }
#endif

        printf("Render display file: started\n");
        const clock_t start = clock();
        display_file.render(*projection_canvas, selection);
        const double time = elapsed_secs(start);
        printf("Render display file: finished (t = %9.6lf)\n", time);

#ifdef WORLD_2D
        selection.render_controls(*projection_canvas);
#endif

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

    // Move to destination.
    void move(const Coord2D &destination) override
    {
        _canvas.move(_window->world_to_viewport(destination));
    }

    // Draw line from current position to destination.
    void draw_line(const Coord2D &destination) override
    {
        _canvas.draw_line(_window->world_to_viewport(destination));
    }

    // Draw circle with the specified center, radius and color.
    void draw_circle(const Coord2D &center, const double radius) override
    {
        _canvas.draw_circle(_window->world_to_viewport(center), radius);
    }

    // Set the color to be used when drawing.
    void set_color(const Color &color) override
    {
        _canvas.set_color(color);
    }

private:

    // Render the x axis and y axis.
    void render_axis()
    {
        const Coord2D center = Coord2D(0, 0);
        const int radius = 10000;

        render_cross(*this, center, radius, RED, GREEN);
    }

    shared_ptr<Window> _window;
    Canvas<VC> &_canvas;

};

