#pragma once

#include "tools.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdocumentation-unknown-command"
#pragma GCC diagnostic ignored "-Wdocumentation"
#pragma GCC diagnostic ignored "-Wreserved-id-macro"
#pragma GCC diagnostic ignored "-Wshift-sign-overflow"
#pragma GCC diagnostic ignored "-Wdeprecated-register"

#include <cairo.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#pragma GCC diagnostic pop

#define UNUSED __attribute__ ((unused))

#ifdef WORLD_2D
using UserSelection = Selection<Coord2D>;
using UserViewport = ViewportCanvas<Coord2D>;
#else
using UserSelection = Selection<Coord3D>;
using UserViewport = ViewportCanvas<Coord3D>;
#endif

// Canvas for GTK surface
class SurfaceCanvas: public Canvas<VC>
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
    void clear(double width, double height)
    {
        const Color border_color = LIGHT_GRAY;
        cairo_set_source_rgb(cr, border_color.red(), border_color.green(), border_color.blue());
        cairo_paint(cr);

        const Color background_color = DARK_GRAY;
        cairo_set_source_rgb(cr, background_color.red(), background_color.green(), background_color.blue());

        const double margin = width * Viewport::margin_percentage;
        cairo_rectangle(
            cr,
            margin, margin,
            width - 2 * margin, height - 2 * margin);
        cairo_stroke_preserve(cr);
        cairo_fill(cr);
    }

    // Move to destination.
    void move(const VC &destination) override
    {
        cairo_move_to(cr, destination.x(), destination.y());
    }

    // Draw line from current position to destination.
    void draw_line(const VC &destination) override
    {
        cairo_set_line_width(cr, 1);
        cairo_line_to(cr, destination.x(), destination.y());
        cairo_stroke(cr);
    }

    // Draw circle with the specified center, radius and color.
    void draw_circle(const VC &center, const double radius) override
    {
        cairo_set_line_width(cr, 1);
        cairo_arc(cr, center.x(), center.y(), radius, 0, 2 * PI);
        cairo_stroke_preserve(cr);
        cairo_fill(cr);
    }

    // Set the color to be used when drawing.
    void set_color(const Color &color) override
    {
        cairo_set_source_rgb(cr, color.red(), color.green(), color.blue());
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
    canvas.clear(widget_width, widget_height);

    UserSelection &selection = *(UserSelection*)data;
    UserViewport viewport(widget_width, widget_height, selection.window(), canvas);
    viewport.render(selection.display_file(), selection);

    refresh(widget);

    return true;
}

static void refresh_canvas(GtkWidget *canvas, UserSelection &selection)
{
    refresh_surface(GTK_WIDGET(canvas), nullptr, &selection);
}

static gboolean draw_canvas(GtkWidget *widget, cairo_t *cr, gpointer UNUSED data)
{
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);

    if (gtk_widget_has_focus(widget))
    {
        const int widget_width = gtk_widget_get_allocated_width(widget);
        const int widget_height = gtk_widget_get_allocated_height(widget);
        const double padding = Viewport::margin_percentage * widget_width - 10;
        gtk_render_focus(
            gtk_widget_get_style_context(widget),
            cr,
            padding, padding,
            widget_width - 2 * padding,
            widget_height - 2 * padding);
    }

    return false;
}

static gboolean canvas_button_press_event(GtkWidget *canvas, GdkEventButton *event, gpointer data)
{
    gtk_widget_grab_focus(canvas);

    UserSelection &selection = *(UserSelection*)data;

    if ((event->button == 1) && (selection.tool() == ROTATE))
    {
        const VC new_center = { event->x, event->y };
        const int widget_width = gtk_widget_get_allocated_width(canvas);
        const int widget_height = gtk_widget_get_allocated_height(canvas);
        const Viewport viewport(widget_width, widget_height);

        selection.set_center_from_viewport(new_center, viewport.height());
    }
    else
    {
        selection.select_tool(NONE);
    }

    refresh_canvas(canvas, selection);

    return true;
}

static const double step = 0.1; // 10 percent

template<class Coord>
static void add_objects_to_list_box(GtkListBox *list_box, vector<shared_ptr<Object<Coord>>> objects) {
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(list_box));
    for(iter = children; iter != NULL; iter = g_list_next(iter))
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    g_list_free(children);

    for (auto &object: objects) {
        GtkWidget *label = gtk_label_new(object->name().c_str());

        gtk_container_add(GTK_CONTAINER(list_box), label);
        gtk_widget_show(label);
    }
}

static const int gtk_window__width = 600;
static const int gtk_window__height = 480;

static const gint pan_column__button = 8;
static const gint pan_column__canvas = 8 * pan_column__button;
static const gint pan_column__list_label = 2 * pan_column__button + 2;
static const gint pan_column__list_box = pan_column__list_label - 1;
static const gint pan_column__menu_bar = pan_column__canvas + pan_column__list_label;

static const gint pan_row__menu_bar = 1;
static const gint pan_row__canvas = 18;
static const gint pan_row__list_label = 1;
static const gint pan_row__list_box = pan_row__canvas - 1;
static const gint pan_row__button = 1;

static const gint column__menu_bar = 0;
static const gint column__tool_bar = column__menu_bar;
static const gint column__canvas = column__menu_bar;
static const gint column__list_label = column__menu_bar + pan_column__canvas;
static const gint column__list_box = column__list_label;

