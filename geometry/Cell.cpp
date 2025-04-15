//
// Created by petrs on 01.12.2023.
//

#include <cmath>
#include "Cell.h"

/*--------------------------------------------------------------------------------------------------------------------*/

/*
 * point distribution:
 * D --- C
 * |  T  |
 * A --- B
 */
GeometryData Cell::computeGeometry (const Point & A, const Point & B, const Point & C, const Point & D)
{
  Point T = Point::centroidQuadrilateral(A, B, C, D);
  double area = Point::areaQuadrilateral(A, B, C, D);

  Vector xi = Vector((A + D) / 2, (B + C) / 2);
  Vector eta = Vector((C + D) / 2, (A + B) / 2);

  Line l1(A.x, A.y, T.x, T.y);
  Line l2(B.x, B.y, T.x, T.y);
  Line l3(T.x, T.y, C.x, C.y);
  Line l4(T.x, T.y, D.x, D.y);

  return {area, T.x, T.y, xi, eta, l1, l2, l3, l4};
  // struct created according to chatgpt recommendation
}

/*--------------------------------------------------------------------------------------------------------------------*/

Cell::Cell (const Point & A, const Point & B, const Point & C, const Point & D)
        : Cell(computeGeometry(A, B, C, D))
{
}

/*--------------------------------------------------------------------------------------------------------------------*/

Cell::Cell (GeometryData g)
        : area(g.area), tx(g.tx), ty(g.ty), xi(g.xi), eta(g.eta),
          l1(g.l1), l2(g.l2), l3(g.l3), l4(g.l4)
{
  w = 0;
  rezi = 0;
  dt = 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

std::vector<Cell> Cell::createCells (const std::vector<Point> & points, const MeshParams & mp)
{
  std::vector<Cell> res;
  for (int j = 0; j < mp.Y_CELLS; ++j) {
    for (int i = 0; i < mp.X_CELLS; ++i) {
      const Point & A = points.at(j * (mp.X_POINTS) + i);
      const Point & B = points.at(j * (mp.X_POINTS) + i + 1);
      const Point & C = points.at((j + 1) * (mp.X_POINTS) + i + 1);
      const Point & D = points.at((j + 1) * (mp.X_POINTS) + i);

      Cell curr = Cell(A, B, C, D);
      res.push_back(curr);
    }
  }
  std::cout << "Loaded " << res.size() << "cells." << std::endl;
  return res;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Cell::toString () const
{
  std::cout << "Cell: "
            << "area = " << area << ", "
            << "T = [" << tx << ";" << ty << "], "
            << "W = ";
  w.toString();
}

/*--------------------------------------------------------------------------------------------------------------------*/
