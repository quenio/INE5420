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
    make_shared<Window<Coord2D>>(-20, -20, 120, 120),
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
    make_shared<Window<Coord3D>>(Coord3D(50, 50, -100), 140, 140),
    DisplayFile<Coord3D>({})
);

static GtkListBox *list_box;

static double scroll_step = 0.1;

static void update_world(SelectedWorld selected)
{
    switch (selected)
    {
        case CUBE:
        {
            world = World<Coord3D>(
                make_shared<Window<Coord3D>>(Coord3D(50, 50, -100), 140, 140),
                DisplayFile<Coord3D>({ draw_cube(Coord3D(20, 20, 20), 50) })
            );
            scroll_step = 0.01;
        }
        break;

        case BEZIER_SURFACE:
        {
            world = World<Coord3D>(
                make_shared<Window<Coord3D>>(Coord3D(50, 50, -100), 140, 140),
                DisplayFile<Coord3D>({
                    draw_bezier_surface({{
                        Coord3D(10, 10, 20), Coord3D(10, 90, 20), Coord3D(90, 10, 20), Coord3D(90, 90, 20),
                        Coord3D(10, 10, 30), Coord3D(10, 90, 30), Coord3D(90, 10, 30), Coord3D(90, 90, 30),
                        Coord3D(10, 10, 40), Coord3D(10, 60, 40), Coord3D(90, 40, 40), Coord3D(90, 90, 40),
                        Coord3D(10, 10, 50), Coord3D(10, 90, 50), Coord3D(90, 10, 50), Coord3D(90, 90, 50)
                    }})
                })
            );
            scroll_step = 0.05;
        }
        break;

        case SPLINE_SURFACE:
        {
            world = World<Coord3D>(
                make_shared<Window<Coord3D>>(Coord3D(0, 100, -100), 200, 200),
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
            scroll_step = 0.05;
        }
        break;

        case TEAPOT:
        {
            ifstream teapot(OBJ_DIR "teapot.obj");
            world = World<Coord3D>(
                make_shared<Window<Coord3D>>(Coord3D(0, 0, -10), 10, 10),
                DisplayFile<Coord3D>(
                    as_display_commands(as_group_3d(obj_file(teapot))) // fast - number of vertices matches the .obj file
//        as_display_commands(as_object_3d(obj_file(teapot))) // slow - too many vertices
                )
            );
            scroll_step = 0.1;
        }
        break;

        case PYRAMID:
        {
            ifstream pyramid(OBJ_DIR "pyramid.obj");
            world = World<Coord3D>(
                make_shared<Window<Coord3D>>(Coord3D(0, 0, -8), 4, 4),
                DisplayFile<Coord3D>(
                    as_display_commands(as_group_3d(obj_file(pyramid)))
                )
            );
            scroll_step = 0.01;
        }
        break;

        case TRUMPET:
        {
            ifstream trumpet(OBJ_DIR "trumpet.obj");
            world = World<Coord3D>(
                make_shared<Window<Coord3D>>(Coord3D(0, -500, -1000), 500, 500),
                DisplayFile<Coord3D>(
                    as_display_commands(as_group_3d(obj_file(trumpet)))
                )
            );
            scroll_step = 0.1;
        }
        break;

        case SHUTTLE:
        {
            ifstream shuttle(OBJ_DIR "shuttle.obj");
            world = World<Coord3D>(
                make_shared<Window<Coord3D>>(Coord3D(0, 0, -20), 20, 20),
                DisplayFile<Coord3D>(
                    as_display_commands(as_group_3d(obj_file(shuttle)))
                )
            );
            scroll_step = 0.02;
        }
        break;

        case MAGNOLIA:
        {
            ifstream magnolia(OBJ_DIR "magnolia.obj");
            world = World<Coord3D>(
                make_shared<Window<Coord3D>>(Coord3D(0, 0, -200), 200, 200),
                DisplayFile<Coord3D>(
                    as_display_commands(as_group_3d(obj_file(magnolia)))
                )
            );
            scroll_step = 0.03;
        }
        break;

        case LAMP:
        {
            ifstream lamp(OBJ_DIR "lamp.obj");
            world = World<Coord3D>(
                make_shared<Window<Coord3D>>(Coord3D(0, 0, -20), 20, 20),
                DisplayFile<Coord3D>(
                    as_display_commands(as_group_3d(obj_file(lamp)))
                )
            );
            scroll_step = 0.05;
        }
        break;

        case HOUSE:
        {
            ifstream house(OBJ_DIR "house.obj");
            world = World<Coord3D>(
                make_shared<Window<Coord3D>>(Coord3D(0, 0, -20), 20, 20),
                DisplayFile<Coord3D>(
                    as_display_commands(as_group_3d(obj_file(house)))
                )
            );
            scroll_step = 0.1;
        }
        break;

        case SQUARE:
        {
            ifstream square_obj(OBJ_DIR "square.obj");
            world = World<Coord3D>(
                make_shared<Window<Coord3D>>(Coord3D(0, 0, -20), 20, 20),
                DisplayFile<Coord3D>(
                    as_display_commands(as_group_3d(obj_file(square_obj)))
                )
            );
            scroll_step = 0.2;
        }
    }

    add_objects_to_list_box(list_box, world.objects());
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

static GtkWidget *button_orthogonal;
static GtkWidget *button_perspective;

static void update_projection_buttons()
{
    switch (projection_method)
    {
        case ORTHOGONAL:
        {
            gtk_button_set_label(GTK_BUTTON(button_orthogonal), "[Orthogonal]");
            gtk_widget_set_sensitive(GTK_WIDGET(button_orthogonal), false);
            gtk_button_set_label(GTK_BUTTON(button_perspective), "Perspective");
            gtk_widget_set_sensitive(GTK_WIDGET(button_perspective), true);
        }
        break;

        case PERSPECTIVE:
        {
            gtk_button_set_label(GTK_BUTTON(button_orthogonal), "Orthogonal");
            gtk_widget_set_sensitive(GTK_WIDGET(button_orthogonal), true);
            gtk_button_set_label(GTK_BUTTON(button_perspective), "[Perspective]");
            gtk_widget_set_sensitive(GTK_WIDGET(button_perspective), false);
        }
        break;
    }
}

static void select_orthogonal(GtkWidget UNUSED *widget, gpointer canvas)
{
    projection_method = ProjectionMethod::ORTHOGONAL;
    refresh_canvas(GTK_WIDGET(canvas), selection);
    update_projection_buttons();
}

static void select_perspective(GtkWidget UNUSED *widget, gpointer canvas)
{
    projection_method = ProjectionMethod::PERSPECTIVE;
    refresh_canvas(GTK_WIDGET(canvas), selection);
    update_projection_buttons();
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

static GtkWidget *button_move;
static GtkWidget *button_scale;
static GtkWidget *button_rotate;

static void select_or_hide_tool_buttons(initializer_list<GtkWidget *> tool_buttons)
{
    for (GtkWidget *button: tool_buttons)
    {
        if (button)
        {
            gtk_widget_set_sensitive(GTK_WIDGET(button), selection.not_empty());

            const gchar *button_label = gtk_button_get_label(GTK_BUTTON(button));
            if (button_label)
            {
                const string label(button_label);
                if (label.find("[") == 0)
                {
                    const size_t suffix_size = (selection.transform_axis() == ALL_AXIS ? 2 : 4);
                    gtk_button_set_label(GTK_BUTTON(button), label.substr(1, label.length() - suffix_size).c_str());
                }
            }
        }
    }
}

static void select_tool_translate()
{
    selection.select_tool(TRANSLATE);
    gtk_button_set_label(GTK_BUTTON(button_move), "[Grab]");
    gtk_widget_set_sensitive(GTK_WIDGET(button_move), false);
    select_or_hide_tool_buttons({ button_scale, button_rotate });
}

static void select_tool_scale()
{
    selection.select_tool(SCALE);
    gtk_button_set_label(GTK_BUTTON(button_scale), "[Scale]");
    gtk_widget_set_sensitive(GTK_WIDGET(button_scale), false);
    select_or_hide_tool_buttons({ button_move, button_rotate });
}

static void select_tool_rotate()
{
    selection.select_tool(ROTATE);
    gtk_button_set_label(GTK_BUTTON(button_rotate), "[Rotate]");
    gtk_widget_set_sensitive(GTK_WIDGET(button_rotate), false);
    select_or_hide_tool_buttons({ button_move, button_scale });
}

static GtkButton * selected_tool_button()
{
    switch (selection.tool())
    {
        case NONE: return nullptr;
        case TRANSLATE: return GTK_BUTTON(button_move);
        case SCALE: return GTK_BUTTON(button_scale);
        case ROTATE: return GTK_BUTTON(button_rotate);
    }
}

static string tool_name()
{
    switch (selection.tool())
    {
        case NONE: return "";
        case TRANSLATE: return "Grab";
        case SCALE: return "Scale";
        case ROTATE: return "Rotate";
    }
}

static string axis_name()
{
    switch (selection.transform_axis())
    {
        case ALL_AXIS: return "";
        case X_AXIS: return " X";
        case Y_AXIS: return " Y";
        case Z_AXIS: return " Z";
    }
}

static void update_button_label_with_axis()
{
    GtkButton *button = selected_tool_button();
    if (button)
    {
        string label = "[" + tool_name() + axis_name() + "]";
        gtk_button_set_label(button, label.c_str());
    }
}

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
            select_or_hide_tool_buttons({ button_move, button_scale, button_rotate });
            break;

        case GDK_KEY_5:
            if (projection_method == ORTHOGONAL)
            {
                select_perspective(nullptr, canvas);
            }
            else
            {
                select_orthogonal(nullptr, canvas);
            }
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
            select_or_hide_tool_buttons({ button_move, button_scale, button_rotate });
            break;

        case GDK_KEY_G:
        case GDK_KEY_g:
            select_tool_translate();
            break;

        case GDK_KEY_S:
        case GDK_KEY_s:
            select_tool_scale();
            break;

        case GDK_KEY_R:
        case GDK_KEY_r:
            select_tool_rotate();
            break;

        case GDK_KEY_X:
        case GDK_KEY_x:
            selection.select_transform_axis(X_AXIS);
            update_button_label_with_axis();
            break;

        case GDK_KEY_Y:
        case GDK_KEY_y:
            selection.select_transform_axis(Y_AXIS);
            update_button_label_with_axis();
            break;

        case GDK_KEY_Z:
        case GDK_KEY_z:
            selection.select_transform_axis(Z_AXIS);
            update_button_label_with_axis();
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

    const Coord2D delta = selection.window()->window_to_world(
        selection.window()->from_viewport(
            { event->delta_x, event->delta_y },
            gtk_widget_get_allocated_height(canvas)));

    switch(event->direction)
    {
        case GDK_SCROLL_UP:
        {
            if ((abs(delta.y()) / world.window()->height()) < scroll_step) return true;

            if (event->state & GDK_SHIFT_MASK)
            {
                world.window()->pan_up(scroll_step);
            }
            else if (event->state & GDK_CONTROL_MASK)
            {
                world.window()->zoom_out(scroll_step);
            }
            else
            {
                world.window()->rotate_z(-1, world.window()->center());
            }
        }
        break;

        case GDK_SCROLL_DOWN:
        {
            if ((abs(delta.y()) / world.window()->height()) < scroll_step) return true;

            if (event->state & GDK_SHIFT_MASK)
            {
                world.window()->pan_down(scroll_step);
            }
            else if (event->state & GDK_CONTROL_MASK)
            {
                world.window()->zoom_in(scroll_step);
            }
            else
            {
                world.window()->rotate_z(+1, world.window()->center());
            }
        }
        break;

        case GDK_SCROLL_LEFT:
        {
            if ((abs(delta.x()) / world.window()->width()) < scroll_step) return true;

            if (event->state & GDK_SHIFT_MASK)
            {
                world.window()->pan_left(scroll_step);
            }
            else if (event->state & GDK_CONTROL_MASK)
            {
                world.window()->zoom_in(scroll_step);
            }
            else
            {
                world.window()->rotate_z(-1, world.window()->center());
            }
        }
        break;

        case GDK_SCROLL_RIGHT:
        {
            if ((abs(delta.x()) / world.window()->width()) < scroll_step) return true;

            if (event->state & GDK_SHIFT_MASK)
            {
                world.window()->pan_right(scroll_step);
            }
            else if (event->state & GDK_CONTROL_MASK)
            {
                world.window()->zoom_out(scroll_step);
            }
            else
            {
                world.window()->rotate_z(+1, world.window()->center());
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

    list<pair<string, GCallback>> surface_method_items;
    surface_method_items.push_back(make_pair("Forward-Difference", G_CALLBACK(select_fd_surface_method)));
    surface_method_items.push_back(make_pair("Regular", G_CALLBACK(select_regular_surface_method)));
    menu_bar_attach(menu_bar, canvas, "Surface", surface_method_items);

#endif

    list<pair<string, GCallback>> clipping_items;
    clipping_items.push_back(make_pair("Cohen-Sutherland", G_CALLBACK(select_cs)));
    clipping_items.push_back(make_pair("Liang-Barsky", G_CALLBACK(select_lb)));
    clipping_items.push_back(make_pair("None", G_CALLBACK(select_none)));
    menu_bar_attach(menu_bar, canvas, "Clipping", clipping_items);

    new_list_label(grid, "Object List:");
    list_box = new_list_box(grid, canvas, selection, G_CALLBACK(select_object));

    button_orthogonal = new_button(
        grid, canvas, "Orthogonal", true, G_CALLBACK(select_orthogonal),
        "Switch to Orthogonal projection.", false, false);
    button_perspective = new_button(
        grid, canvas, "Perspective", true, G_CALLBACK(select_perspective),
        "Switch to Perspective projection.", true, false);

    button_move = new_button(
        grid, canvas, "Grab", false, G_CALLBACK(tool_translate_clicked),
        "Press and use arrow keys to move selected objects.", false, false);
    button_scale = new_button(
        grid, canvas, "Scale", false, G_CALLBACK(tool_scale_clicked),
        "Press and use arrow keys to shrink/enlarge selected objects.", false, false);
    button_rotate = new_button(
        grid, canvas, "Rotate", false, G_CALLBACK(tool_rotate_clicked),
        "Press and use arrow keys to rotate selected objects. Use x, y, z keys to change rotation axis.", true, false);

    new_button(
        grid, canvas, "Zoom In", true, G_CALLBACK(zoom_in_clicked),
        "Press to zoom into the world.", false, false);
    new_button(
        grid, canvas, "Zoom Out", true, G_CALLBACK(zoom_out_clicked),
        "Press to zoom out of the world.", true, false);

    new_button(
        grid, canvas, " < ", true, G_CALLBACK(pan_left_clicked),
        "Press to move the world's window to the left.", false, true);
    new_button(
        grid, canvas, " > ", true, G_CALLBACK(pan_right_clicked),
        "Press to move the world's window to the right.", false, true);
    new_button(
        grid, canvas, "Up", true, G_CALLBACK(pan_up_clicked),
        "Press to move up the world's window.", false, true);
    new_button(
        grid, canvas, "Down", true, G_CALLBACK(pan_down_clicked),
        "Press to move down the world's window.", false, true);

    update_projection_buttons();

    gtk_widget_show_all(gtk_window);
    gtk_main();

    return 0;
}
