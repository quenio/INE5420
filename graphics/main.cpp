#include "ui.h"

static World world(
    make_shared<Window>(-20, -20, 120, 120),
    DisplayFile({
        draw_point(Coord(25, 50)),
        draw_point(Coord(75, 50)),
        draw_line(Coord(10, 10), Coord(90, 90)),
        draw_square(Coord(10, 10), Coord(10, 90), Coord(90, 90), Coord(90, 10))
    })
);

static void zoom_in_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world.window()->zoom_in(step);
    refresh_canvas(GTK_WIDGET(canvas), world);
}

static void zoom_out_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world.window()->zoom_out(step);
    refresh_canvas(GTK_WIDGET(canvas), world);
}

static void pan_left_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world.window()->pan_left(step);
    refresh_canvas(GTK_WIDGET(canvas), world);
}

static void pan_right_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world.window()->pan_right(step);
    refresh_canvas(GTK_WIDGET(canvas), world);
}

static void pan_up_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world.window()->pan_up(step);
    refresh_canvas(GTK_WIDGET(canvas), world);
}

static void pan_down_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world.window()->pan_down(step);
    refresh_canvas(GTK_WIDGET(canvas), world);
}

enum Tool { TRANSLATE, SCALE, ROTATE };

Tool selected_tool = TRANSLATE;

static void tool_translate_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    selected_tool = TRANSLATE;
    gtk_widget_grab_focus(GTK_WIDGET(canvas));
}

static void tool_scale_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    selected_tool = SCALE;
    gtk_widget_grab_focus(GTK_WIDGET(canvas));
}

static void tool_rotate_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    selected_tool = ROTATE;
    gtk_widget_grab_focus(GTK_WIDGET(canvas));
}

static void select_cs(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    clipping_method = ClippingMethod::COHEN_SUTHERLAND;
    refresh_canvas(GTK_WIDGET(canvas), world);
}

static void select_lb(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    clipping_method = ClippingMethod::LIANG_BARSKY;
    refresh_canvas(GTK_WIDGET(canvas), world);
}

static void select_none(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    clipping_method = ClippingMethod::NONE;
    refresh_canvas(GTK_WIDGET(canvas), world);
}

static gboolean canvas_on_key_press(GtkWidget *canvas, GdkEventKey *event, gpointer data)
{
    World &world = *(World*)data;

    switch (selected_tool)
    {
        case TRANSLATE:
            switch (event->keyval)
            {
                case GDK_KEY_Left:
                    world.translate_selected(-1, 0);
                    break;

                case GDK_KEY_Right:
                    world.translate_selected(+1, 0);
                    break;

                case GDK_KEY_Down:
                    world.translate_selected(0, -1);
                    break;

                case GDK_KEY_Up:
                    world.translate_selected(0, +1);
                    break;

                default:
                    break;
            }
            break;

        case SCALE:
            switch (event->keyval)
            {
                case GDK_KEY_Right:
                case GDK_KEY_Up:
                    world.scale_selected(1.1);
                    break;

                case GDK_KEY_Left:
                case GDK_KEY_Down:
                    world.scale_selected(0.9);
                    break;

                default:
                    break;
            }
            break;

        case ROTATE:
            switch (event->keyval)
            {
                case GDK_KEY_Right:
                case GDK_KEY_Down:
                    world.rotate_selected(+1);
                    break;

                case GDK_KEY_Left:
                case GDK_KEY_Up:
                    world.rotate_selected(-1);
                    break;

                default:
                    break;
            }
            break;
    }

    refresh_canvas(GTK_WIDGET(canvas), world);

    return true;
}

static GtkWidget * button_move;
static GtkWidget * button_scale;
static GtkWidget * button_rotate;

static void select_or_hide_tool_buttons(initializer_list<GtkWidget *> tool_buttons)
{
    for (GtkWidget *button: tool_buttons)
        if (button_move)
            gtk_widget_set_sensitive(GTK_WIDGET(button), world.has_selected_objects());
}

static void select_object(UNUSED GtkListBox *list_box, GtkListBoxRow *row, gpointer canvas)
{
    world.clear_selection();

    if (row != nullptr) {
        world.select_object_at(gtk_list_box_row_get_index(row));
    }

    refresh_canvas(GTK_WIDGET(canvas), world);

    select_or_hide_tool_buttons({ button_move, button_scale, button_rotate });
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *gtk_window = new_gtk_window("Graphics");
    GtkWidget *grid = new_grid(gtk_window);

    GtkWidget *canvas = new_canvas(grid, world, G_CALLBACK(canvas_on_key_press));

    list<pair<string, GCallback>> menu_items;
    menu_items.push_back(make_pair("Cohen-Sutherland", G_CALLBACK(select_cs)));
    menu_items.push_back(make_pair("Liang-Barsky", G_CALLBACK(select_lb)));
    menu_items.push_back(make_pair("None", G_CALLBACK(select_none)));
    menu_bar(grid, canvas, menu_items);
    
    new_list_box(grid, canvas, world, G_CALLBACK(select_object));

    new_button(
        grid, canvas, "Zoom In", true, G_CALLBACK(zoom_in_clicked),
        "Press to zoom into the world.");
    new_button(
        grid, canvas, "Zoom Out", true, G_CALLBACK(zoom_out_clicked),
        "Press to zoom out of the world.");
    new_button(
        grid, canvas, " < ", true, G_CALLBACK(pan_left_clicked),
        "Press to move the world's window to the left.");
    new_button(
        grid, canvas, " > ", true, G_CALLBACK(pan_right_clicked),
        "Press to move the world's window to the right.");
    new_button(
        grid, canvas, "Up", true, G_CALLBACK(pan_up_clicked),
        "Press to move up the world's window.");
    new_button(
        grid, canvas, "Down", true, G_CALLBACK(pan_down_clicked),
        "Press to move down the world's window.");

    button_move = new_button(
        grid, canvas, "Translate", false, G_CALLBACK(tool_translate_clicked),
        "Press and use arrow keys to translate selected objects.");
    button_scale = new_button(
        grid, canvas, "Scale", false, G_CALLBACK(tool_scale_clicked),
        "Press and use arrow keys to shrink/enlarge selected objects.");
    button_rotate = new_button(
        grid, canvas, "Rotate", false, G_CALLBACK(tool_rotate_clicked),
        "Press and use arrow keys to rotate selected objects.");

    gtk_widget_show_all(gtk_window);
    gtk_main();

    return 0;
}