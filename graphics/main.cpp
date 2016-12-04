//#define WORLD_2D
#define WORLD_3D

#include "ui.h"
#include "file_conversions.h"
#include "obj_samples.h"
#include "timer.h"

#include <fstream>

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

#define OBJ_DIR "/Users/Quenio/Projects/UFSC/INE5420/graphics/obj/"
//#define OBJ_DIR "/home/daniel/Workspaces/CG/graphics/obj/"

enum SelectedWorld { CUBE, BEZIER_SURFACE, SPLINE_SURFACE, TEAPOT, PYRAMID, TRUMPET, SHUTTLE, MAGNOLIA, LAMP, HOUSE, SQUARE };

static SelectedWorld selected_world = SelectedWorld::CUBE;

static World<Coord3D> world(
    make_shared<Window>(-20, -20, 120, 120),
    DisplayFile<Coord3D>({})
);

static void update_world(SelectedWorld selected)
{
    switch (selected)
    {
        case CUBE:
        {
            world = World<Coord3D>(
                make_shared<Window>(-20, -20, 120, 120),
                DisplayFile<Coord3D>({ draw_cube(Coord3D(20, 20, 20), 50) })
            );
        }
        break;

        case BEZIER_SURFACE:
        {
            world = World<Coord3D>(
                make_shared<Window>(-20, -20, 120, 120),
                DisplayFile<Coord3D>({
                    draw_bezier_surface({{
                        Coord3D(10, 10, 20), Coord3D(10, 90, 20), Coord3D(90, 10, 20), Coord3D(90, 90, 20),
                        Coord3D(10, 10, 30), Coord3D(10, 90, 30), Coord3D(90, 10, 30), Coord3D(90, 90, 30),
                        Coord3D(10, 10, 40), Coord3D(10, 60, 40), Coord3D(90, 40, 40), Coord3D(90, 90, 40),
                        Coord3D(10, 10, 50), Coord3D(10, 90, 50), Coord3D(90, 10, 50), Coord3D(90, 90, 50)
                    }})
                })
            );
        }
        break;

        case SPLINE_SURFACE:
        {
            world = World<Coord3D>(
                make_shared<Window>(-100, 0, +100, +200),
                DisplayFile<Coord3D>({
                    draw_spline_surface({
                        {
                            Coord3D(50, 10, 20), Coord3D(20, 30, 20), Coord3D(20, 70, 20), Coord3D(50, 90, 20),
                            Coord3D(50, 10, 40), Coord3D(20, 30, 40), Coord3D(20, 70, 40), Coord3D(50, 90, 40),
                            Coord3D(50, 10, 60), Coord3D(20, 30, 60), Coord3D(20, 70, 60), Coord3D(50, 90, 60),
                            Coord3D(50, 10, 80), Coord3D(20, 30, 80), Coord3D(20, 70, 80), Coord3D(50, 90, 80)
                        },
                        {
                            Coord3D(20, 30, 20), Coord3D(20, 70, 20), Coord3D(50, 90, 20), Coord3D(80, 70, 20),
                            Coord3D(20, 30, 40), Coord3D(20, 70, 40), Coord3D(50, 90, 40), Coord3D(80, 70, 40),
                            Coord3D(20, 30, 60), Coord3D(20, 70, 60), Coord3D(50, 90, 60), Coord3D(80, 70, 60),
                            Coord3D(20, 30, 80), Coord3D(20, 70, 80), Coord3D(50, 90, 80), Coord3D(80, 70, 80)
                        },
                        {
                            Coord3D(20, 70, 20), Coord3D(50, 90, 20), Coord3D(80, 70, 20), Coord3D(80, 30, 20),
                            Coord3D(20, 70, 40), Coord3D(50, 90, 40), Coord3D(80, 70, 40), Coord3D(80, 30, 40),
                            Coord3D(20, 70, 60), Coord3D(50, 90, 60), Coord3D(80, 70, 60), Coord3D(80, 30, 60),
                            Coord3D(20, 70, 80), Coord3D(50, 90, 80), Coord3D(80, 70, 80), Coord3D(80, 30, 80),
                        }
                    })
                })
            );
        }
        break;

        case TEAPOT:
        {
            ifstream teapot(OBJ_DIR "teapot.obj");
            world = World<Coord3D>(
                make_shared<Window>(-5, -5, 5, 5),
                DisplayFile<Coord3D>(
                    as_display_commands(as_group_3d(obj_file(teapot))) // fast - number of vertices matches the .obj file
//        as_display_commands(as_object_3d(obj_file(teapot))) // slow - too many vertices
                )
            );
        }
        break;

        case PYRAMID:
        {
            ifstream pyramid(OBJ_DIR "pyramid.obj");
            world = World<Coord3D>(
                make_shared<Window>(-2, -2, 2, 2),
                DisplayFile<Coord3D>(
                    as_display_commands(as_group_3d(obj_file(pyramid)))
                )
            );
        }
        break;

        case TRUMPET:
        {
            ifstream trumpet(OBJ_DIR "trumpet.obj");
            world = World<Coord3D>(
                make_shared<Window>(-250, -1000, +250, 0),
                DisplayFile<Coord3D>(
                    as_display_commands(as_group_3d(obj_file(trumpet)))
                )
            );
        }
        break;

        case SHUTTLE:
        {
            ifstream shuttle(OBJ_DIR "shuttle.obj");
            world = World<Coord3D>(
                make_shared<Window>(-10, -10, +10, +10),
                DisplayFile<Coord3D>(
                    as_display_commands(as_group_3d(obj_file(shuttle)))
                )
            );
        }
        break;

        case MAGNOLIA:
        {
            ifstream magnolia(OBJ_DIR "magnolia.obj");
            world = World<Coord3D>(
                make_shared<Window>(-100, -100, +100, +100),
                DisplayFile<Coord3D>(
                    as_display_commands(as_group_3d(obj_file(magnolia)))
                )
            );
        }
        break;

        case LAMP:
        {
            ifstream lamp(OBJ_DIR "lamp.obj");
            world = World<Coord3D>(
                make_shared<Window>(-10, -10, +10, +10),
                DisplayFile<Coord3D>(
                    as_display_commands(as_group_3d(obj_file(lamp)))
                )
            );
        }
        break;

        case HOUSE:
        {
            ifstream house(OBJ_DIR "house.obj");
            world = World<Coord3D>(
                make_shared<Window>(-10, -10, +10, +10),
                DisplayFile<Coord3D>(
                    as_display_commands(as_group_3d(obj_file(house)))
                )
            );
        }
        break;

        case SQUARE:
        {
            ifstream square_obj(OBJ_DIR "square.obj");
            world = World<Coord3D>(
                make_shared<Window>(-20, -20, +20, +20),
                DisplayFile<Coord3D>(
                    as_display_commands(as_group_3d(obj_file(square_obj)))
                )
            );
        }
    }
}
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