static const gint row__menu_bar = 0;
static const gint row__canvas = row__menu_bar + pan_row__menu_bar;
static const gint row__list_label = row__canvas;
static const gint row__list_box = row__list_label + pan_row__list_label;
static const gint row__tool_bar = row__list_box + pan_row__list_box;

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

    GdkRGBA light_gray = { LIGHT_GRAY.red(), LIGHT_GRAY.green(), LIGHT_GRAY.blue(), 1 };
    gtk_widget_override_background_color(grid, GTK_STATE_FLAG_NORMAL, &light_gray);

    return grid;
}

static GtkWidget * new_canvas(
    GtkWidget *grid, UserSelection &selection, GCallback on_key_press, GCallback on_scroll, GCallback on_motion)
{
    GtkWidget *canvas = gtk_drawing_area_new();

    gtk_grid_attach(GTK_GRID(grid), canvas,
                    column__canvas, row__canvas,
                    pan_column__canvas, pan_row__canvas);
    g_signal_connect(canvas, "configure-event", G_CALLBACK(refresh_surface), &selection);
    g_signal_connect(canvas, "draw", G_CALLBACK(draw_canvas), nullptr);
    g_signal_connect(canvas, "button-press-event", G_CALLBACK(canvas_button_press_event), &selection);
    g_signal_connect(canvas, "key-press-event", on_key_press, nullptr);
    g_signal_connect(canvas, "scroll-event", G_CALLBACK(on_scroll), nullptr);
    g_signal_connect(canvas, "motion-notify-event", G_CALLBACK(on_motion), nullptr);

    gtk_widget_set_events(canvas, GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK | GDK_SCROLL_MASK);
    gtk_widget_set_can_focus(canvas, true);

    return canvas;
}

static GtkListBox * new_list_box(GtkWidget *grid, GtkWidget *canvas, UserSelection &selection, GCallback select_object)
{
    GtkListBox *list_box = GTK_LIST_BOX(gtk_list_box_new());
    GdkRGBA light_gray = { LIGHT_GRAY.red(), LIGHT_GRAY.green(), LIGHT_GRAY.blue(), 1 };
    gtk_widget_override_background_color(GTK_WIDGET(list_box), GTK_STATE_FLAG_NORMAL, &light_gray);
    add_objects_to_list_box(list_box, selection.world().objects());
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(list_box),
                    column__list_box, row__list_box,
                    pan_column__list_box, pan_row__list_box);

    g_signal_connect(list_box, "row-selected", select_object, canvas);

    gtk_list_box_set_selection_mode(list_box, GTK_SELECTION_MULTIPLE);

    return list_box;
}

static GtkWidget * new_button(
    GtkWidget *grid, GtkWidget *canvas, const gchar *label, bool enabled, GCallback callback, string tooltip,
    bool separator, bool small)
{
    static gint current_button_column_pan = 0;

    GtkWidget *button_with_label = gtk_button_new_with_label(label);
    gtk_widget_set_sensitive(GTK_WIDGET(button_with_label), enabled);
    gtk_widget_set_tooltip_text(GTK_WIDGET(button_with_label), tooltip.c_str());
    g_signal_connect(button_with_label, "clicked", G_CALLBACK(callback), canvas);
    gtk_grid_attach(GTK_GRID(grid), button_with_label,
                    column__tool_bar + current_button_column_pan, row__tool_bar,
                    pan_column__button - (small ? 3 : 0), pan_row__button);

    current_button_column_pan += pan_column__button - (small ? 3 : 0);

    if (separator) current_button_column_pan += 2;

    return button_with_label;
}

static GtkWidget * new_menu_item(const GtkWidget *menu, const gchar *label, GCallback callback, GtkWidget *canvas, GSList* gsList)
{
    GtkWidget *menu_item = gtk_radio_menu_item_new_with_label(gsList, label);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect(G_OBJECT(menu_item), "activate", callback, canvas);

    return menu_item;
}

static void menu_bar_attach(GtkWidget* menu_bar, GtkWidget* canvas, string menu_name, list<pair<string, GCallback>> menu_items)
{
    GtkWidget *menu = gtk_menu_new();
    GtkWidget *top_item = gtk_menu_item_new_with_label(menu_name.c_str());

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(top_item), menu);

    GSList *gsList = nullptr;
    for (auto &item : menu_items)
    {
        GtkWidget* element = new_menu_item(menu, item.first.c_str(), item.second, canvas, gsList);

        if (gsList == nullptr)
        {
            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(element), TRUE);
            gsList = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(element));
        }
    }

    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), top_item);
}

static GtkWidget * new_menu_bar(GtkWidget *grid)
{
    GtkWidget *menu_bar = gtk_menu_bar_new();

    GdkRGBA lighter_gray = { LIGHTER_GRAY.red(), LIGHTER_GRAY.green(), LIGHTER_GRAY.blue(), 1 };
    gtk_widget_override_background_color(menu_bar, GTK_STATE_FLAG_NORMAL, &lighter_gray);

    gtk_grid_attach(GTK_GRID(grid), menu_bar,
                    column__menu_bar, row__menu_bar,
                    pan_column__menu_bar, pan_row__menu_bar);

    return menu_bar;
}

static GtkWidget * new_list_label(GtkWidget *grid, const gchar *label)
{
    GtkWidget *widget = gtk_label_new(label);

    GdkRGBA light_gray = { LIGHT_GRAY.red(), LIGHT_GRAY.green(), LIGHT_GRAY.blue(), 1 };
    gtk_widget_override_background_color(widget, GTK_STATE_FLAG_NORMAL, &light_gray);

#ifndef _GRAPHICS_BUILD
    gtk_label_set_xalign(GTK_LABEL(widget), 0.0);
#endif

    gtk_grid_attach(GTK_GRID(grid), widget,
                    column__list_label, row__list_label,
                    pan_column__list_label, pan_row__list_label);

    return widget;
}
