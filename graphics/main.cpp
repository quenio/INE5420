//#define WORLD_2D
#define WORLD_3D

#include "ui.h"

using namespace std;

#ifdef WORLD_2D
static World<Coord2D> world(
    make_shared<Window>(-20, -20, 120, 120),
    DisplayFile<Coord2D>({
         draw_point(Coord2D(25, 50)),
         draw_point(Coord2D(75, 50)),
         draw_line(Coord2D(10, 10), Coord2D(90, 90)),
         draw_square(Coord2D(10, 10), Coord2D(10, 90), Coord2D(90, 90), Coord2D(90, 10)),
         draw_bezier_curve(Coord2D(10, 10), Coord2D(10, 90), Coord2D(90, 90), Coord2D(90, 10)),
         draw_spline_curve({
             Coord2D(50, 10), Coord2D(20, 30), Coord2D(20, 70), Coord2D(50, 90), Coord2D(80, 70), Coord2D(80, 30),
             Coord2D(50, 10), Coord2D(20, 30), Coord2D(20, 70)
         })
    })
);
#endif

#ifdef WORLD_3D
static World<Coord3D> world(
    make_shared<Window>(-20, -20, 120, 120),
    DisplayFile<Coord3D>({
         draw_cube(Coord3D(20, 20, 20), 50),
         draw_bezier_surface({{
             Coord3D(10, 10, 20), Coord3D(10, 90, 20), Coord3D(90, 10, 20), Coord3D(90, 90, 20),
             Coord3D(10, 10, 30), Coord3D(10, 90, 30), Coord3D(90, 10, 30), Coord3D(90, 90, 30),
             Coord3D(10, 10, 40), Coord3D(10, 60, 40), Coord3D(90, 40, 40), Coord3D(90, 90, 40),
             Coord3D(10, 10, 50), Coord3D(10, 90, 50), Coord3D(90, 10, 50), Coord3D(90, 90, 50)
         }})
//         draw_spline_surface({
//            {
//                Coord3D(50, 10, 20), Coord3D(20, 30, 20), Coord3D(20, 70, 20), Coord3D(50, 90, 20),
//                Coord3D(50, 10, 40), Coord3D(20, 30, 40), Coord3D(20, 70, 40), Coord3D(50, 90, 40),
//                Coord3D(50, 10, 60), Coord3D(20, 30, 60), Coord3D(20, 70, 60), Coord3D(50, 90, 60),
//                Coord3D(50, 10, 80), Coord3D(20, 30, 80), Coord3D(20, 70, 80), Coord3D(50, 90, 80)
//            },
//            {
//                Coord3D(20, 30, 20), Coord3D(20, 70, 20), Coord3D(50, 90, 20), Coord3D(80, 70, 20),
//                Coord3D(20, 30, 40), Coord3D(20, 70, 40), Coord3D(50, 90, 40), Coord3D(80, 70, 40),
//                Coord3D(20, 30, 60), Coord3D(20, 70, 60), Coord3D(50, 90, 60), Coord3D(80, 70, 60),
//                Coord3D(20, 30, 80), Coord3D(20, 70, 80), Coord3D(50, 90, 80), Coord3D(80, 70, 80)
//            },
//            {
//                Coord3D(20, 70, 20), Coord3D(50, 90, 20), Coord3D(80, 70, 20), Coord3D(80, 30, 20),
//                Coord3D(20, 70, 40), Coord3D(50, 90, 40), Coord3D(80, 70, 40), Coord3D(80, 30, 40),
//                Coord3D(20, 70, 60), Coord3D(50, 90, 60), Coord3D(80, 70, 60), Coord3D(80, 30, 60),
//                Coord3D(20, 70, 80), Coord3D(50, 90, 80), Coord3D(80, 70, 80), Coord3D(80, 30, 80),
//            }
//         })
    })
);
#endif

static UserSelection selection(world);

static void zoom_in_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world.window()->zoom_in(step);
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void zoom_out_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world.window()->zoom_out(step);
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void pan_left_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world.window()->pan_left(step);
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void pan_right_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world.window()->pan_right(step);
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void pan_up_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world.window()->pan_up(step);
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void pan_down_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    world.window()->pan_down(step);
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

enum Tool { TRANSLATE, SCALE, ROTATE };

