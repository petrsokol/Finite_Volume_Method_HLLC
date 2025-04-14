//
// Created by petrs on 01.12.2023.
//

#ifndef GAMM_INTERFACE_H
#define GAMM_INTERFACE_H


#include <unordered_map>
#include "Point.h"
#include "Line.h"

class Interface
{

public:
  const Line line;
  const int ll, l, r, rr;

  // Constructors
  Interface (double len, double nx, double ny, int ll, int l, int r, int rr);

  // methods
  static std::vector<Interface> createFaces (const std::vector<Point> & points, const MeshParams & mp);

  void toString () const;

  // forwarders
  [[nodiscard]] const double & len() const;
  [[nodiscard]] const double & nx() const;
  [[nodiscard]] const double & ny() const;

private:
  static Interface horizontalFace (int k, const Point & a, const Point & b, const MeshParams & mp);

  static Interface verticalFace (int k, const Point & a, const Point & d);
};


#endif //GAMM_INTERFACE_H
