//
// Created by petrs on 01.12.2023.
//

#ifndef GAMM_DEF_H
#define GAMM_DEF_H


#include "../structures/Conservative.h"

class Def
{

public:

  static bool isNaca;
  static bool isHLLC;
  static bool isSecOrd;
  static bool isSetByMach;

  static const double KAPPA;

  static double p_inlet;
  static double p_outlet;
  static double rho_inlet;
  static double alpha_inlet;
  static double mach_infty;

  static const double rhoInitial;
  static const double uInitial;
  static const double vInitial;
  static const double pInitial;
  static const double rhoEInitial;
  static Conservative wInitial;
  static const Conservative wInitialSupersonic;
  static const Conservative wInitialSubsonic;


  static void setConditions (double mach_infinity, double alphaInletDegrees);

  static void setConditions (double p_inlet, double rho_inlet, double alphaInletDegrees, double p_outlet);

  static double degreesToRadians (double degrees);

};


#endif //GAMM_DEF_H
