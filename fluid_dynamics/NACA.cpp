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

void NACA::updateInlet (std::vector<Cell> & cells)
{
  for (int i = 0; i < Def::xInner; ++i) { //celá horní stěna
    int k = Def::firstInner + (Def::yInner - 1) * Def::xCells + i; // last inner row

    Conservative innerW = cells.at(k).w;
    Conservative outerW = Bound::updateInletCell(innerW);
    cells.at(k + Def::xCells).w = outerW;
  }
}

void NACA::updateOutlet (std::vector<Cell> & cells)
{
  for (int j = 0; j < Def::yInner; ++j) {
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

void NACA::updateWalls (std::vector<Cell> & cells, const std::vector<Interface> & faces)
{
  for (int i = 0; i < NACA::wingLength; ++i) {
    int k = Def::firstInnerPoint + NACA::wingStart + i;

    // there are two faces for every cell - horizontal indices are even
    Interface face = faces.at(2 * k);
    Conservative innerW = cells.at(k).w;
    Conservative outerW = Bound::updateWallCell(innerW, face);
    cells.at(k - Def::xCells).w = outerW;
  }
}

void NACA::updatePeriodicity (std::vector<Cell> & cells)
{
  //start
  for (int i = 0; i < NACA::wingStart; ++i) {
    // inner points - k
    int k = Def::firstInner + i;
    // ghost points - l
    int l = Def::firstInner - Def::xCells + Def::xInner - 1 -
            i; //o řadu níž, poslední index - jede v protisměru // l viz BP, p. 13
    cells.at(l).w = cells.at(k).w;
    if (_isnan(cells.at(l).w.r1)) {
      std::cout << "Naca::periodicity: mame problem";
    }
  }
  //finish
  for (int i = 0; i < NACA::wingStart; ++i) { // podle mě by nemělo dělat 261. buňku
    // k: inner points
    int k = Def::firstInner + NACA::wingStart + NACA::wingLength + i;
    // l: ghost points
    int l = Def::firstInner - Def::xCells + NACA::wingStart - 1 - i; // -1 = těsně před koncem // odebrals -1
    cells.at(l).w = cells.at(k).w;
  }
}

void NACA::updateBounds (std::vector<Cell> & cells, const std::vector<Interface> & faces)
{
  updateInlet(cells);
  updateOutlet(cells);
  updateWalls(cells, faces);
  updatePeriodicity(cells);
}
