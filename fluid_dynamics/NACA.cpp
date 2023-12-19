//
// Created by petrs on 19.12.2023.
//

#include <cmath>
#include "NACA.h"
#include "Def.h"
#include "../structures/Primitive.h"

void NACA::updateInlet(std::unordered_map<int, Cell> &cells) {
    for (int i  = 0; i < Def::xInner; ++i) { //celá horní stěna
        int k = Def::firstInner + (Def::yInner - 1) * Def::xCells + i; //check -1;
        Primitive innerPV = Primitive::computePV(cells.at(k).w);
        double p = innerPV.p;
        double rho = Def::RHO * pow(p / Def::P0, 1 / Def::KAPPA);
        double c = sqrt(Def::KAPPA * p / rho);
        double M = sqrt(2 / (Def::KAPPA - 1) * (pow(Def::P0 / p, (Def::KAPPA - 1) / Def::KAPPA) - 1));
        double U = M * c;
        double u = U * cos(Def::ALPHA_INFINITY);
        double v = U * sin(Def::ALPHA_INFINITY);
        double rhoE = p / (Def::KAPPA - 1) + 0.5 * rho * pow(U, 2);

        Conservative outer;
        outer.r1 = rho;
        outer.r2 = rho * u;
        outer.r3 = rho * v;
        outer.r4 = rhoE;

        cells.at(k + Def::xCells).w = outer;
    }
}

void NACA::updateOutlet(std::unordered_map<int, Cell> &cells) {
    for (int j  = 0; j < Def::yInner; ++j) {
        int k = Def::firstInner + j * Def::xCells;
        Primitive innerPV = Primitive::computePV(cells.at(k).w);

        double rhoE = Def::P2 / (Def::KAPPA - 1) + 0.5 * innerPV.rho * pow(innerPV.U, 2);

        Conservative outer;
        outer.r1 = innerPV.rho;
        outer.r2 = innerPV.rhoU;
        outer.r3 = innerPV.rhoV;
        outer.r4 = rhoE;

        cells.at(k - 1).w = outer;
    }
    for (int j = 0; j < Def::yInner; ++j) {
        int k = Def::firstInner + Def::xInner - 1 + j * Def::xCells;
        Primitive innerPV = Primitive::computePV(cells.at(k).w);

        double rhoE = Def::P2 / (Def::KAPPA - 1) + 0.5 * innerPV.rho * pow(innerPV.U, 2);

        Conservative outer;
        outer.r1 = innerPV.rho;
        outer.r2 = innerPV.rhoU;
        outer.r3 = innerPV.rhoV;
        outer.r4 = rhoE;

        cells.at(k + 1).w = outer;
    }
}

void NACA::updateWalls(std::unordered_map<int, Cell> &cells, const std::unordered_map<std::pair<int, int>, Interface, pair_hash> &faces) {
    for (int i = 0; i < NACA::wingLength; ++i) {
        int k = Def::firstInner + NACA::wingStart + i;
        Conservative inner = cells.at(k).w;

        double u = inner.r2 / inner.r1;
        double v = inner.r3 / inner.r1;
        double nx = faces.at(std::make_pair(k, k + 1)).nx;
        double ny = faces.at(std::make_pair(k, k + 1)).ny;

        Conservative outer;
        outer.r1 = inner.r1;
        outer.r2 = inner.r1 * (u - 2 * (u * nx + v * ny) * nx);
        outer.r3 = inner.r1 * (v - 2 * (u * nx + v * ny) * ny);
        outer.r4 = inner.r4;

        cells.at(k - Def::xCells).w = outer;
    }
}

void NACA::updatePeriodicity(std::unordered_map<int, Cell> &cells) {
    //start
    for (int i = 0; i < NACA::wingStart; ++i) {
        int k = Def::firstInner + i;
        cells.at(k - Def::xCells).w = cells.at(k).w;
    }
    //finish
    for (int i = 0; i < NACA::wingStart; ++i) {
        int k = Def::firstInner + Def::xInner - NACA::wingStart + i;
        cells.at(k - Def::xCells).w = cells.at(k).w;
    }
}

void NACA::updateBounds(std::unordered_map<int, Cell> &cells, const std::unordered_map<std::pair<int, int>, Interface, pair_hash> &faces) {
    updateInlet(cells);
    updateOutlet(cells);
    updateWalls(cells, faces);
    updatePeriodicity(cells);
}
