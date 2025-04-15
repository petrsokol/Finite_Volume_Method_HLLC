//
// Created by petrs on 01.12.2023.
//

#include "Interface.h"
#include <cmath>
#include <iostream>

/*--------------------------------------------------------------------------------------------------------------------*/

Interface::Interface (double len, double nx, double ny, int ll, int l, int r, int rr, const Point & p1, const Point & p2)
        : line(len, nx, ny), ll(ll), l(l), r(r), rr(rr), p1(p1), p2(p2)
{
}

/*--------------------------------------------------------------------------------------------------------------------*/

std::vector<Interface> Interface::createFaces (const std::vector<Point> & points, const MeshParams & mp)
{
  std::vector<Interface> res = {};

  // create faces for each cell
  for (int j = 0; j < mp.Y_CELLS; ++j) {
    for (int i = 0; i < mp.X_CELLS; ++i) {
      // pair of horizontal and vertical interfaces
      int k = i + j * mp.X_POINTS;
      // vertical goes first - advantages in iteration over inner faces
      const Point & a = points.at(k);
      const Point & b = points.at(k + 1);
      const Point & d = points.at(k + mp.X_POINTS);

      int faceIndex = i + j * mp.X_CELLS;
      res.emplace_back(verticalFace(faceIndex, a, d));
      res.emplace_back(horizontalFace(faceIndex, a, b, mp));
    }
  }
  std::cout << "Loaded " << res.size() << "faces." << std::endl;
  return res;
}

/*--------------------------------------------------------------------------------------------------------------------*/

Interface Interface::horizontalFace (int k, const Point & a, const Point & b, const MeshParams & mp)
{
  // prepare indices for horizontal face (vertical stack of cells)
  int ll = k - 2 * mp.X_CELLS;
  int l = k - mp.X_CELLS;
  int r = k;
  int rr = k + mp.X_CELLS;

  // compute length of interface
  double lx = b.x - a.x;
  double ly = b.y - a.y;
  double len = hypot(lx, ly);

  // normal vector always points towards the RIGHT cell
  double nx = -(ly / len);
  double ny = (lx / len);

  // construct result
  return {len, nx, ny, ll, l, r, rr, a, b};
}

/*--------------------------------------------------------------------------------------------------------------------*/

Interface Interface::verticalFace (int k, const Point & a, const Point & d)
{
  // prepare indices for vertical face (horizontal line of cells)
  int ll = k - 2;
  int l = k - 1;
  int r = k;
  int rr = k + 1;

  // compute length of interface
  double lx = d.x - a.x;
  double ly = d.y - a.y;
  double len = hypot(lx, ly);

  // normal vector always points towards the RIGHT cell
  double nx = ly / len;
  double ny = -(lx / len);

  // construct result
  return {len, nx, ny, ll, l, r, rr, a, d};
}

/*--------------------------------------------------------------------------------------------------------------------*/

const double & Interface::len () const
{
  return line.len;
}

/*--------------------------------------------------------------------------------------------------------------------*/

const double & Interface::nx () const
{
  return line.nx;
}

/*--------------------------------------------------------------------------------------------------------------------*/

const double & Interface::ny () const
{
  return line.ny;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Interface::toString () const
{
  printf("face between cells [ %d : %d : %d : %d ], len = %f, normal vector (%f, %f)\n", ll, l, r, rr, len(), nx(), ny());
}

/*--------------------------------------------------------------------------------------------------------------------*/
