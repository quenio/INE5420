#include <cairo.h>
#include <gtk/gtk.h>
#include <memory>
#include <list>
#include <sstream>

using namespace std;

#define UNUSED __attribute__ ((unused))

class Coord
{
public:
    Coord (double x, double y): _x(x), _y(y) {}

    virtual double x() const { return _x; }
    virtual double y() const { return _y; }

    // Scale x by factor fx, y by factor fy.
    Coord scale(double fx, double fy)
    {
        return Coord(x() * fx, y() * fy);
    }

    // Move by dx horizontally, dy vertically.
    Coord translate(double dx, double dy)
    {
        return Coord(x() + dx, y() + dy);
    }

private:
    double _x;
    double _y;
};

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

        current = current.translate(0, thickness);
        canvas.draw_line(current);
        canvas.move(current);

        current = current.translate(thickness, 0);
        canvas.draw_line(current);
        canvas.move(current);

        current = current.translate(0, -thickness);
        canvas.draw_line(current);
        canvas.move(current);

        current = current.translate(-thickness, 0);
        canvas.draw_line(current);
    }

    virtual string type()
    {
        return "Point";
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

        _leftBottom = _leftBottom.translate(+tx, +ty);
        _rightTop = _rightTop.translate(-tx, -ty);
    }

    // Zoom in by factor
    void zoom_in(double factor)
    {
        double tx = width() * factor;
        double ty = height() * factor;

        _leftBottom = _leftBottom.translate(-tx, -ty);
        _rightTop = _rightTop.translate(+tx, +ty);
    }

    // Span left by factor
    void span_left(double factor)
    {
        double tx = width() * factor;

        _leftBottom = _leftBottom.translate(-tx, 0);
        _rightTop = _rightTop.translate(-tx, 0);
    }

    // Span right by factor
    void span_right(double factor)
    {
        double tx = width() * factor;

        _leftBottom = _leftBottom.translate(+tx, 0);
        _rightTop = _rightTop.translate(+tx, 0);
    }

    // Span up by factor
    void span_up(double factor)
    {
        double ty = height() * factor;

        _leftBottom = _leftBottom.translate(0, +ty);
        _rightTop = _rightTop.translate(0, +ty);
    }

    // Span down by factor
    void span_down(double factor)
    {
        double ty = height() * factor;

        _leftBottom = _leftBottom.translate(0, -ty);
        _rightTop = _rightTop.translate(0, -ty);
    }

private:
    Coord _leftBottom, _rightTop;
};

// Area on a screen to execute display commands
class Viewport: public Canvas
{
public:

    Viewport(double width, double height, Window window, Canvas &canvas)
        : _width(width), _height(height), _window(window), _canvas(canvas) {}

    // Translate p from window to canvas.
    Coord translate(const Coord &p) const
    {
        return _window.normalize(p).scale(_width, _height);
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
    Window _window;
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


    void render(Viewport &viewport)
    {
        for (auto &command: _commands) command->render(viewport);
    }

    // Objects from command list
    list<shared_ptr<Object>> objects() {
        list<shared_ptr<Object>> list;

        for (auto &command: _commands)
        {
            shared_ptr<DrawCommand> drawCommand = dynamic_pointer_cast<DrawCommand>(command);
            if (drawCommand && drawCommand->object())
            {
                list.push_back(drawCommand->object());
            }
        }

        return list;
    }

private:
    // Commands to be executed
    list<shared_ptr<DisplayCommand>> _commands;
};

// Canvas for GTK surface
class SurfaceCanvas: public Canvas
{
public:

    SurfaceCanvas(cairo_surface_t *surface)
    {
        cr = cairo_create(surface);
    }

    ~SurfaceCanvas()
    {
        cairo_destroy(cr);
    }

    // Paint the whole canvas with the white color.
    void clear()
    {
        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_paint(cr);
    }

    // Move to destination.
    virtual void move(const Coord &destination)
    {
        cairo_move_to(cr, destination.x(), destination.y());
    }

    // Draw line from current position to destination.
    virtual void draw_line(const Coord &destination)
    {
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_set_line_width(cr, 1);
        cairo_line_to(cr, destination.x(), destination.y());
        cairo_stroke(cr);
    }

private:
    cairo_t *cr;
};


inline shared_ptr<DrawCommand> draw_point(Coord a)
{
    return make_shared<DrawCommand>(make_shared<Point>(a));
}

inline shared_ptr<DrawCommand> draw_line(Coord a, Coord b)
{
    return make_shared<DrawCommand>(make_shared<Line>(a, b));
}

inline shared_ptr<DrawCommand> draw_square(Coord a, Coord b, Coord c, Coord d)
{
    return make_shared<DrawCommand>(make_shared<Polygon>(Polygon { a, b, c, d }));
}

static DisplayFile displayFile({
    draw_point(Coord(25, 50)),
    draw_point(Coord(75, 50)),
    draw_line(Coord(10, 10), Coord(90, 90)),
    draw_square(Coord(10, 10), Coord(10, 90), Coord(90, 90), Coord(90, 10))
});


static Window world_window(0, 0, 100, 100);

static void refresh(GtkWidget *widget)
{
    gtk_widget_queue_draw_area(
        widget,
        0, 0,
        gtk_widget_get_allocated_width(widget),
        gtk_widget_get_allocated_height(widget));
}

static cairo_surface_t *surface = NULL;

static gboolean refresh_surface(GtkWidget *widget, GdkEventConfigure UNUSED *event, gpointer UNUSED data)
{
    if (surface) cairo_surface_destroy(surface);

    const int widget_width = gtk_widget_get_allocated_width(widget);
    const int widget_height = gtk_widget_get_allocated_height(widget);

    surface = gdk_window_create_similar_surface(gtk_widget_get_window(widget),
                                                CAIRO_CONTENT_COLOR,
                                                widget_width, widget_height);

    SurfaceCanvas canvas(surface);
    canvas.clear();

    Viewport viewport(widget_width, widget_height, world_window, canvas);
    displayFile.render(viewport);

    refresh(widget);

    return true;
}

static void refresh_canvas(GtkWidget *canvas)
{
    refresh_surface(GTK_WIDGET(canvas), nullptr, nullptr);
}

static gboolean draw_canvas(GtkWidget UNUSED *widget, cairo_t *cr, gpointer UNUSED data)
{
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);

