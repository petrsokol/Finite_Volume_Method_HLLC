//
// Created by petrs on 01.12.2023.
//

#ifndef GAMM_INTERFACE_H
#define GAMM_INTERFACE_H


#include <unordered_map>
#include "Point.h"

class Interface
{

public:
  double len, nx, ny;
  int ll, l, r, rr;

  // Constructor
  Interface () = default;

  Interface (double len, double nx, double ny, int ll, int l, int r, int rr);

  static std::vector<Interface> createFaces (const std::vector<Point> & points, const MeshParams & mp);

  void toString () const;

private:
  static Interface horizontalFace (int k, const Point & a, const Point & b, const MeshParams & mp);

  static Interface verticalFace (int k, const Point & a, const Point & c);
};


#endif //GAMM_INTERFACE_H
