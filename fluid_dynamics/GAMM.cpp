//
// Created by petrs on 27.04.2024.
//

#include "../geometry/Cell.h"
#include "../geometry/Interface.h"
#include "GAMM.h"
#include "Def.h"
#include "Bound.h"

/*--------------------------------------------------------------------------------------------------------------------*/

void GAMM::updateInlet (std::vector<Cell> & cells)
{
  for (int j = 0; j < Def::yInner; ++j) {
    int k = Def::firstInner + j * Def::xCells;
    Bound::inlet2ndOrder(cells.at(k).w, cells.at(k - 1).w, cells.at(k - 2).w);
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void GAMM::updateOutlet (std::vector<Cell> & cells)
{
  for (int j = 0; j < Def::yInner; ++j) {
    int k = Def::firstInner + Def::xInner - 1 + j * Def::xCells;
    Bound::outlet2ndOrder(cells.at(k).w, cells.at(k + 1).w, cells.at(k + 2).w);
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void GAMM::updateWalls (std::vector<Cell> & cells, const std::vector<Interface> & faces)
{
  // bottom wall
  for (int i = 0; i < Def::xInner; ++i) {
    int k = Def::firstInner + i;

    // there are two faces for every cell - horizontal indices are odd
    Interface face = faces.at(2 * k + 1);
    Cell & outer2 = cells.at(face.ll);
    Cell & outer1 = cells.at(face.l);
    const Cell & inner1 = cells.at(face.r);
    const Cell & inner2 = cells.at(face.rr);
    Bound::wall2ndOrder(face, inner2.w, inner1.w, outer1.w, outer2.w);
  }

  // top wall
  for (int i = 0; i < Def::xInner; ++i) {
    int k = Def::firstInner + (Def::yInner) * Def::xCells + i;

    // there are two faces for every cell - horizontal indices are odd
    Interface face = faces.at(2 * k + 1);
    Cell & outer2 = cells.at(face.rr);
    Cell & outer1 = cells.at(face.r);
    const Cell & inner1 = cells.at(face.l);
    const Cell & inner2 = cells.at(face.ll);
    Bound::wall2ndOrder(face, inner2.w, inner1.w, outer1.w, outer2.w);
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void GAMM::updateBounds (std::vector<Cell> & cells, const std::vector<Interface> & faces)
{
  updateInlet(cells);
  updateOutlet(cells);
  updateWalls(cells, faces);
}

/*--------------------------------------------------------------------------------------------------------------------*/
