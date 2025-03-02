//
// Created by petrs on 01.12.2023.
//

#include <cmath>
#include "Def.h"

/*--------------------------------------------------------------------------------------------------------------------*/

const std::string Def::defaultPath = R"(C:\Users\petrs\Documents\CTU\BP\Charts\Data)";
const std::string Def::defaultExtension = ".dat";

bool Def::isNaca = true;
bool Def::isHLLC = true;
bool Def::isSecOrd = true;
bool Def::isSetByMach = false;

const int Def::xInner = isNaca ? 260 : 150;
const int Def::yInner = isNaca ? 60 : 50;
const int Def::inner = xInner * yInner;

const int Def::gl = 2;

const int Def::xPoints = xInner + 2 * gl + 1;
const int Def::yPoints = yInner + 2 * gl + 1;
const int Def::points = xPoints * yPoints;
const int Def::firstInnerPoint = xPoints * gl + gl;

const int Def::xCells = xInner + 2 * gl;
const int Def::yCells = yInner + 2 * gl;
const int Def::cells = xCells * yCells;
const int Def::firstInner = gl * xCells + gl;


const double Def::yLowerBound = 0;
const double Def::yUpperBound = 1;
const double Def::xLowerBound = 0;
const double Def::xUpperBound = 3;
const double Def::dx = (xUpperBound - xLowerBound) / xInner;

const double Def::KAPPA = 1.4;

double Def::p_inlet = 1;
double Def::p_outlet = 0.656;
double Def::rho_inlet = 1;
double Def::alpha_inlet = M_PI * 1.25 / 180;
double Def::mach_infty = 0.5; // keep < 1 by default

const double Def::EPSILON = -8;
const double Def::CFL = 0.2;

const double Def::rhoInitial = 1;
const double Def::uInitial = 0.65;
const double Def::vInitial = 0;
const double Def::pInitial = 0.737; // pro pInitial >= 1 NACA HLL nefunguje
const double Def::rhoEInitial = pInitial / (KAPPA - 1) + 0.5 * rhoInitial * (pow(uInitial, 2) + pow(vInitial, 2));

// supersonic initial condition
const Conservative Def::wInitialSupersonic = Conservative(1, 2.2, 0, 4.42);

// subsonic initial condition
const Conservative Def::wInitialSubsonic = Conservative(1, 1, 0, 2.42);

Conservative Def::wInitial = Def::wInitialSubsonic;

int Def::errorCount;
bool Def::error = false;

/*--------------------------------------------------------------------------------------------------------------------*/

int Def::innerIndex (int i)
{
  return firstInner + i % xInner + (i / xInner) * xCells;
}

/*--------------------------------------------------------------------------------------------------------------------*/

int Def::innerPointIndex (int i)
{
  return firstInnerPoint + i % (xInner + 1) + (i / (xInner + 1)) * xPoints;
}

/*--------------------------------------------------------------------------------------------------------------------*/

double Def::degreesToRadians (double degrees)
{
  double radians = degrees * M_PI / 180;
  return radians;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Def::setConditions (double p_inlet, double rho_inlet, double alphaInletDegrees, double p_outlet)
{
  Def::isSetByMach = false;
  Def::p_inlet = p_inlet;
  Def::rho_inlet = rho_inlet;
  Def::alpha_inlet = degreesToRadians(alphaInletDegrees);
  Def::p_outlet = p_outlet;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Def::setConditions (double mach_infinity, double alphaInletDegrees)
{
  Def::isSetByMach = true;
  Def::p_inlet = 1;
  Def::rho_inlet = 1;
  Def::alpha_inlet = degreesToRadians(alphaInletDegrees);
  Def::mach_infty = mach_infinity;
  double p_2 = Def::p_inlet * pow(1 + (Def::KAPPA - 1) /
                                      2 * pow(Def::mach_infty, 2), -1 * (Def::KAPPA / (Def::KAPPA - 1)));
  std::cout << "vystupni tlak p_2 = " << p_2 << std::endl;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Def::coordsToString (int i)
{
  std::cout << "[" << i % Def::xCells << ", " << i / Def::xCells << "]" << std::endl;
}

/*--------------------------------------------------------------------------------------------------------------------*/

int Def::pointIndexToCellIndex (int k)
{
  return k % xPoints + (k / xPoints) * xCells;
}

/*--------------------------------------------------------------------------------------------------------------------*/

int Def::cellIndexToPointIndex (int k)
{
  int pointIndex = k % xCells + (k / xCells) * xPoints;
  return pointIndex;
}

/*--------------------------------------------------------------------------------------------------------------------*/
