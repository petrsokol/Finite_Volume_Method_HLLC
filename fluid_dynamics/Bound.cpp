//
// Created by petrs on 02.12.2023.
//

#include <cmath>
#include "Bound.h"
#include "Def.h"
#include "../structures/Primitive.h"

/*--------------------------------------------------------------------------------------------------------------------*/

double Bound::p_0 = 0;
double Bound::rho_0 = 0;
double Bound::alpha_0 = 0;
double Bound::M_0 = 0;
double Bound::p_2 = 0;

int Bound::BC_TYPE = -1;

/*--------------------------------------------------------------------------------------------------------------------*/

void Bound::setSubsonicCondition (const double p_0_in, const double rho_0_in,
                                  const double alpha_0_in_degrees, const double p_2_in)
{
  BC_TYPE = SUBSONIC;

  Bound::p_0 = p_0_in;
  Bound::rho_0 = rho_0_in;
  Bound::alpha_0 = degToRad(alpha_0_in_degrees);
  Bound::p_2 = p_2_in;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Bound::setSupersonicCondition (double p_0_in, double rho_0_in, double alpha_0_in_degrees, double M_0_in)
{
  BC_TYPE = SUPERSONIC;

  Bound::p_0 = p_0_in;
  Bound::rho_0 = rho_0_in;
  Bound::alpha_0 = degToRad(alpha_0_in_degrees);
  Bound::M_0 = M_0_in;
}

/*--------------------------------------------------------------------------------------------------------------------*/

Conservative Bound::updateInletCell (const Conservative & innerW)
{
  if (BC_TYPE == SUBSONIC) {
    return updateSubsonicInlet(innerW);
  }

  if (BC_TYPE == SUPERSONIC) {
    return updateSupersonicInlet();
  }

  std::cout << "Bound::updateInletCell: undefined behaviour!!" << std::endl;
  return Conservative(NAN, NAN, NAN, NAN);
}

/*--------------------------------------------------------------------------------------------------------------------*/

Conservative Bound::updateSubsonicInlet (const Conservative & innerW)
{
  // p_infty is extrapolated from the domain
  const double p_infty = Primitive(innerW).p;

  const double rho_infty = get_rho_infty(p_infty);
  const double M_infty = get_M_infty(p_infty);
  const double c_infty = get_c_infty(p_infty, rho_infty);
  const double U_infty = get_U_infty(M_infty, c_infty);
  const double u_infty = get_u_infty(U_infty);
  const double v_infty = get_v_infty(U_infty);
  const double rhoE_infty = get_rhoE_infty(p_infty, U_infty, rho_infty);

  return Conservative(
          rho_infty,
          rho_infty * u_infty,
          rho_infty * v_infty,
          rhoE_infty
  );
}

/*--------------------------------------------------------------------------------------------------------------------*/

Conservative Bound::updateSupersonicInlet ()
{
  // no values are extrapolated from the domain (M_0 is set in the boundary condition

  const double p_infty = get_p_infty();
  const double rho_infty = get_rho_infty(p_infty);
  const double c_infty = get_c_infty(p_infty, rho_infty);
  const double U_infty = get_U_infty(M_0, c_infty);
  const double u_infty = get_u_infty(U_infty);
  const double v_infty = get_v_infty(U_infty);
  const double rhoE_infty = get_rhoE_infty(p_infty, U_infty, rho_infty);

  return Conservative(
          rho_infty,
          rho_infty * u_infty,
          rho_infty * v_infty,
          rhoE_infty
  );
}

/*--------------------------------------------------------------------------------------------------------------------*/

Conservative Bound::updateOutletCell (const Conservative & innerW)
{
  // switch statement?
  if (BC_TYPE == SUBSONIC) {
    return updateSubsonicOutlet(innerW);
  }

  if (BC_TYPE == SUPERSONIC) {
    return updateSupersonicOutlet(innerW);
  }

  std::cout << "Bound::updateOutletCell: undefined behaviour!!" << std::endl;
  return Conservative(NAN, NAN, NAN, NAN);
}

/*--------------------------------------------------------------------------------------------------------------------*/

Conservative Bound::updateSubsonicOutlet (const Conservative & innerW)
{
  // extrapolate three out of four values from the domain, p_2 is set by BC
  const Primitive innerPV(innerW);
  const double & rho_infty = innerPV.rho;
  const double & u_infty = innerPV.u;
  const double & v_infty = innerPV.v;
  const double & U_infty = innerPV.U;

  const double rhoE_infty = get_rhoE_infty(p_2, U_infty, rho_infty);

  return Conservative(
          rho_infty,
          rho_infty * u_infty,
          rho_infty * v_infty,
          rhoE_infty
  );
}

/*--------------------------------------------------------------------------------------------------------------------*/

Conservative Bound::updateSupersonicOutlet (const Conservative & innerW)
{
  // extrapolate all four values from the domain
  return innerW;
}

/*--------------------------------------------------------------------------------------------------------------------*/

Conservative Bound::updateSymmetryCell (const Conservative & innerW, const Interface & f)
{
  const double uInner = innerW.r2 / innerW.r1;
  const double vInner = innerW.r3 / innerW.r1;
  const double nx = f.nx();
  const double ny = f.ny();

  Conservative outerW;
  outerW.r1 = innerW.r1;
  outerW.r2 = innerW.r1 * (uInner - 2 * (uInner * nx + vInner * ny) * nx);
  outerW.r3 = innerW.r1 * (vInner - 2 * (uInner * nx + vInner * ny) * ny);
  outerW.r4 = innerW.r4;

  return outerW;
}

/*--------------------------------------------------------------------------------------------------------------------*/

Conservative Bound::updateWallCell (const Conservative & innerW)
{
  double uInner = innerW.r2 / innerW.r1;
  double vInner = innerW.r3 / innerW.r1;

  double uOuter = -uInner;
  double vOuter = -vInner;

  Conservative outerW(innerW.r1, innerW.r1 * uOuter, innerW.r1 * vOuter, innerW.r4);

  return outerW;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Bound::symmetry2ndOrder (const Interface & f, const Conservative & inner2, const Conservative & inner1,
                              Conservative & outer1, Conservative & outer2)
{
  outer1 = updateSymmetryCell(inner1, f);
  outer2 = updateSymmetryCell(inner2, f);
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Bound::inlet2ndOrder (const Conservative & inner1, Conservative & outer1, Conservative & outer2)
{
  outer1 = updateInletCell(inner1);
  outer2 = updateInletCell(inner1);
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Bound::outlet2ndOrder (const Conservative & inner1, Conservative & outer1, Conservative & outer2)
{
  outer1 = updateOutletCell(inner1);
  outer2 = updateOutletCell(inner1);
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Bound::wall2ndOrder (const Conservative & inner1, const Conservative & inner2, Conservative & outer1,
                          Conservative & outer2)
{
  outer1 = updateWallCell(inner1);
  outer2 = updateWallCell(inner2);
}

/*--------------------------------------------------------------------------------------------------------------------*/

double Bound::degToRad (double degrees)
{
  double radians = degrees * M_PI / 180;
  return radians;
}

/*--------------------------------------------------------------------------------------------------------------------*/

double Bound::get_rho_infty (double p_infty)
{
  return rho_0 * pow(p_infty / p_0, 1 / KAPPA);
}

/*--------------------------------------------------------------------------------------------------------------------*/

double Bound::get_M_infty (double p_infty)
{
  return sqrt(2 / (KAPPA - 1) * (pow(p_0 / p_infty, (KAPPA - 1) / KAPPA) - 1));
}

/*--------------------------------------------------------------------------------------------------------------------*/

double Bound::get_c_infty (const double p_infty, const double rho_infty)
{
  return sqrt(KAPPA * p_infty / rho_infty);
}

/*--------------------------------------------------------------------------------------------------------------------*/

double Bound::get_U_infty (const double M_infty, const double c_infty)
{
  return M_infty * c_infty;
}

/*--------------------------------------------------------------------------------------------------------------------*/

double Bound::get_u_infty (const double U_infty)
{
  return U_infty * cos(alpha_0);
}

/*--------------------------------------------------------------------------------------------------------------------*/

double Bound::get_v_infty (const double U_infty)
{
  return U_infty * sin(alpha_0);
}

/*--------------------------------------------------------------------------------------------------------------------*/

double Bound::get_rhoE_infty (const double p_infty, const double U_infty, const double rho_infty)
{
  return p_infty / (KAPPA - 1) + 0.5 * rho_infty * U_infty * U_infty;
}

/*--------------------------------------------------------------------------------------------------------------------*/

double Bound::get_p_infty ()
{
  return p_0 * pow((1 + ((KAPPA - 1) / 2) * M_0 * M_0), -(KAPPA / (KAPPA - 1)));
}

double Bound::computeCP (const Primitive & pv, const double p_1)
{
  const double & p_inner = pv.p;

  // this should use pressure at the edge of the domain, not just anywhere!!
  const double rho_infty = get_rho_infty(p_1);
  const double M_infty = get_M_infty(p_1);
  const double c_infty = get_c_infty(p_1, rho_infty);
  const double U_infty = get_U_infty(M_infty, c_infty);

  return (p_inner - p_0) / (0.5 * rho_0 * U_infty * U_infty);
}

/*--------------------------------------------------------------------------------------------------------------------*/
