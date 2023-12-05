//
// Created by petrs on 01.12.2023.
//

#include "Point.h"
#include "../fluid_dynamics/Def.h"

Point::Point(double x, double y, int index) : x(x), y(y), index(index) {}

std::vector<Point> Point::createPoints(std::vector<double> bot, std::vector<double> top) {
    std::vector<Point> res{};
    std::vector<double> dy{};

    for (int i = 0; i < Def::xCells; ++i) {
        dy.push_back((top[i] - bot[i]) / Def::yInner);
    }

    for (int i = 0; i < Def::cells; ++i) {
        double xx = (i % Def::xCells - Def::gl) * Def::dx;
        double yy = bot[i % Def::xCells] + (int(i / Def::xCells) - Def::gl) * dy[i % Def::xCells];
        res.emplace_back(xx, yy, i);
    }

    return res;
}

void Point::toString() const {
    std::cout << "point " << index << ": [" << x << ";" << y << "] \n";
}

Point Point::operator+(Point other) const {
    return {Point::x + other.x, Point::y + other.y, -1};
}

Point Point::operator-(Point other) const {
    return {Point::x - other.x, Point::y - other.y, -1};
}

Point Point::operator*(double scalar) const {
    return {Point::x * scalar, Point::y * scalar, -1};
}

Point Point::operator/(double scalar) const {
    if (scalar == 0.0) {
        std::cerr << "Error: Division by zero\n";
        exit(EXIT_FAILURE);
    }
    return {Point::x / scalar, Point::y / scalar, -1};
}
