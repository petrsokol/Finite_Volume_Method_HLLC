//
// Created by petrs on 01.12.2023.
//

#include <cmath>
#include "Primitive.h"
#include "../fluid_dynamics/Def.h"

// Constructor definition
Primitive::Primitive(Conservative w)
{
  rho = w.r1;
  rhoU = w.r2;
  rhoV = w.r3;
  rhoE = w.r4;

  u = w.r2 / w.r1;
  v = w.r3 / w.r1;
  U = sqrt(u * u + v * v);
  p = (Def::KAPPA - 1) * (rhoE - 0.5 * rho * U * U);
  c = sqrt((Def::KAPPA * p) / rho);
  h = (Def::KAPPA * p) / (rho * (Def::KAPPA - 1));
}