static Tool selected_tool = TRANSLATE;

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
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void select_lb(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    clipping_method = ClippingMethod::LIANG_BARSKY;
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void select_none(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    clipping_method = ClippingMethod::NONE;
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void select_parallel(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    projection_method = ProjectionMethod::PARALLEL;
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void select_perspective(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    projection_method = ProjectionMethod::PERSPECTIVE;
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static gboolean canvas_on_key_press(GtkWidget *canvas, GdkEventKey *event, gpointer UNUSED data)
{
    switch (selected_tool)
    {
        case TRANSLATE:
            switch (event->keyval)
            {
                case GDK_KEY_Left:
#ifdef WORLD_2D
                    selection.translate(Coord2D(-1, 0));
#endif
#ifdef WORLD_3D
                    if (event->state & GDK_SHIFT_MASK && projection_method == PERSPECTIVE)
                    {
                        selection.translate(Coord3D(0, 0, -1));
                    }
                    else
                    {
                        selection.translate(Coord3D(-1, 0, 0));
                    }
#endif
                    break;

                case GDK_KEY_Right:
#ifdef WORLD_2D
                    selection.translate(Coord2D(+1, 0));
#endif
#ifdef WORLD_3D
                    if (event->state & GDK_SHIFT_MASK && projection_method == PERSPECTIVE)
                    {
                        selection.translate(Coord3D(0, 0, +1));
                    }
                    else
                    {
                        selection.translate(Coord3D(+1, 0, 0));
                    }
#endif
                    break;

                case GDK_KEY_Down:
#ifdef WORLD_2D
                    selection.translate(Coord2D(0, -1));
#endif
#ifdef WORLD_3D
                    if (event->state & GDK_SHIFT_MASK && projection_method == PERSPECTIVE)
                    {
                        selection.translate(Coord3D(0, 0, -1));
                    }
                    else
                    {
                        selection.translate(Coord3D(0, -1, 0));
                    }
#endif
                    break;

                case GDK_KEY_Up:
#ifdef WORLD_2D
                    selection.translate(Coord2D(0, +1));
#endif
#ifdef WORLD_3D
                    if (event->state & GDK_SHIFT_MASK && projection_method == PERSPECTIVE)
                    {
                        selection.translate(Coord3D(0, 0, +1));
                    }
                    else
                    {
                        selection.translate(Coord3D(0, +1, 0));
                    }
#endif
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
                    selection.scale(1.1);
                    break;

                case GDK_KEY_Left:
                case GDK_KEY_Down:
                    selection.scale(0.9);
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
                    selection.rotate(+1);
                    break;

                case GDK_KEY_Left:
                case GDK_KEY_Up:
                    selection.rotate(-1);
                    break;

                default:
                    break;
            }
            break;
    }

    switch (event->keyval)
    {
        case GDK_KEY_X:
        case GDK_KEY_x:
            selection.select_rotation_axis(X_AXIS);
            break;

        case GDK_KEY_Y:
        case GDK_KEY_y:
            selection.select_rotation_axis(Y_AXIS);
            break;

        case GDK_KEY_Z:
        case GDK_KEY_z:
            selection.select_rotation_axis(Z_AXIS);
            break;

        default:
            break;
    }

    refresh_canvas(GTK_WIDGET(canvas), selection);

    return true;
}

static GtkWidget * button_move;
static GtkWidget * button_scale;
static GtkWidget * button_rotate;

static void select_or_hide_tool_buttons(initializer_list<GtkWidget *> tool_buttons)
{
    for (GtkWidget *button: tool_buttons)
        if (button_move)
            gtk_widget_set_sensitive(GTK_WIDGET(button), selection.not_empty());
}

static void select_object(UNUSED GtkListBox *list_box, GtkListBoxRow *row, gpointer canvas)
{
    selection.clear();

    if (row != nullptr) {
        selection.select_object_at((size_t)gtk_list_box_row_get_index(row));
    }

    refresh_canvas(GTK_WIDGET(canvas), selection);

    select_or_hide_tool_buttons({ button_move, button_scale, button_rotate });
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *gtk_window = new_gtk_window("Graphics");
    GtkWidget *grid = new_grid(gtk_window);

    GtkWidget *canvas = new_canvas(grid, selection, G_CALLBACK(canvas_on_key_press));

    GtkWidget *menu_bar = new_menu_bar(grid);

    list<pair<string, GCallback>> clipping_items;
    clipping_items.push_back(make_pair("Cohen-Sutherland", G_CALLBACK(select_cs)));
    clipping_items.push_back(make_pair("Liang-Barsky", G_CALLBACK(select_lb)));
    clipping_items.push_back(make_pair("None", G_CALLBACK(select_none)));
    menu_bar_attach(menu_bar, canvas, "Clipping", clipping_items);

    list<pair<string, GCallback>> projection_items;
    projection_items.push_back(make_pair("Perspective", G_CALLBACK(select_perspective)));
    projection_items.push_back(make_pair("Parallel", G_CALLBACK(select_parallel)));
    menu_bar_attach(menu_bar, canvas, "Projection", projection_items);

    new_list_box(grid, canvas, selection, G_CALLBACK(select_object));

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
        "Press and use arrow keys to rotate selected objects. Use x, y, z keys to change rotation axis.");

    gtk_widget_show_all(gtk_window);
    gtk_main();

    return 0;
}
