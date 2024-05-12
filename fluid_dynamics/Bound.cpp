//
// Created by petrs on 02.12.2023.
//

#include <cmath>
#include "Bound.h"
#include "Def.h"
#include "../structures/Primitive.h"

double Bound::v_infty = 0;
double Bound::u_infty = 1;
double Bound::rho_infty = 1;
double Bound::p_infty = 1;

Conservative Bound::updateInletCell(const Conservative &innerW) {
    Primitive innerPV = Primitive::computePV(innerW);

    Bound::p_infty = innerPV.p; // p_0
    double p_0 = Def::p_inlet;
    double mach_infty = 5;
    if (Def::mach_infty < 1) {
         mach_infty = sqrt(2 / (Def::KAPPA - 1) * (pow(p_0 / Bound::p_infty, (Def::KAPPA - 1) / Def::KAPPA) - 1));
         if (_isnan(mach_infty)) {
             std::cout << "Bound::updateInletCell, mach_infty je náááán" << 2 / (Def::KAPPA - 1) * (pow(p_0 / Bound::p_infty, (Def::KAPPA - 1) / Def::KAPPA) - 1);
         }
    } else {
         mach_infty = Def::mach_infty;
    }

    Bound::rho_infty = Def::rho_inlet * pow(Bound::p_infty / p_0, 1 / Def::KAPPA);
    double c = sqrt(Def::KAPPA * Bound::p_infty / Bound::rho_infty); // c_infty
    double U = mach_infty * c; // | \vec u_infty |
    Bound::u_infty = U * cos(Def::alpha_inlet); // u_infty
    Bound::v_infty = U * sin(Def::alpha_inlet); // v_infty
    double rhoE = Bound::p_infty / (Def::KAPPA - 1) + 0.5 * Bound::rho_infty * pow(U, 2); // rho_infty E_infty

    Conservative outerW;
    outerW.r1 = rho_infty;
    outerW.r2 = rho_infty * Bound::u_infty;
    outerW.r3 = rho_infty * Bound::v_infty;
    outerW.r4 = rhoE;

    if (_isnan(outerW.r1)) {
        std::cout << "Bound::updateInletCell, error at r1, vstup: \n";
        innerW.toString();
        outerW.toString();
    }
    if (_isnan(outerW.r2)) {
        std::cout << "Bound::updateInletCell, error at r2, vstup: \n";
        innerW.toString();
        outerW.toString();
    }
    if (_isnan(outerW.r3)) {
        std::cout << "Bound::updateInletCell, error at r3, vstup: \n";
        innerW.toString();
        outerW.toString();
    }
    if (_isnan(outerW.r4)) {
        std::cout << "Bound::updateInletCell, error at r4, vstup: \n";
        innerW.toString();
        outerW.toString();
    }

    return outerW;
}

Conservative Bound::updateOutletCell(const Conservative &innerW) {
    Primitive innerPV = Primitive::computePV(innerW);
    Conservative outerW;

    if (Def::mach_infty < 1) {
        double p_2;
        if (!Def::isSetByMach) {// set by p_outlet
            p_2 = Def::p_outlet;
        } else {                // set by mach_infty
            p_2 = Def::p_inlet * pow(1 + (Def::KAPPA - 1) / 2 * pow(Def::mach_infty, 2), -1 * (Def::KAPPA / (Def::KAPPA - 1)));
        }
        double rhoE = p_2 / (Def::KAPPA - 1) + 0.5 * innerPV.rho * pow(innerPV.U, 2);

        outerW.r1 = innerPV.rho;
        outerW.r2 = innerPV.rhoU;
        outerW.r3 = innerPV.rhoV;
        outerW.r4 = rhoE;
    } else {
        outerW = innerW;
    }

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