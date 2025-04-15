//
// Created by petrs on 01.12.2023.
//

#ifndef GAMM_INTERFACE_H
#define GAMM_INTERFACE_H


#include "Point.h"
#include "Line.h"
#include "Cell.h"

class Interface
{

public:
  const Line line;
  const int ll, l, r, rr;

  // attributes for viscous terms
  const Point & p1; // left / bottom point
  const Point & p2; // right / top point
  const double area; // area of the dual cell

  // Constructors
  Interface (double len, double nx, double ny, int ll, int l, int r, int rr, const Point & p1, const Point & p2);

  Interface (Line line, int ll, int l, int r, int rr, const Point & p1, const Point & p2, double area);

  Interface (Line line, int ll, int l, int r, int rr, const Point & p1, const Point & p2);

  // methods
  static std::vector<Interface> createFaces (const std::vector<Point> & points, const MeshParams & mp);

  void toString () const;

  // forwarders
  [[nodiscard]] const double & len () const;

  [[nodiscard]] const double & nx () const;

  [[nodiscard]] const double & ny () const;

private:
  static Interface constructHorizontal (int k, const Point & A, const Point & B, const MeshParams & mp);

  static Interface constructHorizontalFromSixPoints (int k, const MeshParams & mp,
                                                     const Point & A, const Point & B, const Point & C,
                                                     const Point & D, const Point & E, const Point & F);

  static Interface constructVertical (int k, const Point & A, const Point & d);

  static Interface constructVerticalFromSixPoints (int k, const MeshParams & mp,
                                                   const Point & A, const Point & B, const Point & C,
                                                   const Point & D, const Point & E, const Point & F);
};


#endif //GAMM_INTERFACE_H
