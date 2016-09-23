#pragma once

#include "graphics.h"
#include <cairo.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#define UNUSED __attribute__ ((unused))

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
    void clear(const Color &color)
    {
        cairo_set_source_rgb(cr, color.red(), color.green(), color.blue());
        cairo_paint(cr);
    }

    // Move to destination.
    virtual void move(const Coord &destination)
    {
        cairo_move_to(cr, destination.x(), destination.y());
    }

    // Draw line from current position to destination.
    virtual void draw_line(const Coord &destination, const Color &color)
    {
        cairo_set_source_rgb(cr, color.red(), color.green(), color.blue());
        cairo_set_line_width(cr, 1);
        cairo_line_to(cr, destination.x(), destination.y());
        cairo_stroke(cr);
    }

    // Draw circle with the specified center, radius and color.
    virtual void draw_circle(const Coord &center, const double radius, const Color &color)
    {
        cairo_set_source_rgb(cr, color.red(), color.green(), color.blue());
        cairo_set_line_width(cr, 1);
        cairo_arc(cr, center.x(), center.y(), radius, 0, 2 * PI);
        cairo_stroke_preserve(cr);
        cairo_fill(cr);
    }

private:
    cairo_t *cr;
};

static void refresh(GtkWidget *widget)
{
    gtk_widget_queue_draw_area(
        widget,
        0, 0,
        gtk_widget_get_allocated_width(widget),
        gtk_widget_get_allocated_height(widget));
}

static cairo_surface_t *surface = NULL;

static gboolean refresh_surface(GtkWidget *widget, GdkEventConfigure UNUSED *event, gpointer data)
{
    if (surface) cairo_surface_destroy(surface);

    const int widget_width = gtk_widget_get_allocated_width(widget);
    const int widget_height = gtk_widget_get_allocated_height(widget);

    surface = gdk_window_create_similar_surface(gtk_widget_get_window(widget),
                                                CAIRO_CONTENT_COLOR,
                                                widget_width, widget_height);

    SurfaceCanvas canvas(surface);
    canvas.clear(Color(1, 1, 1));

    World &world = *(World*)data;
    ViewportCanvas viewport(widget_width, widget_height, world.window(), canvas);
    world.render(viewport);

    refresh(widget);

    return true;
}

static void refresh_canvas(GtkWidget *canvas, World &world)
{
    refresh_surface(GTK_WIDGET(canvas), nullptr, &world);
}

constexpr double PADDING = 5;

static gboolean draw_canvas(GtkWidget *widget, cairo_t *cr, gpointer UNUSED data)
{
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);

    if (gtk_widget_has_focus(widget))
        gtk_render_focus(
            gtk_widget_get_style_context(widget),
            cr,
            PADDING, PADDING,
            gtk_widget_get_allocated_width(widget) - 2 * PADDING,
            gtk_widget_get_allocated_height(widget) - 2 * PADDING);

    return false;
}

static gboolean canvas_button_press_event(GtkWidget *canvas, GdkEventButton *event, gpointer data)
{
    if (event->button == 1) {
        gtk_widget_grab_focus(canvas);

        const Coord new_center = Coord(event->x, event->y);
        const int widget_width = gtk_widget_get_allocated_width(canvas);
        const int widget_height = gtk_widget_get_allocated_height(canvas);
        const Viewport viewport(widget_width, widget_height);

        World &world = *(World*)data;
        world.set_center_from_viewport(new_center, viewport);

        refresh_canvas(canvas, world);
    }

    return true;
}

static const double step = 0.1; // 10 percent

static void add_objects_to_list_box(GtkListBox *list_box, vector<shared_ptr<Object>> objects) {
    for (auto &object: objects) {
        GtkWidget *label = gtk_label_new(object->name().c_str());

        gtk_container_add(GTK_CONTAINER(list_box), label);
    }
}

static const int gtk_window__width = 600;
static const int gtk_window__height = 480;

static const gint pan_column__menu_bar = 7;
static const gint pan_column__canvas = 7;
static const gint pan_column__list_box = 2;
static const gint pan_column__button = 1;

static const gint pan_row__menu_bar = 1;
static const gint pan_row__canvas = 20;
static const gint pan_row__list_box = pan_row__canvas;
static const gint pan_row__button = 2;

static const gint column__menu_bar = 0;
static const gint column__tool_bar = column__menu_bar;
static const gint column__list_box = column__menu_bar;
static const gint column__canvas = column__list_box + pan_column__list_box;

