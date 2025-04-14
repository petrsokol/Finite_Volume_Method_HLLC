//
// Created by petrs on 14.04.2025.
//

#include <cmath>
#include "Line.h"


Line::Line (const double ax, const double ay, const double bx, const double by)
        : len(std::hypot(bx - ax, by - ay)), nx((by - ay) / len), ny((ax - bx) / len)
{
}

Line::Line (const Point & a, const Point & b)
        : Line(a.x, a.y, b.x, b.y)
{
}

Line::Line (double len, double nx, double ny)
        : len(len), nx(nx), ny(ny)
{
}