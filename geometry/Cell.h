//
// Created by petrs on 01.12.2023.
//

#ifndef GAMM_CELL_H
#define GAMM_CELL_H


#include <unordered_map>
#include "../structures/Vector.h"
#include "../structures/Conservative.h"
#include "Point.h"

class Cell
{

public:
  Conservative w, rezi;
  double area, tx, ty, dt;
  Vector xi{}, eta{};
  int index;

  // Constructor
  Cell () = default;

  Cell (const Point& a, const Point& b, const Point& c, const Point& d);

  static std::vector<Cell> createCells (const std::vector<Point> & points);

  void toString () const;
};


#endif //GAMM_CELL_H
