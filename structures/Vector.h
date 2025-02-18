//
// Created by petrs on 02.12.2023.
//

#ifndef GAMM_VECTOR_H
#define GAMM_VECTOR_H


#include "../geometry/Point.h"

class Vector
{

public:
  double x, y, length, ux, uy;

  // Constructors
  Vector () = default;

  Vector (Point a, Point b);
};


#endif //GAMM_VECTOR_H
