//
// Created by petrs on 27.04.2024.
//

#include <unordered_map>
#include "../geometry/Cell.h"
#include "../geometry/Interface.h"
#include "GAMM.h"
#include "Def.h"
#include "Bound.h"

void GAMM::updateInlet(std::unordered_map<int, Cell> &cells) {
    for (int j  = 0; j < Def::yInner; ++j) {
        int k = Def::firstInner + j * Def::xCells;

        Conservative innerW = cells.at(k).w;
        Conservative outerW = Bound::updateInletCell(innerW);
        cells.at(k - 1).w = outerW;
    }
}

void GAMM::updateOutlet(std::unordered_map<int, Cell> &cells) {
    for (int j = 0; j < Def::yInner; ++j) {
        int k = Def::firstInner + Def::xInner - 1 + j * Def::xCells;

        Conservative innerW = cells.at(k).w;
        Conservative outerW = Bound::updateOutletCell(innerW);
        cells.at(k + 1).w = outerW;
    }
}

void GAMM::updateWalls(std::unordered_map<int, Cell> &cells, const std::unordered_map<std::pair<int, int>, Interface, pair_hash> &faces) {
    for (int i = 0; i < Def::xInner; ++i) {
        int k = Def::firstInner + i;

        Interface face = faces.at(std::make_pair(k, k + 1));
        Conservative innerW = cells.at(k).w;
        Conservative outerW = Bound::updateWallCell(innerW, face);
        cells.at(k - Def::xCells).w = outerW;
    }
    for (int i = 0; i < Def::xInner; ++i) {
        int k = Def::firstInner + (Def::yInner) * Def::xCells + i;

        Interface face = faces.at(std::make_pair(k, k + 1));
        Conservative innerW = cells.at(k - Def::xCells).w;
        Conservative outerW = Bound::updateWallCell(innerW, face);
        cells.at(k).w = outerW;
    }
}

void GAMM::updateBounds(std::unordered_map<int, Cell> &cells, const std::unordered_map<std::pair<int, int>, Interface, pair_hash> &faces) {
    updateInlet(cells);
    updateOutlet(cells);
    updateWalls(cells, faces);
}