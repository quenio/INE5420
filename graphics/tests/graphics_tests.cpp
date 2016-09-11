// Copyright (c) 2016 Quenio Cesar Machado dos Santos. All rights reserved.

#include "min_unit.h"
#include "../graphics.h"

void print(Coord coord)
{
    printf("(%f,%f)\n", coord.x(), coord.y());
}

const char * to_world()
{
    const double left = -20, bottom = -20, right = 120, top = 120;

    Window window(left, bottom, right, top);

    Coord left_top(left, top);
    Coord left_bottom(left, bottom);
    Coord right_bottom(right, bottom);
    Coord right_top(right, top);
    Coord center = equidistant(left_bottom, right_top);
    Coord northwest = equidistant(left_top, center);
    Coord northeast = equidistant(right_top, center);
    Coord southeast = equidistant(right_bottom, center);
    Coord southwest = equidistant(left_bottom, center);

    print(window.to_world(Coord(-1, +1)));

    mu_assert(window.to_world(Coord(-1, +1)) == left_top);
    mu_assert(window.to_world(Coord(-1, -1)) == left_bottom);
    mu_assert(window.to_world(Coord(+1, -1)) == right_bottom);
    mu_assert(window.to_world(Coord(+1, +1)) == right_top);
    mu_assert(window.to_world(Coord(0, 0)) == center);
    mu_assert(window.to_world(Coord(-0.5, +0.5)) == northwest);
    mu_assert(window.to_world(Coord(+0.5, +0.5)) == northeast);
    mu_assert(window.to_world(Coord(+0.5, -0.5)) == southeast);
    mu_assert(window.to_world(Coord(-0.5, -0.5)) == southwest);

    return nullptr;
}

const char * from_world()
{
    const double left = -20, bottom = -20, right = 120, top = 120;

    Window window(left, bottom, right, top);

    Coord left_top(left, top);
    Coord left_bottom(left, bottom);
    Coord right_bottom(right, bottom);
    Coord right_top(right, top);
    Coord center = equidistant(left_bottom, right_top);
    Coord northwest = equidistant(left_top, center);
    Coord northeast = equidistant(right_top, center);
    Coord southeast = equidistant(right_bottom, center);
    Coord southwest = equidistant(left_bottom, center);

    mu_assert(window.from_world(left_top) == Coord(-1, +1));
    mu_assert(window.from_world(left_bottom) == Coord(-1, -1));
    mu_assert(window.from_world(right_bottom) == Coord(+1, -1));
    mu_assert(window.from_world(right_top) == Coord(+1, +1));
    mu_assert(window.from_world(center) == Coord(0, 0));
    mu_assert(window.from_world(northwest) == Coord(-0.5, +0.5));
    mu_assert(window.from_world(northeast) == Coord(+0.5, +0.5));
    mu_assert(window.from_world(southeast) == Coord(+0.5, -0.5));
    mu_assert(window.from_world(southwest) == Coord(-0.5, -0.5));

    return nullptr;
}

const char * to_viewport()
{
    Coord left_top(-1, +1);
    Coord left_bottom(-1, -1);
    Coord right_bottom(+1, -1);
    Coord right_top(+1, +1);
    Coord center = Coord(0, 0);
    Coord northwest = Coord(-0.5, +0.5);
    Coord northeast = Coord(+0.5, +0.5);
    Coord southeast = Coord(+0.5, -0.5);
    Coord southwest = Coord(-0.5, -0.5);

    Window window(0, 0, 200, 200); // world coord not used for the window-to-viewport transform

    const double width = 100, height = 100;
    const Viewport viewport(width, height);

    mu_assert(window.to_viewport(left_top, viewport) == Coord(2.5, 2.5));
    mu_assert(window.to_viewport(left_bottom, viewport) == Coord(2.5, 97.5));
    mu_assert(window.to_viewport(right_bottom, viewport) == Coord(97.5, 97.5));
    mu_assert(window.to_viewport(right_top, viewport) == Coord(97.5, 2.5));
    mu_assert(window.to_viewport(center, viewport) == Coord(50, 50));
    mu_assert(window.to_viewport(northwest, viewport) == Coord(26.25, 26.25));
    mu_assert(window.to_viewport(northeast, viewport) == Coord(73.75, 26.25));
    mu_assert(window.to_viewport(southeast, viewport) == Coord(73.75, 73.75));
    mu_assert(window.to_viewport(southwest, viewport) == Coord(26.25, 73.75));

    return nullptr;
}

const char * from_viewport()
{
    Coord left_top(2.5, 2.5);
    Coord left_bottom(2.5, 97.5);
    Coord right_bottom(97.5, 97.5);
    Coord right_top(97.5, 2.5);
    Coord center = Coord(50, 50);
    Coord northwest = Coord(26.25, 26.25);
    Coord northeast = Coord(73.75, 26.25);
    Coord southeast = Coord(73.75, 73.75);
    Coord southwest = Coord(26.25, 73.75);

    Window window(0, 0, 200, 200); // world coord not used for the window-to-viewport transform

    const double width = 100, height = 100;
    const Viewport viewport(width, height);

    mu_assert(window.from_viewport(left_top, viewport) == Coord(-1, +1));
    mu_assert(window.from_viewport(left_bottom, viewport) == Coord(-1, -1));
    mu_assert(window.from_viewport(right_bottom, viewport) == Coord(+1, -1));
    mu_assert(window.from_viewport(right_top, viewport) == Coord(+1, +1));
    mu_assert(window.from_viewport(center, viewport) == Coord(0, 0));
    mu_assert(window.from_viewport(northwest, viewport) == Coord(-0.5, +0.5));
    mu_assert(window.from_viewport(northeast, viewport) == Coord(+0.5, +0.5));
    mu_assert(window.from_viewport(southeast, viewport) == Coord(+0.5, -0.5));
    mu_assert(window.from_viewport(southwest, viewport) == Coord(-0.5, -0.5));

    return nullptr;
}

void all_tests()
{
    mu_test(to_world);
    mu_test(from_world);
    mu_test(to_viewport);
    mu_test(from_viewport);
}
