//
// Created by petrs on 14.04.2025.
//

#include <cmath>
#include "Line.h"

// normal direction behaves like a VERTICAL interface
// if line points UP, normal vector points RIGHT
Line::Line (const double ax, const double ay, const double bx, const double by)
        : len(std::hypot(bx - ax, by - ay)), nx((by - ay) / len), ny(-(bx - ax) / len)
{
}

Line::Line (const Point & start, const Point & end)
        : Line(start.x, start.y, end.x, end.y)
{
}

Line::Line (double len, double nx, double ny)
        : len(len), nx(nx), ny(ny)
{
}