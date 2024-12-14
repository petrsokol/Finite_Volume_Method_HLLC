//
// Created by petrs on 27.04.2024.
//

#include <unordered_map>
#include "../geometry/Cell.h"
#include "../geometry/Interface.h"
#include "GAMM.h"
#include "Def.h"
#include "Bound.h"

void GAMM::updateInlet (std::vector<Cell> & cells)
{
  for (int j = 0; j < Def::yInner; ++j) {
    int k = Def::firstInner + j * Def::xCells;

    Conservative innerW = cells.at(k).w;
    Conservative outerW = Bound::updateInletCell(innerW);
    cells.at(k - 1).w = outerW;
  }
}

void GAMM::updateOutlet (std::vector<Cell> & cells)
{
  for (int j = 0; j < Def::yInner; ++j) {
    int k = Def::firstInner + Def::xInner - 1 + j * Def::xCells;

    Conservative innerW = cells.at(k).w;
    Conservative outerW = Bound::updateOutletCell(innerW);
    cells.at(k + 1).w = outerW;
//    printf("GAMM::outlet: moved data from inner cell [%d: %d] to outer cell [%d: %d]\n", k, cells.at(k).index, k + 1, cells.at(k + 1).index);
  }
//  printf("\n");
}

void GAMM::updateWalls (std::vector<Cell> & cells,
                        const std::vector<Interface> & faces)
{
  for (int i = 0; i < Def::xInner; ++i) {
    int k = Def::firstInner + i;

    // there are two faces for every cell - horizontal indices are even
    Interface face = faces.at(2 * k + 1);
    Conservative innerW = cells.at(k).w;
    Conservative outerW = Bound::updateWallCell(innerW, face);
    cells.at(k - Def::xCells).w = outerW;
  }
  for (int i = 0; i < Def::xInner; ++i) {
    int k = Def::firstInner + (Def::yInner) * Def::xCells + i;

    // there are two faces for every cell - horizontal indices are even
    Interface face = faces.at(2 * k + 1);
    Conservative innerW = cells.at(k - Def::xCells).w;
    Conservative outerW = Bound::updateWallCell(innerW, face);
    cells.at(k).w = outerW;
  }
}

void GAMM::updateBounds (std::vector<Cell> & cells, const std::vector<Interface> & faces)
{
  updateInlet(cells);
  updateOutlet(cells);
  updateWalls(cells, faces);
}