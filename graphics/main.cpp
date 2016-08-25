// Copyright (c) 2016 Quenio Cesar Machado dos Santos. All rights reserved.

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

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    new_gtk_window("Graphics");
    new_grid();
    new_canvas(world);
    new_list_box(world.objects());

    new_button("Zoom In", G_CALLBACK(zoom_in_clicked));
    new_button("Zoom Out", G_CALLBACK(zoom_out_clicked));
    new_button(" < ", G_CALLBACK(span_left_clicked));
    new_button(" > ", G_CALLBACK(span_right_clicked));
    new_button(" Up ", G_CALLBACK(span_up_clicked));
    new_button(" Down ", G_CALLBACK(span_down_clicked));

    gtk_widget_show_all(gtk_window);
    gtk_main();

    return 0;
}