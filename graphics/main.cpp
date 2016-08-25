// Copyright (c) 2016 Quenio Cesar Machado dos Santos. All rights reserved.

#include "graphics.h"

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    new_gtk_window("Graphics");
    new_grid();
    new_canvas();
    new_list_box();

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