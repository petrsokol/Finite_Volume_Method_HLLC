//
// Created by petrs on 01.12.2023.
//

#include <cmath>
#include "Cell.h"
#include "../fluid_dynamics/Def.h"

Cell::Cell(std::vector<Point> vec) {
    Point a = vec[0];
    Point b = vec[1];
    Point c = vec[2];
    Point d = vec[3];

    Cell::index = a.index;

    double x1 = 1.0/3 * (a.x + b.x + c.x);
    double y1 = 1.0/3 * (a.y + b.y + c.y);
    double A1 = 0.5 * fabs(a.x*(b.y-c.y) + b.x*(c.y-a.y) + c.x*(a.y-b.y));

    double x2 = 1.0/3 * (a.x + c.x + d.x);
    double y2 = 1.0/3 * (a.y + c.y + d.y);
    double A2 = 0.5 * fabs(a.x*(c.y-d.y) + c.x*(d.y-a.y) + d.x*(a.y-c.y));

    Cell::area = A1 + A2;
    Cell::tx = (A1*x1 + A2*x2)/(A1 + A2);
    Cell::ty = (A1*y1 + A2*y2)/(A1 + A2); //ChatGPT

    Cell::w = Def::wInitial;
    Cell::rezi = 0;

    Cell::xi = Vector((a + d) / 2, (b + c) / 2);
    Cell::eta = Vector((c + d) / 2, (a + b) / 2);
}

std::vector<Point> Cell::getVertices(const std::vector<Point>& points, int i) {
    std::vector<Point> res{};
    res.push_back(points[i]); // a
    res.push_back(points[i + 1]); // b
    res.push_back(points[i + 1 + Def::xCells]); // c
    res.push_back(points[i + Def::xCells]); // d
    return res;
}

std::unordered_map<int, Cell> Cell::createCells(const std::vector<Point>& points) {
    std::unordered_map<int, Cell> res{};
    for (int j = 0; j < Def::yCells - 1; ++j) {
        for (int i = 0; i < Def::xCells - 1; ++i) {
            int k = i + j * Def::xCells;
            res[k] = Cell(getVertices(points, k));
        }
    }
    return res;
}

void Cell::toString() const {
    std::cout << "Cell " << index << ": area = " << area << ", T = [" << tx << ";" << ty << "], W = ";
    w.toString();
}
