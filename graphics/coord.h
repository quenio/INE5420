// Copyright (c) 2016 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

class Coord
{
public:
    Coord (double x, double y): _x(x), _y(y) {}

    virtual double x() const { return _x; }
    virtual double y() const { return _y; }

    // Scale x by factor fx, y by factor fy.
    Coord scale(double fx, double fy)
    {
        return Coord(x() * fx, y() * fy);
    }

    // Move by dx horizontally, dy vertically.
    Coord translate(double dx, double dy)
    {
        return Coord(x() + dx, y() + dy);
    }

private:
    double _x;
    double _y;
};

