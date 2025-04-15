//
// Created by petrs on 01.12.2023.
//

#include <fstream>
#include <iostream>
#include <filesystem>
#include "Point.h"

/*--------------------------------------------------------------------------------------------------------------------*/

Point::Point (double x, double y) : x(x), y(y), values(0, 0), contributors(0)
{
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Point::toString () const
{
  std::cout << "point" << ": [" << x << ";" << y << "] \n";
}

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

std::vector<Point> Point::loadPointsFromFile (const std::filesystem::path & path, const std::string & fileName, const MeshParams & mp)
{
  return loadPointsFromFile(path / fileName, mp);
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

void Point::updateValues (double mach, double cp)
{
  values[0] += mach;
  values[1] += cp;
  contributors++;
}

/*--------------------------------------------------------------------------------------------------------------------*/
