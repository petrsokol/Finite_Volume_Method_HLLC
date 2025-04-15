//
// Created by petrs on 01.12.2023.
//

#include <fstream>
#include <iostream>
#include <filesystem>
#include <cmath>
#include "Point.h"

/*--------------------------------------------------------------------------------------------------------------------*/

Point::Point (double x, double y) : x(x), y(y), w(0, 0, 0, 0)
{
}

/*--------------------------------------------------------------------------------------------------------------------*/

std::vector<Point> Point::loadPointsFromFile (const std::filesystem::path & path, const MeshParams & mp)
{
  std::vector<Point> res;
  std::ifstream input(path);
  for (int j = 0; j < mp.Y_POINTS; ++j) {
    for (int i = 0; i < mp.X_POINTS; ++i) {
      double x;
      double y;
      input >> x;
      input >> y;
      res.emplace_back(x, y);
    }
  }
  std::cout << "Loaded " << res.size() << "points." << std::endl;
  return res;
}

/*--------------------------------------------------------------------------------------------------------------------*/

int Point::pointIndexToCellIndex (int i, int j, const MeshParams & mp)
{
  int res;
  if (i == mp.X_POINTS - 1 || j == mp.Y_POINTS - 1)
    res = -1;
  else
    res = i + j * mp.X_CELLS;
//  printf("for point [%d, %d] index = %d\n", i, j, res);
  return res;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Point::updateW (const Conservative & cellW)
{
  w += cellW;
  contributors++;
  //std::cout << "Point::updateW value updated, current w = " << w << ", contributors = " << contributors << std::endl;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Point::resetW ()
{
  w = 0;
  contributors = 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

double Point::areaQuadrilateral (const Point & A, const Point & B, const Point & C, const Point & D)
{
  return areaTriangle(A, B, C) + areaTriangle(A, C, D);
}

/*--------------------------------------------------------------------------------------------------------------------*/

double Point::areaTriangle (const Point & A, const Point & B, const Point & C)
{
  return 0.5 * fabs(A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y));
}

/*--------------------------------------------------------------------------------------------------------------------*/

Point Point::centroidQuadrilateral (const Point & A, const Point & B, const Point & C, const Point & D)
{
  double x1 = 1.0 / 3 * (A.x + B.x + C.x);
  double y1 = 1.0 / 3 * (A.y + B.y + C.y);
  double A1 = Point::areaTriangle(A, B, C);

  double x2 = 1.0 / 3 * (A.x + C.x + D.x);
  double y2 = 1.0 / 3 * (A.y + C.y + D.y);
  double A2 = Point::areaTriangle(A, C, D);

  double tx = (A1 * x1 + A2 * x2) / (A1 + A2);
  double ty = (A1 * y1 + A2 * y2) / (A1 + A2);

  return {tx, ty};
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Point::toString () const
{
  std::cout << "point" << ": [" << x << ";" << y << "] \n";
}

/*--------------------------------------------------------------------------------------------------------------------*/
// OPERATOR OVERLOADING
/*--------------------------------------------------------------------------------------------------------------------*/

Point Point::operator+ (Point other) const
{
  return {Point::x + other.x, Point::y + other.y};
}

/*--------------------------------------------------------------------------------------------------------------------*/

Point Point::operator- (Point other) const
{
  return {Point::x - other.x, Point::y - other.y};
}

/*--------------------------------------------------------------------------------------------------------------------*/

Point Point::operator* (double scalar) const
{
  return {Point::x * scalar, Point::y * scalar};
}

/*--------------------------------------------------------------------------------------------------------------------*/

Point Point::operator/ (double scalar) const
{
  if (scalar == 0.0) {
    std::cerr << "Error: Division by zero\n";
    exit(EXIT_FAILURE);
  }
  return {Point::x / scalar, Point::y / scalar};
}

/*--------------------------------------------------------------------------------------------------------------------*/