//
// Created by petrs on 16.04.2024.
//

#include "Case.h"

#include <utility>
#include <cmath>

const int Case::gl = 2;
const double Case::KAPPA = 1.4;

Case::Case(std::string name, int xInner, int yInner) {

    // name
    Case::name = std::move(name);

    // input geometry
    Case::xInner = xInner;
    Case::yInner = yInner;

    // calculated geometry
    xCells = xInner + 2 * gl + 1;
    yCells = yInner + 2 * gl + 1;
    cells = xCells * yCells;

    inner = xInner * yInner;
    firstInner = xCells * gl + gl;

    // boundary condition - inlet
    p_inlet = 1;
    rho_inlet = 1;
    alpha_inlet = 1.25 * M_PI / 180;

    // boundary condition - outlet
    p_outlet = 0.656;

    // initial condition
    double rhoInitial = 1;
    double uInitial = 0.65;
    double vInitial = 0;
    double pInitial = 0.75;
    double rhoEInitial = pInitial / (KAPPA - 1) + 0.5 * rhoInitial * (pow(uInitial, 2) + pow(vInitial, 2));
    wInitial = Conservative(rhoInitial, rhoInitial * uInitial, rhoInitial * vInitial, rhoEInitial);

    // other
    EPSILON = -8;
}