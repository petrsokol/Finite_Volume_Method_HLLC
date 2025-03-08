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
  std::vector<double> values;
  int contributors;

  // Constructor
  Point (double x, double y, int index);

  // Methods
  void toString () const;

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
