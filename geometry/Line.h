//
// Created by petrs on 14.04.2025.
//

#ifndef GAMM_LINE_H
#define GAMM_LINE_H


#include "Point.h"

class Line
{
public:
  // attributes
  const double len, nx, ny;

  // constructors
  Line (double len, double nx, double ny);

  Line (const Point & a, const Point & b);

  Line (double ax, double ay, double bx, double by);
};


#endif //GAMM_LINE_H
