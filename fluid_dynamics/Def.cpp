//
// Created by petrs on 01.12.2023.
//

#include <cmath>
#include "Def.h"

const std::string Def::defaultPath = R"(C:\Users\petrs\Documents\CTU\BP\Charts\Data)";
const std::string Def::defaultExtension = ".dat";

const bool Def::isNaca = true;

const int Def::xInner = isNaca ? 260 : 150;
const int Def::yInner = isNaca ? 60 : 50;
const int Def::inner = xInner * yInner;

const int Def::gl = 2;

const int Def::xCells = xInner + 2 * gl + 1;
const int Def::yCells = yInner + 2 * gl + 1;
const int Def::cells = xCells * yCells;

const int Def::firstInner = xCells*gl + gl;

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
const double Def::EPSILON = -13;

const double Def::rhoInitial = 1;
const double Def::uInitial = 0.65;
const double Def::vInitial = 0;
const double Def::pInitial = 0.75;
const double Def::rhoEInitial = pInitial / (KAPPA - 1) + 0.5 * rhoInitial * (pow(uInitial, 2) + pow(vInitial, 2));
const Conservative Def::wInitial = Conservative(rhoInitial, rhoInitial * uInitial, rhoInitial * vInitial, rhoEInitial);



bool Def::error = false;

int Def::innerIndex(int i) {
    return firstInner + i % xInner + (i / xInner) * xCells;
}

int Def::innerPointIndex(int i) {
    return firstInner + i % (xInner + 1) + (i / (xInner + 1)) * xCells;
}

void Def::setConditions(double p_inlet, double rho_inlet, double alpha_inlet, double p_outlet) {
    Def::p_inlet = p_inlet;
    Def::rho_inlet = rho_inlet;
    Def::alpha_inlet = alpha_inlet;
    Def::p_outlet = p_outlet;
}

void Def::setConditions(double mach_infinity, double alpha_inlet) {

}
