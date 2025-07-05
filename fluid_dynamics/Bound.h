//
// Created by petrs on 02.12.2023.
//

#ifndef GAMM_BOUND_H
#define GAMM_BOUND_H


#include "../geometry/Cell.h"
#include "../geometry/Interface.h"
#include "Constants.h"
#include "../structures/Primitive.h"

/*--------------------------------------------------------------------------------------------------------------------*/

enum BOUNDARY_CONDITION_TYPE
{
  SUBSONIC,
  SUPERSONIC,
  BLASIUS
};

// make non-static -> instance of Bound with constructor (p_inlet, rho_inlet, ...)
// different constructors for different boundary conditions -> check for flight regimes and choose correct methods
class Bound
{

private:

  static Conservative updateInletCell (const Conservative & innerW);

  static Conservative updateOutletCell (const Conservative & innerW);

  static Conservative updateSymmetryCell (const Conservative & innerW, const Interface & f);

  static Conservative updateWallCell (const Conservative & innerW);

  static Conservative updateSubsonicInlet (const Conservative & innerW);

  static Conservative updateSupersonicInlet ();

  static Conservative updateSubsonicOutlet (const Conservative & innerW);

  static Conservative updateSupersonicOutlet (const Conservative & innerW);

public:
  static int BC_TYPE;

  static double p_0;
  static double rho_0;
  static double alpha_0;
  static double M_0;
  static double p_2;

  static void setSubsonicCondition (double p_0_in, double rho_0_in, double alpha_0_in_degrees, double p_2_in);

  static void setSupersonicCondition (double p_0_in, double rho_0_in, double alpha_0_in_degrees, double M_0_in);

  static void inlet2ndOrder (const Conservative & inner1, Conservative & outer1, Conservative & outer2);

  static void outlet2ndOrder (const Conservative & inner1, Conservative & outer1, Conservative & outer2);

  static void symmetry2ndOrder (const Interface & f, const Conservative & inner2, const Conservative & inner1,
                                Conservative & outer1, Conservative & outer2);

  static void wall2ndOrder (const Conservative & inner2, const Conservative & inner1,
                            Conservative & outer1, Conservative & outer2);

  static double computeCP (const Primitive & pv, double p_1);

  static double degToRad (double degrees);

  static double get_rho_infty (double p_infty);

  static double get_M_infty (double p_infty);

  static double get_c_infty (double p_infty, double rho_infty);

  static double get_U_infty (double M_infty, double c_infty);

  static double get_u_infty (double U_infty);

  static double get_v_infty (double U_infty);

  static double get_rhoE_infty (double p_infty, double U_infty, double rho_infty);

  static double get_p_infty ();
};

#endif //GAMM_BOUND_H
