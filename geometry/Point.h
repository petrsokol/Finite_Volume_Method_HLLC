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
  // point coordinates
  const double x, y;

  // attributes for post-processing
  static constexpr int valueCount = 2;
  // values[0] -> mach
  // values[1] -> c_p
  double values[valueCount];
  int contributors;

  // Constructor
  Point (double x, double y);

  // Methods
  void toString () const;

  // methods that could be moved elsewhere
  void updateValues(double mach, double cp);

  static std::vector<Point>
  loadPointsFromFile (const std::string & dir, const std::string & file, const MeshParams & mp);

  static std::vector<Point>
  loadPointsFromFile (const std::string & completeDir, const MeshParams & mp);

  static int pointIndexToCellIndex (int i, int j, const MeshParams & mp);

  // Overloaded operators
  Point operator+ (Point other) const;

  Point operator- (Point other) const;

  Point operator* (double scalar) const;

  Point operator/ (double scalar) const;
};


#endif //GAMM_POINT_H
