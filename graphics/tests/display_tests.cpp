#include "min_unit.h"
#include "../display.h"

static void print(Coord2D coord)
{
    printf("(%f,%f)\n", coord.x(), coord.y());
}

static const char * to_world()
{
    const double left = -20, bottom = -20, right = 120, top = 120;

    Window window(left, bottom, right, top);

    Coord2D left_top(left, top);
    Coord2D left_bottom(left, bottom);
    Coord2D right_bottom(right, bottom);
    Coord2D right_top(right, top);
    Coord2D center = equidistant(left_bottom, right_top);
    Coord2D northwest = equidistant(left_top, center);
    Coord2D northeast = equidistant(right_top, center);
    Coord2D southeast = equidistant(right_bottom, center);
    Coord2D southwest = equidistant(left_bottom, center);

    print(window.to_world(Coord2D(-1, +1)));

    mu_assert(window.to_world(Coord2D(-1, +1)) == left_top);
    mu_assert(window.to_world(Coord2D(-1, -1)) == left_bottom);
    mu_assert(window.to_world(Coord2D(+1, -1)) == right_bottom);
    mu_assert(window.to_world(Coord2D(+1, +1)) == right_top);
    mu_assert(window.to_world(Coord2D(0, 0)) == center);
    mu_assert(window.to_world(Coord2D(-0.5, +0.5)) == northwest);
    mu_assert(window.to_world(Coord2D(+0.5, +0.5)) == northeast);
    mu_assert(window.to_world(Coord2D(+0.5, -0.5)) == southeast);
    mu_assert(window.to_world(Coord2D(-0.5, -0.5)) == southwest);

    return nullptr;
}

static const char * from_world()
{
    const double left = -20, bottom = -20, right = 120, top = 120;

    Window window(left, bottom, right, top);

    Coord2D left_top(left, top);
    Coord2D left_bottom(left, bottom);
    Coord2D right_bottom(right, bottom);
    Coord2D right_top(right, top);
    Coord2D center = equidistant(left_bottom, right_top);
    Coord2D northwest = equidistant(left_top, center);
    Coord2D northeast = equidistant(right_top, center);
    Coord2D southeast = equidistant(right_bottom, center);
    Coord2D southwest = equidistant(left_bottom, center);

    mu_assert(window.from_world(left_top) == Coord2D(-1, +1));
    mu_assert(window.from_world(left_bottom) == Coord2D(-1, -1));
    mu_assert(window.from_world(right_bottom) == Coord2D(+1, -1));
    mu_assert(window.from_world(right_top) == Coord2D(+1, +1));
    mu_assert(window.from_world(center) == Coord2D(0, 0));
    mu_assert(window.from_world(northwest) == Coord2D(-0.5, +0.5));
    mu_assert(window.from_world(northeast) == Coord2D(+0.5, +0.5));
    mu_assert(window.from_world(southeast) == Coord2D(+0.5, -0.5));
    mu_assert(window.from_world(southwest) == Coord2D(-0.5, -0.5));

    return nullptr;
}

static const char * to_viewport()
{
    Coord2D left_top(-1, +1);
    Coord2D left_bottom(-1, -1);
    Coord2D right_bottom(+1, -1);
    Coord2D right_top(+1, +1);
    Coord2D center = Coord2D(0, 0);
    Coord2D northwest = Coord2D(-0.5, +0.5);
    Coord2D northeast = Coord2D(+0.5, +0.5);
    Coord2D southeast = Coord2D(+0.5, -0.5);
    Coord2D southwest = Coord2D(-0.5, -0.5);

    Window window(0, 0, 200, 200); // world coord not used for the window-to-viewport transform

    const double width = 100, height = 100;
    const Viewport viewport(width, height);

    mu_assert(window.to_viewport(left_top, viewport) == Coord2D(2.5, 2.5));
    mu_assert(window.to_viewport(left_bottom, viewport) == Coord2D(2.5, 97.5));
    mu_assert(window.to_viewport(right_bottom, viewport) == Coord2D(97.5, 97.5));
    mu_assert(window.to_viewport(right_top, viewport) == Coord2D(97.5, 2.5));
    mu_assert(window.to_viewport(center, viewport) == Coord2D(50, 50));
    mu_assert(window.to_viewport(northwest, viewport) == Coord2D(26.25, 26.25));
    mu_assert(window.to_viewport(northeast, viewport) == Coord2D(73.75, 26.25));
    mu_assert(window.to_viewport(southeast, viewport) == Coord2D(73.75, 73.75));
    mu_assert(window.to_viewport(southwest, viewport) == Coord2D(26.25, 73.75));

    return nullptr;
}

static const char * from_viewport()
{
    Coord2D left_top(2.5, 2.5);
    Coord2D left_bottom(2.5, 97.5);
    Coord2D right_bottom(97.5, 97.5);
    Coord2D right_top(97.5, 2.5);
    Coord2D center = Coord2D(50, 50);
    Coord2D northwest = Coord2D(26.25, 26.25);
    Coord2D northeast = Coord2D(73.75, 26.25);
    Coord2D southeast = Coord2D(73.75, 73.75);
    Coord2D southwest = Coord2D(26.25, 73.75);

    Window window(0, 0, 200, 200); // world coord not used for the window-to-viewport transform

    const double width = 100, height = 100;
    const Viewport viewport(width, height);

    mu_assert(window.from_viewport(left_top, viewport) == Coord2D(-1, +1));
    mu_assert(window.from_viewport(left_bottom, viewport) == Coord2D(-1, -1));
    mu_assert(window.from_viewport(right_bottom, viewport) == Coord2D(+1, -1));
    mu_assert(window.from_viewport(right_top, viewport) == Coord2D(+1, +1));
    mu_assert(window.from_viewport(center, viewport) == Coord2D(0, 0));
    mu_assert(window.from_viewport(northwest, viewport) == Coord2D(-0.5, +0.5));
    mu_assert(window.from_viewport(northeast, viewport) == Coord2D(+0.5, +0.5));
    mu_assert(window.from_viewport(southeast, viewport) == Coord2D(+0.5, -0.5));
    mu_assert(window.from_viewport(southwest, viewport) == Coord2D(-0.5, -0.5));

    return nullptr;
}

void all_tests()
{
    mu_test(to_world);
    mu_test(from_world);
    mu_test(to_viewport);
    mu_test(from_viewport);
}
