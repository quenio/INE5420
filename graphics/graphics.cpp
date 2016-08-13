#include <cairo.h>
#include <gtk/gtk.h>
#include <list>

using namespace std;

#define UNUSED __attribute__ ((unused))

class Point;

// Drawable area of the screen
class Canvas
{
public:

    // Move to destination.
    virtual void move(Point destination) = 0;

    // Draw line from current position to destination.
    virtual void draw_line(Point destination) = 0;

};

// Drawable objects
class Drawable {
public:

    // Draw something in canvas.
    virtual void draw(Canvas &canvas) = 0;

};

// Two-dimensional points
class Point: public Drawable
{
public:
    Point(double x, double y): _x(x), _y(y) {}

    double x() { return _x; }
    double y() { return _y; }

    // Scale x by factor fx, y by factor fy.
    Point scale(double fx, double fy)
    {
        return Point(_x *= fx, _y *= fy);
    }

    // Draw a point in canvas at position (x, y).
    void draw(Canvas &canvas)
    {
        canvas.move(*this);
        canvas.draw_line(*this);
    }

private:
    double _x, _y;
};

// Straight one-dimensional figure delimited by two points
class Line: public Drawable
{
public:

    Line(Point a, Point b): _a(a), _b(b) {}

    // Draw line in canvas.
    void draw(Canvas &canvas)
    {
        canvas.move(_a);
        canvas.draw_line(_b);
    }

private:
    Point _a, _b;
};

// Plane figure bound by a set of lines - the sides - meeting in a set of points - the vertices
class Polygon: public Drawable
{
public:

    Polygon(initializer_list<Point> vertices): _vertices(vertices) {}

    void draw(Canvas &canvas)
    {
        canvas.move(*_vertices.begin());
        for (const auto &p: _vertices) canvas.draw_line(p);
    }

private:
    list<Point> _vertices;
};


// Visible area of the world
class Window
{
public:

    Window(double left, double right, double top, double bottom)
        :_left(left), _right(right), _top(top), _bottom(bottom) {}

    Point normalize(Point point)
    {
        return Point(
            (point.x() - _left) / (_right - _left),
            1.0 - ((point.y() - _bottom) / (_top - _bottom))
        );
    }

private:
    double _left, _right, _top, _bottom;
};

// Area on a screen to execute display commands
class Viewport: public Canvas
{
public:

    Viewport(double width, double height, Window window, Canvas &canvas)
        : _width(width), _height(height), _window(window), _canvas(canvas) {}

    // Translate p from window to canvas.
    Point translate(Point p)
    {
        return _window.normalize(p).scale(_width, _height);
    }


    // Move to destination.
    virtual void move(Point destination)
    {
        _canvas.move(translate(destination));
    }

    // Draw line from current position to destination.
    virtual void draw_line(Point destination)
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

// List of commands to be executed in order to display an output image
class DisplayFile
{
public:

    DisplayFile(initializer_list<shared_ptr<DisplayCommand>> commands): _commands(commands) {}


    void render(Viewport &viewport)
    {
        for (auto &c: _commands) c->render(viewport);
    }

private:
    // Commands to be executed
    list<shared_ptr<DisplayCommand>> _commands;
};

// Commands to draw objects
class DrawCommand: public DisplayCommand
{
public:

    DrawCommand(Drawable &drawable): _drawable(drawable) {}

    // Applies drawable to the viewport.
    virtual void render(Viewport &viewport)
    {
        _drawable.draw(viewport);
    }

private:
    Drawable &_drawable;
};

static cairo_surface_t *surface = NULL;

struct {
    int count;
    double x[100];
    double y[100];
} glob;

static void do_drawing()
{
    if (surface == NULL) return;

    cairo_t *cr;
    cr = cairo_create(surface);

    cairo_set_line_width(cr, 1);

    int i, j;
    for (i = 0; i <= glob.count - 1; i++ ) {
        for (j = 0; j <= glob.count - 1; j++ ) {
            cairo_move_to(cr, glob.x[i], glob.y[i]);
            cairo_line_to(cr, glob.x[j], glob.y[j]);
        }
    }

    cairo_stroke(cr);
    cairo_destroy (cr);
}

static gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer UNUSED user_data)
{
    glob.x[glob.count] = event->x;
    glob.y[glob.count++] = event->y;

    printf("clicked (%f, %f)\n", event->x, event->y);

    do_drawing();
    gtk_widget_queue_draw_area(
        widget,
        0, 0,
        gtk_widget_get_allocated_width(widget),
        gtk_widget_get_allocated_height(widget));

    return true;
}

static void clear_surface()
{
    cairo_t *cr;

    cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    cairo_destroy(cr);
}

static gboolean canvas_configure_event(GtkWidget *widget, GdkEventConfigure UNUSED *event, gpointer UNUSED data)
{
    if (surface) cairo_surface_destroy(surface);

    surface = gdk_window_create_similar_surface(
            gtk_widget_get_window(widget),
            CAIRO_CONTENT_COLOR,
            gtk_widget_get_allocated_width(widget),
            gtk_widget_get_allocated_height(widget));

    clear_surface();

    glob.count = 0;

    printf("canvas_configure_event\n");

    return true;
}

static gboolean canvas_draw(GtkWidget UNUSED *widget, cairo_t *cr, gpointer UNUSED data)
{
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);

    return false;
}

int main(int argc, char *argv[])
{
//    DisplayFile displayFile({
//        make_shared<DisplayCommand>(Line(Point(10, 10), Point(100, 100)))
//    });

    glob.count = 0;

    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(window, "button-press-event", G_CALLBACK(clicked), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *canvas = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), canvas);
    g_signal_connect(canvas, "configure-event", G_CALLBACK(canvas_configure_event), NULL);
    g_signal_connect(canvas, "draw", G_CALLBACK(canvas_draw), NULL);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    gtk_window_set_title(GTK_WINDOW(window), "Lines");

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}