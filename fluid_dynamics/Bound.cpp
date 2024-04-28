//
// Created by petrs on 02.12.2023.
//

#include <cmath>
#include "Bound.h"
#include "Def.h"
#include "../structures/Primitive.h"

Conservative Bound::updateInletCell(const Conservative &innerW) {
    Primitive innerPV = Primitive::computePV(innerW);

    double p_infty = innerPV.p; // p_0
    double p_0, mach_infty;

    if (!Def::isSetByMach) { // set by p_inlet
        p_0 = Def::p_inlet;
        mach_infty = sqrt(2 / (Def::KAPPA - 1) * (pow(p_0 / p_infty, (Def::KAPPA - 1) / Def::KAPPA) - 1));
    }
    else { // set by mach_inlet - nekonverguje
        mach_infty = Def::mach_inlet;
        p_0 = p_infty * pow(1 + (Def::KAPPA - 1) / 2 * pow(Def::mach_inlet, 2), (Def::KAPPA / (Def::KAPPA - 1)));
    }
    double rho_infty = Def::rho_inlet * pow(p_infty / p_0, 1 / Def::KAPPA);
    double c = sqrt(Def::KAPPA * p_infty / rho_infty); // c_infty
    double U = mach_infty * c; // | \vec u_infty |
    double u = U * cos(Def::alpha_inlet); // u_infty
    double v = U * sin(Def::alpha_inlet); // v_infty
    double rhoE = p_infty / (Def::KAPPA - 1) + 0.5 * rho_infty * pow(U, 2); // rho_infty E_infty


    Conservative outerW;
    outerW.r1 = rho_infty;
    outerW.r2 = rho_infty * u;
    outerW.r3 = rho_infty * v;
    outerW.r4 = rhoE;

    return outerW;
}

Conservative Bound::updateOutletCell(const Conservative &innerW) {
    Primitive innerPV = Primitive::computePV(innerW);

    double rhoE = Def::p_outlet / (Def::KAPPA - 1) + 0.5 * innerPV.rho * pow(innerPV.U, 2);

    Conservative outerW;
    outerW.r1 = innerPV.rho;
    outerW.r2 = innerPV.rhoU;
    outerW.r3 = innerPV.rhoV;
    outerW.r4 = rhoE;

    return outerW;
}

Conservative Bound::updateWallCell(const Conservative &innerW, const Interface &face) {

    double u = innerW.r2 / innerW.r1;
    double v = innerW.r3 / innerW.r1;
    double nx = face.nx;
    double ny = face.ny;

    Conservative outerW;
    outerW.r1 = innerW.r1;
    outerW.r2 = innerW.r1 * (u - 2 * (u * nx + v * ny) * nx);
    outerW.r3 = innerW.r1 * (v - 2 * (u * nx + v * ny) * ny);
    outerW.r4 = innerW.r4;

    return outerW;
}