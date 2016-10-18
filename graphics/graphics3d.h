#pragma once

#include "transforms.h"

// 3D coordinates
class Coord3D: public XYZCoord<Coord3D>
{
public:

    Coord3D(double x, double y, double z): XYZCoord(x, y, z) {}

    Coord3D(const TVector &vector): XYZCoord(vector) {}

    list<Coord3D *> controls() override
    {
        return { this };
    }

};
