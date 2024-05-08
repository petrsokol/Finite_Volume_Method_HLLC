//
// Created by petrs on 19.12.2023.
//

#include <cmath>
#include "NACA.h"
#include "Def.h"
#include "../structures/Primitive.h"
#include "Bound.h"

const int NACA::wingStart = 30;
const int NACA::wingLength = 200; // previous tests with 200 - changed 240506

void NACA::updateInlet(std::unordered_map<int, Cell> &cells) {
    for (int i  = 0; i < Def::xInner; ++i) { //celá horní stěna
        int k = Def::firstInner + (Def::yInner - 1) * Def::xCells + i; // last inner row

        Conservative innerW = cells.at(k).w;
        Conservative outerW = Bound::updateInletCell(innerW);
        cells.at(k + Def::xCells).w = outerW;
    }
}

void NACA::updateOutlet(std::unordered_map<int, Cell> &cells) {
    for (int j  = 0; j < Def::yInner; ++j) {
        int k = Def::firstInner + j * Def::xCells;

        Conservative innerW = cells.at(k).w;
        Conservative outerW = Bound::updateOutletCell(innerW);
        cells.at(k - 1).w = outerW;
    }
    for (int j = 0; j < Def::yInner; ++j) {
        int k = Def::firstInner + Def::xInner - 1 + j * Def::xCells; // last inner in each row

        Conservative innerW = cells.at(k).w;
        Conservative outerW = Bound::updateOutletCell(innerW);
        cells.at(k + 1).w = outerW;
    }
}

void NACA::updateWalls(std::unordered_map<int, Cell> &cells, const std::unordered_map<std::pair<int, int>, Interface, pair_hash> &faces) {
    for (int i = 0; i < NACA::wingLength; ++i) {
        int k = Def::firstInner + NACA::wingStart + i;

        Interface face = faces.at(std::make_pair(k, k + 1));
        Conservative innerW = cells.at(k).w;
        Conservative outerW = Bound::updateWallCell(innerW, face);
        cells.at(k - Def::xCells).w = outerW;
    }
}

void NACA::updatePeriodicity(std::unordered_map<int, Cell> &cells) {
    //start
    for (int i = 0; i < NACA::wingStart; ++i) {
        // inner cells - k
        int k = Def::firstInner + i;
        // ghost cells - l
        int l = Def::firstInner - Def::xCells + Def::xInner - 1 - i; //o řadu níž, poslední index - jede v protisměru // l viz BP, p. 13
        cells.at(l).w = cells.at(k).w;
    }
    //finish
    for (int i = 0; i < NACA::wingStart; ++i) { // podle mě by nemělo dělat 261. buňku
        // k: inner cells
        int k = Def::firstInner + NACA::wingStart + NACA::wingLength + i;
        // l: ghost cells
        int l = Def::firstInner - Def::xCells + NACA::wingStart - 1 - i; // -1 = těsně před koncem // odebrals -1
        cells.at(l).w = cells.at(k).w;
    }
}

void NACA::updateBounds(std::unordered_map<int, Cell> &cells, const std::unordered_map<std::pair<int, int>, Interface, pair_hash> &faces) {
    updateInlet(cells);
    updateOutlet(cells);
    updateWalls(cells, faces);
    updatePeriodicity(cells);
}
