//
// Created by petrs on 01.12.2023.
//

#ifndef GAMM_PRIMITIVE_H
#define GAMM_PRIMITIVE_H


#include <iostream>
#include "Conservative.h"

class Primitive
{
public:
  double rho, rhoU, rhoV, rhoE;
  double U, u, v, c, p, h;

  // Constructors
  Primitive () = default;

  Primitive (Conservative w);

  // Compound assignment: +=
  Primitive & operator += (const Primitive & other)
  {
    rho += other.rho;
    rhoU += other.rhoU;
    rhoV += other.rhoV;
    rhoE += other.rhoE;
    U += other.U;
    u += other.u;
    v += other.v;
    c += other.c;
    p += other.p;
    h += other.h;
    return *this;
  }

  // Compound assignment: -=
  Primitive & operator -= (const Primitive & other)
  {
    rho -= other.rho;
    rhoU -= other.rhoU;
    rhoV -= other.rhoV;
    rhoE -= other.rhoE;
    U -= other.U;
    u -= other.u;
    v -= other.v;
    c -= other.c;
    p -= other.p;
    h -= other.h;
    return *this;
  }

  // Compound assignment: *= (scalar)
  Primitive & operator *= (double scalar)
  {
    rho *= scalar;
    rhoU *= scalar;
    rhoV *= scalar;
    rhoE *= scalar;
    U *= scalar;
    u *= scalar;
    v *= scalar;
    c *= scalar;
    p *= scalar;
    h *= scalar;
    return *this;
  }

  // Compound assignment: /= (scalar)
  Primitive & operator /= (double scalar)
  {
    rho /= scalar;
    rhoU /= scalar;
    rhoV /= scalar;
    rhoE /= scalar;
    U /= scalar;
    u /= scalar;
    v /= scalar;
    c /= scalar;
    p /= scalar;
    h /= scalar;
    return *this;
  }

  // Arithmetic operator: +
  Primitive operator + (const Primitive & other) const
  {
    Primitive result = *this;
    result += other;
    return result;
  }

  // Arithmetic operator: -
  Primitive operator - (const Primitive & other) const
  {
    Primitive result = *this;
    result -= other;
    return result;
  }

  // Arithmetic operator: * (scalar)
  Primitive operator * (double scalar) const
  {
    Primitive result = *this;
    result *= scalar;
    return result;
  }

  // Arithmetic operator: / (scalar)
  Primitive operator / (double scalar) const
  {
    Primitive result = *this;
    result /= scalar;
    return result;
  }

  // Scalar multiplication from left side
  friend Primitive operator * (double scalar, const Primitive & primitive)
  {
    return primitive * scalar;
  }

  // Assign scalar to all members
  Primitive & operator = (double scalar)
  {
    rho = scalar;
    rhoU = scalar;
    rhoV = scalar;
    rhoE = scalar;
    U = scalar;
    u = scalar;
    v = scalar;
    c = scalar;
    p = scalar;
    h = scalar;
    return *this;
  }
};


#endif //GAMM_PRIMITIVE_H
