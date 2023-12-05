//
// Created by petrs on 05.11.2023.
//

#include <cmath>

#include "Lines.h"
#include "../fluid_dynamics/Def.h"

void Lines::arc(std::vector<double> x, std::vector<double> &y, double l1, double l2, double d) {
    double y_s = (d * d - pow((l2 - l1) / 2, 2)) / (2 * d);
    double x_s = (l1 + l2) / 2;
    double r = d - y_s;

    for (int i = bound(l1); i < bound(l2); ++i) {
        y[i] = y_s + sqrt(pow(r,2) - pow(x[i]-x_s, 2));
    }
}

void Lines::line(std::vector<double> &y, double l1, double l2, double yValue) {
    for (int i = bound(l1); i < bound(l2); ++i) {
        y[i] = yValue;
    }
}

int Lines::bound(double l) {
    return int(floor(l / Def::dx) + Def::gl);
}
