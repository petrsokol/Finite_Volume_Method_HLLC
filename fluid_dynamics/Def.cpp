//
// Created by petrs on 01.12.2023.
//

#include <cmath>
#include "Def.h"

const std::string Def::defaultPath = R"(C:\Users\petrs\Documents\CTU\BP\Charts\Data)";
const std::string Def::defaultExtension = ".dat";

const bool Def::isNaca = false;
const bool Def::isHLLC = false;
bool Def::isSetByMach = false;

const int Def::xInner = isNaca ? 260 : 150;
const int Def::yInner = isNaca ? 60 : 50;
const int Def::inner = xInner * yInner;

const int Def::gl = 2;

const int Def::xPoints = xInner + 2 * gl + 1;
const int Def::yPoints = yInner + 2 * gl + 1;
const int Def::points = xPoints * yPoints;

const int Def::firstInnerPoint = xPoints * gl + gl;

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

const double Def::EPSILON = -13;
const double Def::CFL = 0.5;

const double Def::rhoInitial = 1;
const double Def::uInitial = 0.65;
const double Def::vInitial = 0;
const double Def::pInitial = 0.9; // pro pInitial >= 1 NACA HLL nefunguje
const double Def::rhoEInitial = pInitial / (KAPPA - 1) + 0.5 * rhoInitial * (pow(uInitial, 2) + pow(vInitial, 2));
const Conservative Def::wInitial = Conservative(rhoInitial, rhoInitial * uInitial, rhoInitial * vInitial, rhoEInitial);

int Def::errorCount;



bool Def::error = false;

int Def::innerIndex(int i) {
    return firstInnerPoint + i % xInner + (i / xInner) * xPoints;
}

int Def::innerPointIndex(int i) {
    return firstInnerPoint + i % (xInner + 1) + (i / (xInner + 1)) * xPoints;
}

int Def::innerGhostIndex(int i) {
    return (firstInnerPoint - xPoints - 1) + i % (xInner + 3) + (i / (xInner + 3) * xPoints);
}

void Def::setConditions(double p_inlet, double rho_inlet, double alpha_inlet, double p_outlet) {
    Def::isSetByMach = false;
    Def::p_inlet = p_inlet;
    Def::rho_inlet = rho_inlet;
    Def::alpha_inlet = alpha_inlet;
    Def::p_outlet = p_outlet;
}

void Def::setConditions(double mach_infinity, double alpha_inlet) {
    Def::isSetByMach = true;
    Def::p_inlet = 1;
    Def::rho_inlet = 1;
    Def::alpha_inlet = alpha_inlet;
    Def::mach_infty = mach_infinity;
    double p_2 = Def::p_inlet * pow(1 + (Def::KAPPA - 1) / 2 * pow(Def::mach_infty, 2), -1 * (Def::KAPPA / (Def::KAPPA - 1)));
    std::cout << "vystupni tlak p_2 = " << p_2 << std::endl;
}

void Def::coordsToString(int i) {
    std::cout << "[" << i % Def::xPoints << ", " << i / Def::xPoints << "]" << std::endl;
}
