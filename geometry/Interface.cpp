//
// Created by petrs on 01.12.2023.
//

#include "Interface.h"
#include "../fluid_dynamics/Def.h"
#include <cmath>

Interface::Interface(Point p1, Point p2) {
    bool isHorizontal = p2.index - p1.index == 1;

    double lx = p2.x - p1.x;
    double ly = p2.y - p1.y;
    length = sqrt(pow(lx, 2) + pow(ly, 2));

    double ux = lx / length;
    double uy = ly / length;

    nx = isHorizontal ? -uy : uy;
    ny = isHorizontal ? ux : -ux;

    right = p1.index;
    left = isHorizontal ? p1.index - Def::xCells : p1.index - 1;
}

std::unordered_map<std::pair<int, int>, Interface, pair_hash> Interface::createInnerFaces(std::vector<Point> points) {
    std::unordered_map<std::pair<int, int>, Interface, pair_hash> res;

    //horizontal first
    for (int j = 0; j < Def::yInner + 1; ++j) {
        for (int i = 0; i < Def::xInner; ++i) {
            // první vevnitř, j přidá celou řadu
//            int k = Def::firstInner + i + (j % Def::xInner) * Def::xCells; // původně
            int k = Def::firstInner + i + j * Def::xCells; // nově
            res[std::make_pair(k, k + 1)] = Interface(points[k], points[k + 1]);
        }
    }

    //vertical next
    for (int i = 0; i < Def::xInner + 1; ++i) {
        for (int j = 0; j < Def::yInner; ++j) {
            int k = Def::firstInner + i + j * Def::xCells;
            res[std::make_pair(k, k + Def::xCells)] = Interface(points[k], points[k + Def::xCells]);
        }
    }

    return res;
}

void Interface::toString() const {
    std::cout << "Interface " << left << "-" << right << ": length: " << length << ", norm: [" << nx << ":" << ny << "]\n";
}
