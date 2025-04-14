//
// Created by petrs on 01.12.2023.
//

#ifndef GAMM_CELL_H
#define GAMM_CELL_H


#include <unordered_map>
#include "../structures/Vector.h"
#include "../structures/Conservative.h"
#include "Point.h"

struct GeometryData
{
  double area = 0;
  double tx = 0;
  double ty = 0;
  Vector xi{}, eta{};
};

class Cell
{

public:
  // const attributes
  const double area;
  const double tx, ty;
  const Vector xi{}, eta{};

  // variable attributes
  Conservative w;
  Conservative rezi;
  double dt;

  // Constructor
  Cell (const Point & a, const Point & b, const Point & c, const Point & d);

  explicit Cell (GeometryData gd);

  static std::vector<Cell> createCells (const std::vector<Point> & points, const MeshParams & mp);

  void toString () const;

private:
  static GeometryData computeGeometry(const Point & a, const Point & b, const Point & c, const Point & d);
};


#endif //GAMM_CELL_H
