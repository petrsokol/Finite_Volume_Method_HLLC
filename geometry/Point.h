//
// Created by petrs on 01.12.2023.
//

#ifndef GAMM_POINT_H
#define GAMM_POINT_H


#include <vector>
#include <string>
#include <filesystem>
#include "MeshParams.h"
#include "../structures/Conservative.h"

class Point
{

public:
  // point coordinates
  const double x, y;

  // attributes for post-processing
  static constexpr int valueCount = 2;
  // values[0] -> mach
  // values[1] -> c_p
  double values[valueCount];
  int contributors;

  // attributes for viscous terms
  Conservative w;

  // Constructor
  Point (double x, double y);

  // Methods
  void toString () const;

  static int pointIndexToCellIndex (int i, int j, const MeshParams & mp);

  // methods that could be moved elsewhere
  void updateValues (double mach, double cp);

  void updateW (const Conservative & cellW);

  void resetW ();

  static std::vector<Point>
  loadPointsFromFile (const std::filesystem::path & path, const MeshParams & mp);

  static double areaQuadrilateral (const Point & A, const Point & B, const Point & C, const Point & D);

  static double areaTriangle (const Point & A, const Point & B, const Point & C);

  static Point centroidQuadrilateral (const Point & A, const Point & B, const Point & C, const Point & D);

  // Overloaded operators
  Point operator + (Point other) const;

  Point operator - (Point other) const;

  Point operator * (double scalar) const;

  Point operator / (double scalar) const;
};


#endif //GAMM_POINT_H
