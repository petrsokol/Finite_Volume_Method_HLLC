//
// Created by petrs on 01.12.2023.
//

#include <fstream>
#include "Point.h"
#include "../fluid_dynamics/Def.h"
#include "../structures/Primitive.h"

const int Point::valueCount = 2;

/*--------------------------------------------------------------------------------------------------------------------*/

Point::Point (double x, double y, int index) : x(x), y(y), index(index), values(valueCount, 0), contributors(0)
{}

/*--------------------------------------------------------------------------------------------------------------------*/

void Point::toString () const
{
  std::cout << "point " << index << ": [" << x << ";" << y << "] \n";
}

/*--------------------------------------------------------------------------------------------------------------------*/

Point Point::operator+ (Point other) const
{
  return {Point::x + other.x, Point::y + other.y, -1};
}

/*--------------------------------------------------------------------------------------------------------------------*/

Point Point::operator- (Point other) const
{
  return {Point::x - other.x, Point::y - other.y, -1};
}

/*--------------------------------------------------------------------------------------------------------------------*/

Point Point::operator* (double scalar) const
{
  return {Point::x * scalar, Point::y * scalar, -1};
}

/*--------------------------------------------------------------------------------------------------------------------*/

Point Point::operator/ (double scalar) const
{
  if (scalar == 0.0) {
    std::cerr << "Error: Division by zero\n";
    exit(EXIT_FAILURE);
  }
  return {Point::x / scalar, Point::y / scalar, -1};
}

/*--------------------------------------------------------------------------------------------------------------------*/

std::vector<Point> Point::loadPointsFromFile (const std::string & dir, const std::string & file, const MeshParams & mp)
{
  std::vector<Point> res;
  std::ifstream input(dir + "\\" + file);
  for (int j = 0; j < mp.Y_POINTS; ++j) {
    for (int i = 0; i < mp.X_POINTS; ++i) {
      int k = i + j * mp.X_POINTS;
      double x;
      double y;
      input >> x;
      input >> y;
      int index = pointIndexToCellIndex(i, j, mp);
      res.emplace_back(x, y, k);
    }
  }
  printf("Loaded %llu points.\n", res.size());
  return res;
}

/*--------------------------------------------------------------------------------------------------------------------*/

int Point::pointIndexToCellIndex (int i, int j, const MeshParams & mp)
{
  int res;
  if (i == mp.X_POINTS - 1)
    res = -1;
  else if (j == mp.Y_POINTS - 1)
    res = -1;
  else
    res = i + j * mp.X_CELLS;
//  printf("for point [%d, %d] index = %d\n", i, j, res);
  return res;
}

/*--------------------------------------------------------------------------------------------------------------------*/