    return false;
}

static const double step = 0.1; // 10 percent

static void zoom_in_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world_window.zoom_out(step);
    refresh_canvas(GTK_WIDGET(canvas));
}

static void zoom_out_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world_window.zoom_in(step);
    refresh_canvas(GTK_WIDGET(canvas));
}

static void span_left_clicked(GtkWidget UNUSED *widget, gpointer UNUSED canvas)
{
    world_window.span_left(step);
    refresh_canvas(GTK_WIDGET(canvas));
}

static void span_right_clicked(GtkWidget UNUSED *widget, gpointer UNUSED canvas)
{
    world_window.span_right(step);
    refresh_canvas(GTK_WIDGET(canvas));
}

static void span_up_clicked(GtkWidget UNUSED *widget, gpointer UNUSED canvas)
{
    world_window.span_up(step);
    refresh_canvas(GTK_WIDGET(canvas));
}

static void span_down_clicked(GtkWidget UNUSED *widget, gpointer UNUSED canvas)
{
    world_window.span_down(step);
    refresh_canvas(GTK_WIDGET(canvas));
}

static void add_objects_to_list_box(GtkListBox *list_box) {
    for (auto &object: displayFile.objects()) {
        GtkWidget *label = gtk_label_new(object->name().c_str());
        gtk_list_box_prepend(list_box, label);
    }
}

static const int gtk_window__width = 600;
static const int gtk_window__height = 480;

static const gint span_column__canvas = 6;
static const gint span_column__list_box = 2;
static const gint span_column__button = 1;

static const gint span_row__canvas = 6;
static const gint span_row__list_box = span_row__canvas;
static const gint span_row__button = 1;

static const gint column__tool_bar = 0;
static const gint column__list_box = column__tool_bar;
static const gint column__canvas = column__list_box + span_column__list_box;

static const gint row__tool_bar = 0;
static const gint row__list_box = 1;
static const gint row__canvas = row__list_box;

static GtkWidget *gtk_window;
static GtkWidget *grid;
static GtkWidget *canvas;

static void new_gtk_window(const gchar *title)
{
    gtk_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(gtk_window), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(gtk_window), title);
    gtk_window_set_default_size(GTK_WINDOW(gtk_window), gtk_window__width, gtk_window__height);
    g_signal_connect(gtk_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}

static void new_grid()
{
    grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), true);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), true);
    gtk_container_add(GTK_CONTAINER(gtk_window), grid);
}

static void new_canvas()
{
    canvas = gtk_drawing_area_new();
    gtk_grid_attach(GTK_GRID(grid), canvas,
                    column__canvas, row__canvas,
                    span_column__canvas, span_row__canvas);
    g_signal_connect(canvas, "configure-event", G_CALLBACK(refresh_surface), NULL);
    g_signal_connect(canvas, "draw", G_CALLBACK(draw_canvas), NULL);
}

static void new_list_box()
{
    GtkWidget *list_box = gtk_list_box_new();
    add_objects_to_list_box(GTK_LIST_BOX(list_box));
    gtk_grid_attach(GTK_GRID(grid), list_box,
                    column__list_box, row__list_box,
                    span_column__list_box, span_row__list_box);
}

static void new_button(const gchar *label, GCallback callback)
{
    static gint button_count = 0;
    GtkWidget *button_with_label = gtk_button_new_with_label(label);
    g_signal_connect(button_with_label, "clicked", G_CALLBACK(callback), canvas);
    gtk_grid_attach(GTK_GRID(grid), button_with_label,
                    column__tool_bar + (button_count++), row__tool_bar,
                    span_column__button, span_row__button);
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    new_gtk_window("Graphics");
    new_grid();
    new_canvas();
    new_list_box();

    new_button("Zoom In", G_CALLBACK(zoom_in_clicked));
    new_button("Zoom Out", G_CALLBACK(zoom_out_clicked));
    new_button(" < ", G_CALLBACK(span_left_clicked));
    new_button(" > ", G_CALLBACK(span_right_clicked));
    new_button(" Up ", G_CALLBACK(span_up_clicked));
    new_button(" Down ", G_CALLBACK(span_down_clicked));

    gtk_widget_show_all(gtk_window);
    gtk_main();

    return 0;
}