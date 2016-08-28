#include "ui.h"

static World world(
    Window(0, 0, 100, 100),
    DisplayFile({
        draw_point(Coord(25, 50)),
        draw_point(Coord(75, 50)),
        draw_line(Coord(10, 10), Coord(90, 90)),
        draw_square(Coord(10, 10), Coord(10, 90), Coord(90, 90), Coord(90, 10))
    })
);

static void zoom_in_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world.window().zoom_out(step);
    refresh_canvas(GTK_WIDGET(canvas), world);
}

static void zoom_out_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world.window().zoom_in(step);
    refresh_canvas(GTK_WIDGET(canvas), world);
}

static void span_left_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world.window().span_left(step);
    refresh_canvas(GTK_WIDGET(canvas), world);
}

static void span_right_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world.window().span_right(step);
    refresh_canvas(GTK_WIDGET(canvas), world);
}

static void span_up_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world.window().span_up(step);
    refresh_canvas(GTK_WIDGET(canvas), world);
}

static void span_down_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world.window().span_down(step);
    refresh_canvas(GTK_WIDGET(canvas), world);
}

enum Tool { MOVE, SCALE, ROTATE };

Tool selected_tool = MOVE;

static void tool_move_clicked(GtkWidget UNUSED *widget, UNUSED gpointer canvas)
{
    selected_tool = MOVE;
    gtk_widget_grab_focus(GTK_WIDGET(canvas));
}

static void tool_scale_clicked(GtkWidget UNUSED *widget, UNUSED gpointer canvas)
{
    selected_tool = SCALE;
    gtk_widget_grab_focus(GTK_WIDGET(canvas));
}

static void tool_rotate_clicked(GtkWidget UNUSED *widget, UNUSED gpointer canvas)
{
    selected_tool = ROTATE;
    gtk_widget_grab_focus(GTK_WIDGET(canvas));
}

static gboolean canvas_on_key_press(GtkWidget *canvas, GdkEventKey *event, gpointer data)
{
    World &world = *(World*)data;

    switch (selected_tool)
    {
        case MOVE:
            switch (event->keyval)
            {
                case GDK_KEY_Left:
                    world.move(-1, 0);
                    break;

                case GDK_KEY_Right:
                    world.move(+1, 0);
                    break;

                case GDK_KEY_Down:
                    world.move(0, -1);
                    break;

                case GDK_KEY_Up:
                    world.move(0, +1);
                    break;
            }
            break;

        case SCALE:
            switch (event->keyval)
            {
                case GDK_KEY_Left:
                case GDK_KEY_Up:
                    world.scale(1.1);
                    break;

                case GDK_KEY_Right:
                case GDK_KEY_Down:
                    world.scale(0.9);
                    break;
            }
            break;

        case ROTATE:
            switch (event->keyval)
            {
                case GDK_KEY_Right:
                case GDK_KEY_Up:
                    world.rotate(+1);
                    break;

                case GDK_KEY_Left:
                case GDK_KEY_Down:
                    world.rotate(-1);
                    break;
            }
            break;
    }

    refresh_canvas(GTK_WIDGET(canvas), world);

    return true;
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *gtk_window = new_gtk_window("Graphics");
    GtkWidget *grid = new_grid(gtk_window);
    GtkWidget *canvas = new_canvas(grid, world, G_CALLBACK(canvas_on_key_press));
    new_list_box(grid, world);

    new_button(grid, canvas, "Zoom In", G_CALLBACK(zoom_in_clicked));
    new_button(grid, canvas, "Zoom Out", G_CALLBACK(zoom_out_clicked));
    new_button(grid, canvas, " < ", G_CALLBACK(span_left_clicked));
    new_button(grid, canvas, " > ", G_CALLBACK(span_right_clicked));
    new_button(grid, canvas, "Up", G_CALLBACK(span_up_clicked));
    new_button(grid, canvas, "Down", G_CALLBACK(span_down_clicked));
    new_button(grid, canvas, "Move", G_CALLBACK(tool_move_clicked));
    new_button(grid, canvas, "Scale", G_CALLBACK(tool_scale_clicked));
    new_button(grid, canvas, "Rotate", G_CALLBACK(tool_rotate_clicked));

    gtk_widget_show_all(gtk_window);
    gtk_main();

    return 0;
}