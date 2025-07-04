//
// Created by petrs on 27.04.2024.
//

#include "../geometry/Cell.h"
#include "../geometry/Interface.h"
#include "GAMM.h"
#include "Bound.h"

/*--------------------------------------------------------------------------------------------------------------------*/

const int GAMM::WALL_START = 0;
const int GAMM::WALL_LENGTH = 150; // previous tests with 200 - changed 240506

/*--------------------------------------------------------------------------------------------------------------------*/

void GAMM::updateInlet (const MeshParams & mp, std::vector<Cell> & cells)
{
  for (int j = 0; j < mp.Y_INNER; ++j) {
    int k = mp.FIRST_INNER + j * mp.X_CELLS;

    const Conservative & innerW1 = cells.at(k).w;
    Conservative & outerW1 = cells.at(k - 1).w;
    Conservative & outerW2 = cells.at(k - 2).w;

    Bound::inlet2ndOrder(innerW1, outerW1, outerW2);

    /*
    const Cell & c = cells.at(k - 1);
    printf("INLET cell at [%.2f, %.2f] has Conservative values of [%.4f, %.4f, %.4f, %.4f]\n",
           c.tx, c.ty,
           c.w.r1, c.w.r2, c.w.r3, c.w.r4);
    */
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void GAMM::updateOutlet (const MeshParams & mp, std::vector<Cell> & cells)
{
  for (int j = 0; j < mp.Y_INNER; ++j) {
    int k = mp.FIRST_INNER + mp.X_INNER - 1 + j * mp.X_CELLS;

    const Conservative & innerW1 = cells.at(k).w;
    Conservative & outerW1 = cells.at(k + 1).w;
    Conservative & outerW2 = cells.at(k + 2).w;

    Bound::outlet2ndOrder(innerW1, outerW1, outerW2);

    /*
    const Cell & c = cells.at(k + 1);
    printf("OUTLET cell at [%.2f, %.2f] has Conservative values of [%.4f, %.4f, %.4f, %.4f]\n",
           c.tx, c.ty,
           c.w.r1, c.w.r2, c.w.r3, c.w.r4);
    */
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void GAMM::updateSymmetry (const MeshParams & mp, std::vector<Cell> & cells, const std::vector<Interface> & faces)
{
  // bottom wall
  for (int i = 0; i < mp.X_INNER; ++i) {
    int k = mp.FIRST_INNER + i;

    // there are two faces for every cell - horizontal indices are odd
    Interface face = faces.at(2 * k + 1);

    Cell & outer2 = cells.at(face.ll);
    Cell & outer1 = cells.at(face.l);
    const Cell & inner1 = cells.at(face.r);
    const Cell & inner2 = cells.at(face.rr);

    Bound::symmetry2ndOrder(face, inner2.w, inner1.w, outer1.w, outer2.w);

    /*
    const Cell & c = outer1;
    printf("WALL cell at [%.2f, %.2f] has Conservative values of [%.4f, %.4f, %.4f, %.4f]\n",
           c.tx, c.ty,
           c.w.r1, c.w.r2, c.w.r3, c.w.r4);
    */
  }

  // top wall
  for (int i = 0; i < mp.X_INNER; ++i) {
    int k = mp.FIRST_INNER + mp.Y_INNER * mp.X_CELLS + i;

    // there are two faces for every cell - horizontal indices are odd
    Interface face = faces.at(2 * k + 1);

    Cell & outer2 = cells.at(face.rr);
    Cell & outer1 = cells.at(face.r);
    const Cell & inner1 = cells.at(face.l);
    const Cell & inner2 = cells.at(face.ll);

    Bound::symmetry2ndOrder(face, inner2.w, inner1.w, outer1.w, outer2.w);

    /*
    const Cell & c = outer1;
    printf("WALL cell at [%.2f, %.2f] has Conservative values of [%.4f, %.4f, %.4f, %.4f]\n",
           c.tx, c.ty,
           c.w.r1, c.w.r2, c.w.r3, c.w.r4);
           */
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void GAMM::updateBounds (const MeshParams & mp, std::vector<Cell> & cells, const std::vector<Interface> & faces)
{
  updateInlet(mp, cells);
  updateOutlet(mp, cells);
  updateSymmetry(mp, cells, faces);
}

/*--------------------------------------------------------------------------------------------------------------------*/
