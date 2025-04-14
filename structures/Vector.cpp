//
// Created by petrs on 02.12.2023.
//

#include <cmath>
#include "Vector.h"

Vector::Vector (Point a, Point b)
        : x(b.x - a.x), y(b.y - a.y), len(hypot(x, y)), ux(x / len), uy(y / len)
{
}
