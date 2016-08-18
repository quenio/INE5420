#include <cairo.h>
#include <gtk/gtk.h>
#include <memory>
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

    // Move by dx horizontally, dy vertically.
    Point translate(double dx, double dy)
    {
        return Point(_x += dx, _y += dy);
    }

    // Draw a point in canvas at position (x, y).
    void draw(Canvas &canvas)
    {
        const double thickness = 0.3;

        Point current = *this;
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
        Point previous = _vertices.back();
        for (auto &current: _vertices)
        {
            canvas.move(previous);
            canvas.draw_line(current);
            previous = current;
        }
    }

private:
    list<Point> _vertices;
};


// Visible area of the world
class Window
{
public:

    Window(double left, double bottom, double right, double top)
        :_leftBottom(left, bottom), _rightTop(right, top) {}

    double left() { return _leftBottom.x(); }
    double bottom() { return _leftBottom.y(); }
    double right() { return _rightTop.x(); }
    double top() { return _rightTop.y(); }

    double width() { return right() - left(); }
    double height() { return top() - bottom(); }

    // Normalize point to window dimensions
    Point normalize(Point point)
    {
        return Point(
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
    Point _leftBottom, _rightTop;
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

    DrawCommand(shared_ptr<Drawable> drawable): _drawable(drawable) {}

    // Applies drawable to the viewport.
    virtual void render(Viewport &viewport)
    {
        _drawable->draw(viewport);
    }

private:
    shared_ptr<Drawable> _drawable;
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
    virtual void move(Point destination)
    {
        cairo_move_to(cr, destination.x(), destination.y());
    }

    // Draw line from current position to destination.
    virtual void draw_line(Point destination)
    {
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_set_line_width(cr, 1);
        cairo_line_to(cr, destination.x(), destination.y());
        cairo_stroke(cr);
    }

private:
    cairo_t *cr;
};


inline shared_ptr<DrawCommand> draw_point(Point a)
{
    return make_shared<DrawCommand>(make_shared<Point>(a));
}

inline shared_ptr<DrawCommand> draw_line(Point a, Point b)
{
    return make_shared<DrawCommand>(make_shared<Line>(a, b));
}

inline shared_ptr<DrawCommand> draw_square(Point a, Point b, Point c, Point d)
{
    return make_shared<DrawCommand>(make_shared<Polygon>(Polygon { a, b, c, d }));
}

static DisplayFile displayFile({
    draw_point(Point(25, 50)),
    draw_point(Point(75, 50)),
    draw_line(Point(10, 10), Point(90, 90)),
    draw_square(Point(10, 10), Point(10, 90), Point(90, 90), Point(90, 10))
});

static Window window(0, 0, 100, 100);

static cairo_surface_t *surface = NULL;

static void refresh(GtkWidget *widget)
{
    gtk_widget_queue_draw_area(
        widget,
        0, 0,
        gtk_widget_get_allocated_width(widget),
        gtk_widget_get_allocated_height(widget));
}

static gboolean refresh_surface(GtkWidget *widget, GdkEventConfigure UNUSED *event, gpointer UNUSED data)
{
    if (surface) cairo_surface_destroy(surface);

    const int width = gtk_widget_get_allocated_width(widget);
    const int height = gtk_widget_get_allocated_height(widget);

    surface = gdk_window_create_similar_surface(gtk_widget_get_window(widget), CAIRO_CONTENT_COLOR, width, height);

    SurfaceCanvas canvas(surface);
    canvas.clear();

    Viewport viewport(width, height, window, canvas);
    displayFile.render(viewport);

    refresh(widget);

    return true;
}

static void refresh_canvas(GtkWidget *canvas)
{
    refresh_surface(GTK_WIDGET(canvas), nullptr, nullptr);
}

static gboolean canvas_draw(GtkWidget UNUSED *widget, cairo_t *cr, gpointer UNUSED data)
{
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);

    return false;
}

static const double step = 0.1; // 10 percent

static void zoom_in_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    window.zoom_out(step);
    refresh_canvas(GTK_WIDGET(canvas));
}

static void zoom_out_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    window.zoom_in(step);
    refresh_canvas(GTK_WIDGET(canvas));
}

static void span_left_clicked(GtkWidget UNUSED *widget, gpointer UNUSED canvas)
{
    window.span_left(step);
    refresh_canvas(GTK_WIDGET(canvas));
}

static void span_right_clicked(GtkWidget UNUSED *widget, gpointer UNUSED canvas)
{
    window.span_right(step);
    refresh_canvas(GTK_WIDGET(canvas));
}

static void span_up_clicked(GtkWidget UNUSED *widget, gpointer UNUSED canvas)
{
    window.span_up(step);
    refresh_canvas(GTK_WIDGET(canvas));
}

static void span_down_clicked(GtkWidget UNUSED *widget, gpointer UNUSED canvas)
{
    window.span_down(step);
    refresh_canvas(GTK_WIDGET(canvas));
}

static const gint max_column_span = 6;
static const gint max_row_span = 6;

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), true);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), true);
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *zoom_in = gtk_button_new_with_label("Zoom In");
    gtk_grid_attach(GTK_GRID(grid), zoom_in, 0, 0, 1, 1);

    GtkWidget *zoom_out = gtk_button_new_with_label("Zoom Out");
    gtk_grid_attach(GTK_GRID(grid), zoom_out, 1, 0, 1, 1);

    GtkWidget *span_left = gtk_button_new_with_label(" < ");
    gtk_grid_attach(GTK_GRID(grid), span_left, 2, 0, 1, 1);

    GtkWidget *span_right = gtk_button_new_with_label(" > ");
    gtk_grid_attach(GTK_GRID(grid), span_right, 3, 0, 1, 1);

    GtkWidget *span_up = gtk_button_new_with_label(" Up ");
    gtk_grid_attach(GTK_GRID(grid), span_up, 4, 0, 1, 1);

    GtkWidget *span_down = gtk_button_new_with_label(" Down ");
    gtk_grid_attach(GTK_GRID(grid), span_down, 5, 0, 1, 1);


    GtkWidget *listbox = gtk_list_box_new();
    gtk_grid_attach(GTK_GRID(grid), listbox, 0, 1, 2, 5);


    GtkWidget *canvas = gtk_drawing_area_new();
    gtk_grid_attach(GTK_GRID(grid), canvas, 2, 1, max_column_span, max_row_span);
    g_signal_connect(canvas, "configure-event", G_CALLBACK(refresh_surface), NULL);
    g_signal_connect(canvas, "draw", G_CALLBACK(canvas_draw), NULL);

    g_signal_connect(zoom_in, "clicked", G_CALLBACK(zoom_in_clicked), canvas);
    g_signal_connect(zoom_out, "clicked", G_CALLBACK(zoom_out_clicked), canvas);
    g_signal_connect(span_left, "clicked", G_CALLBACK(span_left_clicked), canvas);
    g_signal_connect(span_right, "clicked", G_CALLBACK(span_right_clicked), canvas);
    g_signal_connect(span_up, "clicked", G_CALLBACK(span_up_clicked), canvas);
    g_signal_connect(span_down, "clicked", G_CALLBACK(span_down_clicked), canvas);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window), "Lines");

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}