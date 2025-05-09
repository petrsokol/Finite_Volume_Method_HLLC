//
// Created by petrs on 08.05.2025.
//

#include "BLASIUS.h"
#include "Bound.h"

/*--------------------------------------------------------------------------------------------------------------------*/

const int BLASIUS::WALL_START = 50;
const int BLASIUS::WALL_LENGTH = 50;

/*--------------------------------------------------------------------------------------------------------------------*/

void BLASIUS::updateInlet (const MeshParams & mp, std::vector<Cell> & cells)
{
  for (int j = 0; j < mp.Y_INNER; ++j) {
    int k = mp.FIRST_INNER + j * mp.X_CELLS;

    const Conservative & innerW1 = cells.at(k).w;
    Conservative & outerW1 = cells.at(k - 1).w;
    Conservative & outerW2 = cells.at(k - 2).w;

    Bound::inlet2ndOrder(innerW1, outerW1, outerW2);
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void BLASIUS::updateOutlet (const MeshParams & mp, std::vector<Cell> & cells)
{
  for (int j = 0; j < mp.Y_INNER; ++j) {
    int k = mp.FIRST_INNER + mp.X_INNER - 1 + j * mp.X_CELLS;

    const Conservative & innerW1 = cells.at(k).w;
    Conservative & outerW1 = cells.at(k + 1).w;
    Conservative & outerW2 = cells.at(k + 2).w;

    Bound::outlet2ndOrder(innerW1, outerW1, outerW2);
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void BLASIUS::updateWalls (const MeshParams & mp, std::vector<Cell> & cells, const std::vector<Interface> & faces)
{
  // bottom wall
  for (int i = 0; i < BLASIUS::WALL_LENGTH; ++i) {
    int k = mp.FIRST_INNER + BLASIUS::WALL_START + i;

    // there are two faces for every cell - horizontal indices are odd
    Interface face = faces.at(2 * k + 1);

    Cell & outer2 = cells.at(face.ll);
    Cell & outer1 = cells.at(face.l);
    const Cell & inner1 = cells.at(face.r);
    const Cell & inner2 = cells.at(face.rr);

    Bound::wall2ndOrder(inner2.w, inner1.w, outer1.w, outer2.w);
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void BLASIUS::updateSymmetry (const MeshParams & mp, std::vector<Cell> & cells, const std::vector<Interface> & faces)
{
  // top border
  for (int i = 0; i < mp.X_INNER; ++i) {
    int k = mp.FIRST_INNER + mp.Y_INNER * mp.X_CELLS + i;

    // there are two faces for every cell - horizontal indices are odd
    Interface face = faces.at(2 * k + 1);

    Cell & outer2 = cells.at(face.rr);
    Cell & outer1 = cells.at(face.r);
    const Cell & inner1 = cells.at(face.l);
    const Cell & inner2 = cells.at(face.ll);

    Bound::symmetry2ndOrder(face, inner2.w, inner1.w, outer1.w, outer2.w);
  }

  // bottom border
  for (int i = 0; i < BLASIUS::WALL_START; ++i) {
    int k = mp.FIRST_INNER + i;

    // there are two faces for every cell - horizontal indices are odd
    Interface face = faces.at(2 * k + 1);

    Cell & outer2 = cells.at(face.ll);
    Cell & outer1 = cells.at(face.l);
    const Cell & inner1 = cells.at(face.r);
    const Cell & inner2 = cells.at(face.rr);

    Bound::symmetry2ndOrder(face, inner2.w, inner1.w, outer1.w, outer2.w);
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void BLASIUS::updateBounds (const MeshParams & mp, std::vector<Cell> & cells, const std::vector<Interface> & faces)
{
  updateWalls(mp, cells, faces);
  updateInlet(mp, cells);
  updateOutlet(mp, cells);
  updateSymmetry(mp, cells, faces);
}

