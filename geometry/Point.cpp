//
// Created by petrs on 01.12.2023.
//

#include <fstream>
#include "Point.h"
#include "../fluid_dynamics/Def.h"
#include "../structures/Primitive.h"

const int Point::valueCount = 2;

Point::Point (double x, double y, int index) : x(x), y(y), index(index), values(valueCount, 0), contributors(0)
{}

std::vector<Point> Point::createPoints (std::vector<double> bot, std::vector<double> top)
{
  std::vector<Point> res{};
  std::vector<double> dy{};

  for (int i = 0; i < Def::xPoints; ++i) {
    dy.push_back((top[i] - bot[i]) / Def::yInner);
  }

  for (int i = 0; i < Def::points; ++i) {
    double xx = (i % Def::xPoints - Def::gl) * Def::dx;
    double yy = bot[i % Def::xPoints] + (int(i / Def::xPoints) - Def::gl) * dy[i % Def::xPoints];
    res.emplace_back(xx, yy, i);
  }

  return res;
}

void Point::toString () const
{
  std::cout << "point " << index << ": [" << x << ";" << y << "] \n";
}

Point Point::operator+ (Point other) const
{
  return {Point::x + other.x, Point::y + other.y, -1};
}

Point Point::operator- (Point other) const
{
  return {Point::x - other.x, Point::y - other.y, -1};
}

Point Point::operator* (double scalar) const
{
  return {Point::x * scalar, Point::y * scalar, -1};
}

Point Point::operator/ (double scalar) const
{
  if (scalar == 0.0) {
    std::cerr << "Error: Division by zero\n";
    exit(EXIT_FAILURE);
  }
  return {Point::x / scalar, Point::y / scalar, -1};
}

std::vector<Point> Point::loadPointsFromFile (const std::string & dir, const std::string & file)
{
  std::vector<Point> res;
  std::ifstream input(dir + "\\" + file);
  for (int j = 0; j < Def::yPoints; ++j) {
    for (int i = 0; i < Def::xPoints; ++i) {
      int k = i + j * Def::xPoints;
      double x;
      double y;
      input >> x;
      input >> y;
      int index = pointIndexToCellIndex(i, j);
      res.emplace_back(x, y, k);
    }
  }
  printf("Loaded %llu points.\n", res.size());
  return res;
}

int Point::pointIndexToCellIndex (int i, int j)
{
  int res;
  if (i == Def::xPoints - 1)
    res = -1;
  else if (j == Def::yPoints - 1)
    res = -1;
  else
    res = i + j * Def::xCells;
//  printf("for point [%d, %d] index = %d\n", i, j, res);
  return res;
}
