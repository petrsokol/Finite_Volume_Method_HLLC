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
    Bound::inlet2ndOrder(cells[k].w, cells[k - 1].w, cells[k - 2].w);
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void GAMM::updateOutlet (std::vector<Cell> & cells)
{
  for (int j = 0; j < Def::yInner; ++j) {
    int k = Def::firstInner + Def::xInner - 1 + j * Def::xCells;
    Bound::outlet2ndOrder(cells[k].w, cells[k + 1].w, cells[k + 2].w);
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void GAMM::updateWalls (std::vector<Cell> & cells, const std::vector<Interface> & faces)
{
  // bottom wall
  for (int i = 0; i < Def::xInner; ++i) {
    int k = Def::firstInner + i;

    // there are two faces for every cell - horizontal indices are odd
    Interface face = faces[2 * k + 1];
    Cell & outer2 = cells[face.ll];
    Cell & outer1 = cells[face.l];
    Cell & inner1 = cells[face.r];
    Cell & inner2 = cells[face.rr];
    Bound::wall2ndOrder(face, inner2.w, inner1.w, outer1.w, outer2.w);
  }

  // top wall
  for (int i = 0; i < Def::xInner; ++i) {
    int k = Def::firstInner + (Def::yInner) * Def::xCells + i;

    // there are two faces for every cell - horizontal indices are odd
    Interface face = faces[2 * k + 1];
    Cell & outer2 = cells[face.rr];
    Cell & outer1 = cells[face.r];
    Cell & inner1 = cells[face.l];
    Cell & inner2 = cells[face.ll];
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
