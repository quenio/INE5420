#include <limits.h>
#include <cairo.h>
#include <gtk/gtk.h>

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

static gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer __unused user_data)
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

static gboolean canvas_configure_event(GtkWidget *widget, GdkEventConfigure __unused *event, gpointer __unused data)
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

static gboolean canvas_draw(GtkWidget __unused *widget, cairo_t *cr, gpointer __unused data)
{
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);

    return false;
}

int main(int argc, char *argv[])
{
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