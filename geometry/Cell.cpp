//
// Created by petrs on 01.12.2023.
//

#include <cmath>
#include "Cell.h"

/*--------------------------------------------------------------------------------------------------------------------*/

GeometryData Cell::computeGeometry (const Point & a, const Point & b, const Point & c, const Point & d)
{
  double x1 = 1.0 / 3 * (a.x + b.x + c.x);
  double y1 = 1.0 / 3 * (a.y + b.y + c.y);
  double A1 = 0.5 * fabs(a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y));

  double x2 = 1.0 / 3 * (a.x + c.x + d.x);
  double y2 = 1.0 / 3 * (a.y + c.y + d.y);
  double A2 = 0.5 * fabs(a.x * (c.y - d.y) + c.x * (d.y - a.y) + d.x * (a.y - c.y));

  double area = A1 + A2;
  double tx = (A1 * x1 + A2 * x2) / (A1 + A2);
  double ty = (A1 * y1 + A2 * y2) / (A1 + A2); //ChatGPT

  Vector xi = Vector((a + d) / 2, (b + c) / 2);
  Vector eta = Vector((c + d) / 2, (a + b) / 2);

  Line l1(a.x, a.y, tx, ty);
  Line l2(b.x, b.y, tx, ty);
  Line l3(tx, ty, c.x, c.y);
  Line l4(tx, ty, d.x, d.y);

  return {area, tx, ty, xi, eta, l1, l2, l3, l4};
  // struct created according to chatgpt recommendation
}

/*--------------------------------------------------------------------------------------------------------------------*/

Cell::Cell (const Point & a, const Point & b, const Point & c, const Point & d)
        : Cell(computeGeometry(a, b, c, d))
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
      const Point & a = points.at(j * (mp.X_POINTS) + i);
      const Point & b = points.at(j * (mp.X_POINTS) + i + 1);
      const Point & c = points.at((j + 1) * (mp.X_POINTS) + i + 1);
      const Point & d = points.at((j + 1) * (mp.X_POINTS) + i);

      Cell curr = Cell(a, b, c, d);
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