static void tool_translate_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    selection.select_tool(TRANSLATE);
    gtk_widget_grab_focus(GTK_WIDGET(canvas));
}

static void tool_scale_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    selection.select_tool(SCALE);
    gtk_widget_grab_focus(GTK_WIDGET(canvas));
}

static void tool_rotate_clicked(GtkWidget UNUSED *widget, gpointer canvas)
{
    selection.select_tool(ROTATE);
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

static void select_regular_surface_method(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    surface_method = SurfaceMethod ::REGULAR;
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void select_fd_surface_method(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    surface_method = SurfaceMethod ::FORWARD_DIFFERENCE;
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

#ifdef WORLD_3D

static void select_cube_world(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    selected_world = SelectedWorld::CUBE;
    update_world(selected_world);
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void select_bezier_world(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    selected_world = SelectedWorld::BEZIER_SURFACE;
    update_world(selected_world);
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void select_spline_world(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    selected_world = SelectedWorld::SPLINE_SURFACE;
    update_world(selected_world);
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void select_teapot_world(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    selected_world = SelectedWorld::TEAPOT;
    update_world(selected_world);
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void select_pyramid_world(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    selected_world = SelectedWorld::PYRAMID;
    update_world(selected_world);
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void select_trumpet_world(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    selected_world = SelectedWorld::TRUMPET;
    update_world(selected_world);
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void select_shuttle_world(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    selected_world = SelectedWorld::SHUTTLE;
    update_world(selected_world);
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void select_magnolia_world(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    selected_world = SelectedWorld::MAGNOLIA;
    update_world(selected_world);
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void select_lamp_world(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    selected_world = SelectedWorld::LAMP;
    update_world(selected_world);
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void select_house_world(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    selected_world = SelectedWorld::HOUSE;
    update_world(selected_world);
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

static void select_square_world(GtkWidget UNUSED *menu_item, gpointer canvas)
{
    selected_world = SelectedWorld::SQUARE;
    update_world(selected_world);
    refresh_canvas(GTK_WIDGET(canvas), selection);
}

#endif

static GtkListBox *list_box;

static gboolean canvas_on_key_press(GtkWidget *canvas, GdkEventKey *event, gpointer UNUSED data)
{
    switch (selection.tool())
    {
        case NONE:
            printf("no-op\n");
            break;

        case TRANSLATE:
            printf("no-op\n");
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
                    selection.rotate(+10);
                    break;

                case GDK_KEY_Left:
                case GDK_KEY_Up:
                    selection.rotate(-10);
                    break;

                default:
                    break;
            }
            break;
    }

    switch (event->keyval)
    {
        case GDK_KEY_Return:
            selection.select_tool(NONE);
            break;

        case GDK_KEY_A:
        case GDK_KEY_a:
            selection.toggle_full_selection();
#ifndef _GRAPHICS_BUILD
            if (selection.not_empty())
            {
                gtk_list_box_select_all(list_box);
            }
            else
            {
                gtk_list_box_unselect_all(list_box);
            }
#endif
            break;

        case GDK_KEY_G:
        case GDK_KEY_g:
            selection.select_tool(TRANSLATE);
            break;

        case GDK_KEY_S:
        case GDK_KEY_s:
            selection.select_tool(SCALE);
            break;

        case GDK_KEY_R:
        case GDK_KEY_r:
            selection.select_tool(ROTATE);
            break;

        case GDK_KEY_X:
        case GDK_KEY_x:
            selection.select_transform_axis(X_AXIS);
            break;

        case GDK_KEY_Y:
        case GDK_KEY_y:
            selection.select_transform_axis(Y_AXIS);
            break;

        case GDK_KEY_Z:
        case GDK_KEY_z:
            selection.select_transform_axis(Z_AXIS);
            break;

        default:
            break;
    }

    refresh_canvas(GTK_WIDGET(canvas), selection);

    return true;
}

static gboolean canvas_on_scroll(GtkWidget *canvas, GdkEventScroll *event)
{
    if (selection.tool() != NONE) return true;

    switch(event->direction)
    {
        case GDK_SCROLL_UP:
        {
            if (event->state & GDK_SHIFT_MASK)
            {
                world.window()->pan_up(step);
            }
            else if (event->state & GDK_CONTROL_MASK)
            {
                world.window()->zoom_out(step);
            }
            else
            {
//                world.window()->rotate_x(-10, world.window()->center());
            }
        }
        break;

        case GDK_SCROLL_DOWN:
        {
            if (event->state & GDK_SHIFT_MASK)
            {
                world.window()->pan_down(step);
            }
            else if (event->state & GDK_CONTROL_MASK)
            {
                world.window()->zoom_in(step);
            }
            else
            {
//                world.window()->rotate_x(+10, world.window()->center());
            }
        }
        break;

        case GDK_SCROLL_LEFT:
        {
            if (event->state & GDK_SHIFT_MASK)
            {
                world.window()->pan_left(step);
            }
            else if (event->state & GDK_CONTROL_MASK)
            {
                world.window()->zoom_in(step);
            }
            else
            {
//                world.window()->rotate_y(-10, world.window()->center());
            }
        }
        break;

        case GDK_SCROLL_RIGHT:
        {
            if (event->state & GDK_SHIFT_MASK)
            {
                world.window()->pan_right(step);
            }
            else if (event->state & GDK_CONTROL_MASK)
            {
                world.window()->zoom_out(step);
            }
            else
            {
//                world.window()->rotate_y(+10, world.window()->center());
            }
        }
        break;

        case GDK_SCROLL_SMOOTH:
            printf("WARNING: Unexpected scroll event: %d", event->direction);
    }

    refresh_canvas(GTK_WIDGET(canvas), selection);

    return true;
}

static gboolean canvas_on_motion(GtkWidget *canvas, GdkEventMotion *event)
{
    const Coord2D location = selection.window()->window_to_world(
        selection.window()->from_viewport(
            { event->x, event->y },
            gtk_widget_get_allocated_height(canvas)));
    static Coord2D previous_location = location;

    const double delta_x = location.x() - previous_location.x();
    const double delta_y = location.y() - previous_location.y();
    const double distance_to_center = distance(location, selection.center());
    const double distance_to_previous = distance(location, previous_location);
    const double factor = distance_to_previous / distance_to_center;

    printf("dx = %f; dy = %f\n", delta_x, delta_y);
    printf("factor = %f\n", factor);

    switch (selection.tool())
    {
        case TRANSLATE:
        {
            if (!equals(delta_x, 0) || !equals(delta_y, 0))
            {
                selection.translate(delta_x, delta_y, delta_x);
                refresh_canvas(GTK_WIDGET(canvas), selection);
            }
        }
        break;

        case SCALE:
        {
            if (distance_to_center > distance(previous_location, selection.center()))
            {
                selection.scale(1.0 + factor);
            }
            else
            {
                selection.scale(1.0 - factor);
            }

            refresh_canvas(GTK_WIDGET(canvas), selection);
        }
        break;

        case ROTATE:
        {
            const double ang_location = angular_coefficient(location, selection.center(), 1, 0);
            const double ang_previous = angular_coefficient(previous_location, selection.center(), 1, 0);

            if (ang_location > ang_previous)
            {
                selection.rotate(-(180 * factor));
            }
            else
            {
                selection.rotate(+(180 * factor));
            }

            refresh_canvas(GTK_WIDGET(canvas), selection);
        }
        break;

        case NONE:
        {
            printf("no-op\n");
        }
        break;
    }

    previous_location = location;

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

static void select_object(UNUSED GtkListBox *lb, GtkListBoxRow *row, gpointer canvas)
{
    printf("Object selection: started\n");
    const clock_t start = clock();

    selection.clear();
    if (row != nullptr) {
        selection.select_object_at((size_t)gtk_list_box_row_get_index(row));
    }
    refresh_canvas(GTK_WIDGET(canvas), selection);
    select_or_hide_tool_buttons({ button_move, button_scale, button_rotate });
    gtk_widget_grab_focus(GTK_WIDGET(canvas));

    const double time = elapsed_secs(start);
    printf("Object selection: finished (t = %9.6lf)\n", time);
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

#ifdef WORLD_3D
    update_world(selected_world);
#endif

    GtkWidget *gtk_window = new_gtk_window("Graphics");
    GtkWidget *grid = new_grid(gtk_window);

    GtkWidget *canvas = new_canvas(
        grid, selection, G_CALLBACK(canvas_on_key_press), G_CALLBACK(canvas_on_scroll), G_CALLBACK(canvas_on_motion));

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

    list<pair<string, GCallback>> surface_method_items;
    surface_method_items.push_back(make_pair("Forward-Difference", G_CALLBACK(select_fd_surface_method)));
    surface_method_items.push_back(make_pair("Regular", G_CALLBACK(select_regular_surface_method)));
    menu_bar_attach(menu_bar, canvas, "Surface", surface_method_items);

#ifdef WORLD_3D

    list<pair<string, GCallback>> world_items;
    world_items.push_back(make_pair("Cube", G_CALLBACK(select_cube_world)));
    world_items.push_back(make_pair("Bézier Surface", G_CALLBACK(select_bezier_world)));
    world_items.push_back(make_pair("Spline Surface", G_CALLBACK(select_spline_world)));
    world_items.push_back(make_pair("Teapot", G_CALLBACK(select_teapot_world)));
    world_items.push_back(make_pair("Pyramid", G_CALLBACK(select_pyramid_world)));
    world_items.push_back(make_pair("Trumpet", G_CALLBACK(select_trumpet_world)));
    world_items.push_back(make_pair("Shuttle", G_CALLBACK(select_shuttle_world)));
    world_items.push_back(make_pair("Magnolia", G_CALLBACK(select_magnolia_world)));
    world_items.push_back(make_pair("Lamp", G_CALLBACK(select_lamp_world)));
    world_items.push_back(make_pair("House", G_CALLBACK(select_house_world)));
    world_items.push_back(make_pair("Square", G_CALLBACK(select_square_world)));
    menu_bar_attach(menu_bar, canvas, "World", world_items);

#endif

    list_box = new_list_box(grid, canvas, selection, G_CALLBACK(select_object));

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
