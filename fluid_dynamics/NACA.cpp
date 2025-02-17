//
// Created by petrs on 19.12.2023.
//

#include <cmath>
#include "NACA.h"
#include "Def.h"
#include "../structures/Primitive.h"
#include "Bound.h"

const int NACA::wingStart = 31;
const int NACA::wingLength = 198; // previous tests with 200 - changed 240506

void NACA::updateInlet (std::vector<Cell> & cells)
{
  for (int i = 0; i < Def::xInner; ++i) { //celá horní stěna
    int k = Def::firstInner + (Def::yInner - 1) * Def::xCells + i; // last inner row

    const Conservative & innerW1 = cells.at(k).w;
    Conservative & outerW1 = cells.at(k + Def::xCells).w;
    Conservative & outerW2 = cells.at(k + 2 * Def::xCells).w;

    Bound::inlet2ndOrder(innerW1, outerW1, outerW2);
  }
}

void NACA::updateOutlet (std::vector<Cell> & cells)
{
  for (int j = 0; j < Def::yInner; ++j) {
    int k = Def::firstInner + j * Def::xCells;

    const Conservative & innerW1 = cells.at(k).w;
    Conservative & outerW1 = cells.at(k - 1).w;
    Conservative & outerW2 = cells.at(k - 2).w;

    Bound::outlet2ndOrder(innerW1, outerW1, outerW2);
  }

  for (int j = 0; j < Def::yInner; ++j) {
    int k = Def::firstInner + Def::xInner - 1 + j * Def::xCells; // last inner in each row

    const Conservative & innerW1 = cells.at(k).w;
    Conservative & outerW1 = cells.at(k + 1).w;
    Conservative & outerW2 = cells.at(k + 2).w;

    Bound::outlet2ndOrder(innerW1, outerW1, outerW2);
  }
}

void NACA::updateWalls (std::vector<Cell> & cells, const std::vector<Interface> & faces)
{
  for (int i = 0; i < NACA::wingLength; ++i) {
    int k = Def::firstInner + NACA::wingStart + i;

    // there are two faces for every cell - horizontal indices are odd
    Interface face = faces.at(2 * k + 1);

    const Cell & inner2 = cells.at(face.rr);
    const Cell & inner1 = cells.at(face.r);
    Cell & outer1 = cells.at(face.l);
    Cell & outer2 = cells.at(face.ll);

    Bound::wall2ndOrder(face, inner2.w, inner1.w, outer1.w, outer2.w);
  }
}

void NACA::updatePeriodicity (std::vector<Cell> & cells)
{
  // start
  for (int i = 0; i < NACA::wingStart; ++i) {

    // inner points - k1, k2
    int k1 = Def::firstInner + i;
    int k2 = Def::firstInner + i + Def::xCells;

    // indices for ghost cells - l1, l2
    int l1 = Def::firstInner + Def::xInner - Def::xCells - 1 - i;
    int l2 = Def::firstInner + Def::xInner - 2 * Def::xCells - 1 - i;

    // update ghost cells
    cells.at(l1).w = cells.at(k1).w;
    cells.at(l2).w = cells.at(k2).w;

    if (_isnan(cells.at(l1).w.r1)) {
      std::cout << "Naca::periodicity: mame problem";
    }
  }

  // finish
  for (int i = 0; i < NACA::wingStart; ++i) {

    // k1: inner points
    int k1 = Def::firstInner + NACA::wingStart + NACA::wingLength + i;
    int k2 = Def::firstInner + NACA::wingStart + NACA::wingLength + i + Def::xCells;

    // l1: ghost points
    int l1 = Def::firstInner - Def::xCells + NACA::wingStart - 1 - i;
    int l2 = Def::firstInner - 2 * Def::xCells + NACA::wingStart - 1 - i;

    // update ghost cells
    cells.at(l1).w = cells.at(k1).w;
    cells.at(l2).w = cells.at(k2).w;
  }
}

void NACA::updateBounds (std::vector<Cell> & cells, const std::vector<Interface> & faces)
{
  updateInlet(cells);
  updateOutlet(cells);
  updateWalls(cells, faces);
  updatePeriodicity(cells);
}
