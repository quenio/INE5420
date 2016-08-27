// Copyright (c) 2016 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "graphics.h"
#include <gtk/gtk.h>

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
    canvas.clear();

    World &world = *(World*)data;
    Viewport viewport(widget_width, widget_height, world.window(), canvas);
    world.render(viewport);

    refresh(widget);

    return true;
}

static void refresh_canvas(GtkWidget *canvas, World &world)
{
    refresh_surface(GTK_WIDGET(canvas), nullptr, &world);
}

static gboolean draw_canvas(GtkWidget UNUSED *widget, cairo_t *cr, gpointer UNUSED data)
{
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);

    return false;
}

static const double step = 0.1; // 10 percent

static void add_objects_to_list_box(GtkListBox *list_box, list<shared_ptr<Object>> objects) {
    for (auto &object: objects) {
        GtkWidget *label = gtk_label_new(object->name().c_str());

        gtk_container_add(GTK_CONTAINER(list_box), label);
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

static GtkWidget * new_canvas(GtkWidget *grid, World &world)
{
    GtkWidget *canvas = gtk_drawing_area_new();

    gtk_grid_attach(GTK_GRID(grid), canvas,
                    column__canvas, row__canvas,
                    span_column__canvas, span_row__canvas);
    g_signal_connect(canvas, "configure-event", G_CALLBACK(refresh_surface), &world);
    g_signal_connect(canvas, "draw", G_CALLBACK(draw_canvas), nullptr);

    return canvas;
}

static int select_object(UNUSED GtkListBox *list_box, GtkListBoxRow *row, UNUSED gpointer user_data)
{
    gint i = gtk_list_box_row_get_index(row);

    printf("Selected row = %i\n", i);

    return i;
}

static void new_list_box(GtkWidget *grid, list<shared_ptr<Object>> objects)
{
    GtkWidget *list_box = gtk_list_box_new();
    add_objects_to_list_box(GTK_LIST_BOX(list_box), objects);
    gtk_grid_attach(GTK_GRID(grid), list_box,
                    column__list_box, row__list_box,
                    span_column__list_box, span_row__list_box);

    g_signal_connect(GTK_LIST_BOX(list_box), "row-selected", G_CALLBACK(select_object), nullptr);
}

static void new_button(GtkWidget *grid, GtkWidget *canvas, const gchar *label, GCallback callback)
{
    static gint button_count = 0;

    GtkWidget *button_with_label = gtk_button_new_with_label(label);
    g_signal_connect(button_with_label, "clicked", G_CALLBACK(callback), canvas);
    gtk_grid_attach(GTK_GRID(grid), button_with_label,
                    column__tool_bar + (button_count++), row__tool_bar,
                    span_column__button, span_row__button);
}
