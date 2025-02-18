//
// Created by petrs on 01.12.2023.
//

#include "Interface.h"
#include "../fluid_dynamics/Def.h"
#include <cmath>

void Interface::toString () const
{
  printf("face between cells [ %d : %d : %d : %d ], len = %f, normal vector (%f, %f)\n", ll, l, r, rr, len, nx, ny);
}

std::vector<Interface> Interface::createFaces (const std::vector<Point> & points)
{
  std::vector<Interface> res = {};

  // create faces for each cell
  for (int j = 0; j < Def::yCells; ++j) {
    for (int i = 0; i < Def::xCells; ++i) {
      // pair of horizontal and vertical interfaces
      int k = i + j * Def::xPoints;
      // vertical goes first - advantages in iteration over inner faces
      const Point & a = points.at(k);
      const Point & b = points.at(k + 1);
      const Point & c = points.at(k + Def::xPoints);
      int faceIndex = i + j * Def::xCells;
      res.emplace_back(verticalFace(faceIndex, a, c));
      res.emplace_back(horizontalFace(faceIndex, a, b));
    }
  }

  printf("Loaded %llu faces.\n", res.size());
  return res;
}

Interface Interface::horizontalFace (int k, const Point & a, const Point & b)
{
  // prepare indices for horizontal face (vertical stack of cells)
  int ll = k - 2 * Def::xCells;
  int l = k - Def::xCells;
  int r = k;
  int rr = k + Def::xCells;

  // compute length of interface
  double lx = b.x - a.x;
  double ly = b.y - a.y;
  double len = sqrt(lx * lx + ly * ly);

  // normal vector always points towards the RIGHT cell
  double nx = -(ly / len);
  double ny = (lx / len);

  // construct result
  return {len, nx, ny, ll, l, r, rr};
}

Interface Interface::verticalFace (int k, const Point & a, const Point & c)
{
  // prepare indices for vertical face (horizontal line of cells)
  int ll = k - 2;
  int l = k - 1;
  int r = k;
  int rr = k + 1;

  // compute length of interface
  double lx = c.x - a.x;
  double ly = c.y - a.y;
  double len = sqrt(lx * lx + ly * ly);

  // normal vector always points towards the RIGHT cell
  double nx = ly / len;
  double ny = -(lx / len);

  // construct result
  return {len, nx, ny, ll, l, r, rr};
}

Interface::Interface (double len, double nx, double ny, int ll, int l, int r, int rr)
        : len(len), nx(nx), ny(ny), ll(ll), l(l), r(r), rr(rr)
{}
