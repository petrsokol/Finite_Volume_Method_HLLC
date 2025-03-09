//
// Created by petrs on 01.12.2023.
//

#ifndef GAMM_POINT_H
#define GAMM_POINT_H


#include <vector>
#include <string>
#include "MeshParams.h"

class Point
{

public:
  static const int valueCount;
  double x, y;
  // values[0] -> mach
  // values[1] -> c_p
  std::vector<double> values;
  int contributors;

  // Constructor
  Point (double x, double y, int index);

  // Methods
  void toString () const;

  void updateValues(double mach, double cp);

  // Overloaded operators
  Point operator+ (Point other) const;

  Point operator- (Point other) const;

  Point operator* (double scalar) const;

  Point operator/ (double scalar) const;

  static std::vector<Point>
  loadPointsFromFile (const std::string & dir, const std::string & file, const MeshParams & mp);

  static int pointIndexToCellIndex (int i, int j, const MeshParams & mp);
};


#endif //GAMM_POINT_H
