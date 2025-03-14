//
// Created by petrs on 19.12.2023.
//

#include <cmath>
#include "NACA.h"
#include "Bound.h"

/*--------------------------------------------------------------------------------------------------------------------*/

const int NACA::WALL_START = 30;
const int NACA::WALL_LENGTH = 200; // previous tests with 200 - changed 240506

/*--------------------------------------------------------------------------------------------------------------------*/

// top row of cells
void NACA::updateInlet (const MeshParams & mp, std::vector<Cell> & cells)
{
  for (int i = 0; i < mp.X_INNER; ++i) {
    int k = mp.FIRST_INNER + (mp.Y_INNER - 1) * mp.X_CELLS + i;

    const Conservative & innerW1 = cells.at(k).w;
    Conservative & outerW1 = cells.at(k + mp.X_CELLS).w;
    Conservative & outerW2 = cells.at(k + 2 * mp.X_CELLS).w;

    Bound::inlet2ndOrder(innerW1, outerW1, outerW2);
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

// leftmost and rightmost inner columns in cell grid
void NACA::updateOutlet (const MeshParams & mp, std::vector<Cell> & cells)
{
  for (int j = 0; j < mp.Y_INNER; ++j) {
    int k = mp.FIRST_INNER + j * mp.X_CELLS;

    const Conservative & innerW1 = cells.at(k).w;
    Conservative & outerW1 = cells.at(k - 1).w;
    Conservative & outerW2 = cells.at(k - 2).w;

    Bound::outlet2ndOrder(innerW1, outerW1, outerW2);
  }

  for (int j = 0; j < mp.Y_INNER; ++j) {
    int k = mp.FIRST_INNER + mp.X_INNER - 1 + j * mp.X_CELLS;

    const Conservative & innerW1 = cells.at(k).w;
    Conservative & outerW1 = cells.at(k + 1).w;
    Conservative & outerW2 = cells.at(k + 2).w;

    Bound::outlet2ndOrder(innerW1, outerW1, outerW2);
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

// part of bottom row of cells
void NACA::updateWalls (const MeshParams & mp, std::vector<Cell> & cells, const std::vector<Interface> & faces)
{
  for (int i = 0; i < NACA::WALL_LENGTH; ++i) {
    int k = mp.FIRST_INNER + NACA::WALL_START + i;

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
void NACA::updatePeriodicity (const MeshParams & mp, std::vector<Cell> & cells)
{
  // start
  for (int i = 0; i < NACA::WALL_START; ++i) {
    int k1 = mp.FIRST_INNER + i;
    int k2 = mp.FIRST_INNER + i + mp.X_CELLS;

    int l1 = mp.FIRST_INNER + mp.X_INNER - mp.X_CELLS - 1 - i;
    int l2 = mp.FIRST_INNER + mp.X_INNER - 2 * mp.X_CELLS - 1 - i;

    cells.at(l1).w = cells.at(k1).w;
    cells.at(l2).w = cells.at(k2).w;
  }

  // finish
  for (int i = 0; i < NACA::WALL_START; ++i) {
    int k1 = mp.FIRST_INNER + NACA::WALL_START + NACA::WALL_LENGTH + i;
    int k2 = mp.FIRST_INNER + NACA::WALL_START + NACA::WALL_LENGTH + i + mp.X_CELLS;

    int l1 = mp.FIRST_INNER - mp.X_CELLS + NACA::WALL_START - 1 - i;
    int l2 = mp.FIRST_INNER - 2 * mp.X_CELLS + NACA::WALL_START - 1 - i;

    cells.at(l1).w = cells.at(k1).w;
    cells.at(l2).w = cells.at(k2).w;
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void NACA::updateBounds (const MeshParams & mp, std::vector<Cell> & cells, const std::vector<Interface> & faces)
{
  NACA::updateInlet(mp, cells);
  NACA::updateOutlet(mp, cells);
  NACA::updateWalls(mp, cells, faces);
  NACA::updatePeriodicity(mp, cells);
}

/*--------------------------------------------------------------------------------------------------------------------*/
