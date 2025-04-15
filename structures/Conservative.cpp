//
// Created by petrs on 01.12.2023.
//

#include "Conservative.h"

Conservative operator*(double scalar, const Conservative& vec) {
    return vec * scalar;
}

std::ostream& operator<<(std::ostream& os, const Conservative& obj)
{
  os << "[" << obj.r1 << ", " << obj.r2 << ", " << obj.r3 << ", " << obj.r4 << "]";
  return os;
}