static const gint row__menu_bar = 0;
static const gint row__tool_bar = row__menu_bar + pan_row__menu_bar;
static const gint row__list_box = row__tool_bar + pan_row__button;
static const gint row__canvas = row__list_box;

static GtkWidget * new_gtk_window(const gchar *title)
{
    GtkWidget *gtk_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_position(GTK_WINDOW(gtk_window), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(gtk_window), title);
    gtk_window_set_default_size(GTK_WINDOW(gtk_window), gtk_window__width, gtk_window__height);
    g_signal_connect(gtk_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    return gtk_window;
}

static GtkWidget * new_grid(GtkWidget *gtk_window)
{
    GtkWidget *grid = gtk_grid_new();

    gtk_grid_set_column_homogeneous(GTK_GRID(grid), true);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), true);
    gtk_container_add(GTK_CONTAINER(gtk_window), grid);

    return grid;
}

static GtkWidget * new_canvas(GtkWidget *grid, World &world, GCallback on_key_press)
{
    GtkWidget *canvas = gtk_drawing_area_new();

    gtk_grid_attach(GTK_GRID(grid), canvas,
                    column__canvas, row__canvas,
                    pan_column__canvas, pan_row__canvas);
    g_signal_connect(canvas, "configure-event", G_CALLBACK(refresh_surface), &world);
    g_signal_connect(canvas, "draw", G_CALLBACK(draw_canvas), nullptr);
    g_signal_connect(canvas, "button-press-event", G_CALLBACK(canvas_button_press_event), &world);
    g_signal_connect(canvas, "key-press-event", on_key_press, &world);

    gtk_widget_set_events(canvas, GDK_BUTTON_PRESS_MASK);
    gtk_widget_set_can_focus(canvas, true);

    return canvas;
}

static void new_list_box(GtkWidget *grid, GtkWidget *canvas, World &world, GCallback select_object)
{
    GtkWidget *list_box = gtk_list_box_new();
    add_objects_to_list_box(GTK_LIST_BOX(list_box), world.objects());
    gtk_grid_attach(GTK_GRID(grid), list_box,
                    column__list_box, row__list_box,
                    pan_column__list_box, pan_row__list_box);

    g_signal_connect(GTK_LIST_BOX(list_box), "row-selected", select_object, canvas);
}

static GtkWidget * new_button(
    GtkWidget *grid, GtkWidget *canvas, const gchar *label, bool enabled, GCallback callback, string tooltip)
{
    static gint button_count = 0;

    GtkWidget *button_with_label = gtk_button_new_with_label(label);
    gtk_widget_set_sensitive(GTK_WIDGET(button_with_label), enabled);
    gtk_widget_set_tooltip_text(GTK_WIDGET(button_with_label), tooltip.c_str());
    g_signal_connect(button_with_label, "clicked", G_CALLBACK(callback), canvas);
    gtk_grid_attach(GTK_GRID(grid), button_with_label,
                    column__tool_bar + (button_count++), row__tool_bar,
                    pan_column__button, pan_row__button);

    return button_with_label;
}

static void select_cs(GtkWidget UNUSED *menu_item)
{
    clipping_method = ClippingMethod::COHEN_SUTHERLAND;
}

static void select_lb(GtkWidget UNUSED *menu_item)
{
    clipping_method = ClippingMethod::LIANG_BARSKY;
}

static GSList *line_clipping_group = nullptr;

static void menu_item(const GtkWidget *menu, const gchar *label, GCallback callback)
{
    GtkWidget *menu_item = gtk_radio_menu_item_new_with_label(line_clipping_group, label);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect(G_OBJECT(menu_item), "activate", callback, NULL);

    if (line_clipping_group == nullptr)
    {
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu_item), TRUE);
        line_clipping_group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(menu_item));
    }
}

static void line_clipping_menu(GtkWidget * menu_bar)
{
    GtkWidget *menu = gtk_menu_new();
    GtkWidget *top_item = gtk_menu_item_new_with_label("Line Clipping");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(top_item), menu);

    menu_item(menu, "Cohen-Sutherland", G_CALLBACK(select_cs));
    menu_item(menu, "Liang-Barsky", G_CALLBACK(select_lb));

    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), top_item);
}

static void menu_bar(GtkWidget *grid)
{
    GtkWidget *menu_bar = gtk_menu_bar_new();

    line_clipping_menu(menu_bar);

    gtk_grid_attach(GTK_GRID(grid), menu_bar,
                    column__menu_bar, row__menu_bar,
                    pan_column__menu_bar, pan_row__menu_bar);
}
