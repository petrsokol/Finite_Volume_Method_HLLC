//
// Created by petrs on 02.12.2023.
//

#include <cmath>
#include "Vector.h"

Vector::Vector(Point a, Point b) {
    Vector::x = b.x - a.x;
    Vector::y = b.y - a.y;
    Vector::length = sqrt(pow(x, 2) + pow(y, 2));
    ux = x / length;
    uy = y / length;
}
