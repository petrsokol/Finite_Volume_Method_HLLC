//
// Created by petrs on 19.12.2023.
//

#include <cmath>
#include "NACA.h"
#include "Def.h"
#include "../structures/Primitive.h"
#include "Bound.h"

/*--------------------------------------------------------------------------------------------------------------------*/

const int NACA::wingStart = 30;
const int NACA::wingLength = 200; // previous tests with 200 - changed 240506

/*--------------------------------------------------------------------------------------------------------------------*/

// top row of cells
void NACA::updateInlet (std::vector<Cell> & cells)
{
  for (int i = 0; i < Def::xInner; ++i) {
    int k = Def::firstInner + (Def::yInner - 1) * Def::xCells + i;

    const Conservative & innerW1 = cells.at(k).w;
    Conservative & outerW1 = cells.at(k + Def::xCells).w;
    Conservative & outerW2 = cells.at(k + 2 * Def::xCells).w;

    Bound::inlet2ndOrder(innerW1, outerW1, outerW2);
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

// leftmost and rightmost inner columns in cell grid
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
    int k = Def::firstInner + Def::xInner - 1 + j * Def::xCells;

    const Conservative & innerW1 = cells.at(k).w;
    Conservative & outerW1 = cells.at(k + 1).w;
    Conservative & outerW2 = cells.at(k + 2).w;

    Bound::outlet2ndOrder(innerW1, outerW1, outerW2);
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

// part of bottom row of cells
void NACA::updateWalls (std::vector<Cell> & cells, const std::vector<Interface> & faces)
{
  for (int i = 0; i < NACA::wingLength; ++i) {
    int k = Def::firstInner + NACA::wingStart + i;

    Interface face = faces.at(2 * k + 1);

    const Cell & inner2 = cells.at(face.rr);
    const Cell & inner1 = cells.at(face.r);
    Cell & outer1 = cells.at(face.l);
    Cell & outer2 = cells.at(face.ll);

    Bound::wall2ndOrder(face, inner2.w, inner1.w, outer1.w, outer2.w);
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

// remaining part of bottom row of cells
void NACA::updatePeriodicity (std::vector<Cell> & cells)
{
  // start
  for (int i = 0; i < NACA::wingStart; ++i) {
    int k1 = Def::firstInner + i;
    int k2 = Def::firstInner + i + Def::xCells;

    int l1 = Def::firstInner + Def::xInner - Def::xCells - 1 - i;
    int l2 = Def::firstInner + Def::xInner - 2 * Def::xCells - 1 - i;

    cells.at(l1).w = cells.at(k1).w;
    cells.at(l2).w = cells.at(k2).w;
  }

  // finish
  for (int i = 0; i < NACA::wingStart; ++i) {
    int k1 = Def::firstInner + NACA::wingStart + NACA::wingLength + i;
    int k2 = Def::firstInner + NACA::wingStart + NACA::wingLength + i + Def::xCells;

    int l1 = Def::firstInner - Def::xCells + NACA::wingStart - 1 - i;
    int l2 = Def::firstInner - 2 * Def::xCells + NACA::wingStart - 1 - i;

    cells.at(l1).w = cells.at(k1).w;
    cells.at(l2).w = cells.at(k2).w;
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void NACA::updateBounds (std::vector<Cell> & cells, const std::vector<Interface> & faces)
{
  NACA::updateInlet(cells);
  NACA::updateOutlet(cells);
  NACA::updateWalls(cells, faces);
  NACA::updatePeriodicity(cells);
}

/*--------------------------------------------------------------------------------------------------------------------*/
