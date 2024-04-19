//
// Created by petrs on 15.04.2024.
//

#include <cmath>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "../geometry/Interface.h"
#include "../geometry/Cell.h"
#include "GAMM.h"
#include "../fluid_dynamics/Scheme.h"
#include "../structures/Primitive.h"
#include "../fluid_dynamics/Def.h"

const int GAMM::gl = 2;

const int GAMM::xInner = 15;
const int GAMM::yInner = 5;

const int GAMM::yCells = yInner + 2 * gl + 1;
const int GAMM::xCells = xInner + 2 * gl + 1;

const int GAMM::cells = xCells * yCells;
const double GAMM::yLowerBound = 0;

const double GAMM::yUpperBound = 1;
const double GAMM::xLowerBound = 0;
const double GAMM::xUpperBound = 3;

const double GAMM::dx = (xUpperBound - xLowerBound) / xInner;

const int GAMM::firstInner = xCells * gl + gl;

int GAMM::bound(double l) {
    return int(floor(l / GAMM::dx) + GAMM::gl);
}

void GAMM::arc(std::vector<double> x, std::vector<double> &y, double l1, double l2, double d) {
    double y_s = d - (d * d + pow((l2 - l1) / 2, 2)) / (2 * d);
    double x_s = (l1 + l2) / 2;
    double r = d - y_s;

    std::cout << bound(l1) << ", " << bound(l2) << std::endl;

    for (int i = bound(l1); i < bound(l2); ++i) {
        y[i] = y_s + sqrt(pow(r,2) - pow(x[i]-x_s, 2));
        std::cout << y[i] << std::endl;
    }
}

void GAMM::line(std::vector<double> &y, double l1, double l2, double yValue) {
    for (int i = bound(l1); i < bound(l2); ++i) {
        y[i] = yValue;
    }
}

std::vector<Point> GAMM::createPoints(std::vector<double> bot, std::vector<double> top) {
    std::vector<Point> res{};
    std::vector<double> dy{};

    for (int i = 0; i < GAMM::xCells; ++i) {
        dy.push_back((top[i] - bot[i]) / GAMM::yInner);
    }

    for (int i = 0; i < GAMM::cells; ++i) {
        double xx = (i % GAMM::xCells - GAMM::gl) * GAMM::dx;
        double yy = bot[i % GAMM::xCells] + (int(i / GAMM::xCells) - GAMM::gl) * dy[i % GAMM::xCells];
        res.emplace_back(xx, yy, i);
    }

    return res;
}

void GAMM::savePointsToFile(const std::string dir, const std::string name, std::vector<Point> points) {
    std::ofstream outputStream(dir + "\\" + name + ".dat");
    for (int i = 0; i < GAMM::cells; ++i) {
        outputStream << points[i].x << " " << points[i].y << std::endl;
    }
    outputStream.close();
}

void GAMM::solveHLLC(std::string name, std::string exportDestination,
                     const std::unordered_map<std::pair<int, int>, Interface, pair_hash> &faces,
                     std::unordered_map<int, Cell> &cells, std::vector<double> &boundaryCondition, int epsilon,
                     const bool useLocalTimeStep, const bool useHLLC) {
    //export - DataIO - export(cells, int mode (0 - mach, 1 - pressure, ...), int firstinner, int cellCount)
    double rezi = 1;
    bool error = false;
    int reps = 0;

    if (useLocalTimeStep) {
        while (rezi > epsilon && !error && reps < 30000) {
            reps++;
            Scheme::updateCellDT(cells, 0.5);
            GAMM::updateBounds(cells, faces, boundaryCondition);
            useHLLC ? Scheme::computeHLLC_localTimeStep(cells, faces) : Scheme::computeHLL_localTimeStep(cells, faces);
            rezi = Scheme::computeRezi_localTimeStep(cells);
            Scheme::updateCells(cells);
            if (reps % 100 == 0) {
                std::cout << "reps: " << reps << ", rezi: " << rezi << /*", dt: " << dt <<*/ "\n";
            }

        }
    } else {
        double t = 0;
        double dt;
        while (rezi > epsilon && !error && reps < 30000) {
            reps++;

            dt = Scheme::computeDT(cells, 0.5);
//            std::unordered_map<int, double> deltaT = Scheme::LocalTimeStep(cells, 0.5);
            t += dt;

            GAMM::updateBounds(cells, faces, boundaryCondition);
            useHLLC ? Scheme::computeHLLC(cells, faces, dt) : Scheme::computeHLL(cells, faces, dt);
            rezi = Scheme::computeRezi_localTimeStep(cells);
            Scheme::updateCells(cells);
            if (reps % 100 == 0) {
                std::cout << "reps: " << reps << ", rezi: " << rezi << /*", dt: " << dt <<*/ "\n";
            }
        }
    }

    if (rezi <= epsilon) {
        std::cout << "jsme na dne, sefe." << std::endl;
    }
}

void GAMM::updateInlet(std::unordered_map<int, Cell> &cells, const std::vector<double> &boundaryCondition) {
    double p_inlet = boundaryCondition[0];
    double rho_inlet = boundaryCondition[1];
    double alpha_inlet = boundaryCondition[2];
    double p_outlet = boundaryCondition[3];

    for (int j  = 0; j < GAMM::yInner; ++j) {
        int k = GAMM::firstInner + j * GAMM::xCells;
        Primitive innerPV = Primitive::computePV(cells.at(k).w);
        double p = innerPV.p;
        double rho = rho_inlet * pow(p / p_inlet, 1 / Def::KAPPA);
        double c = sqrt(Def::KAPPA * p / rho);
        double M = sqrt(2 / (Def::KAPPA - 1) * (pow(p_inlet / p, (Def::KAPPA - 1) / Def::KAPPA) - 1));
        double U = M * c;
        double u = U * cos(alpha_inlet);
        double v = U * sin(alpha_inlet);
        double rhoE = p / (Def::KAPPA - 1) + 0.5 * rho * pow(U, 2);

        Conservative outer;
        outer.r1 = rho;
        outer.r2 = rho * u;
        outer.r3 = rho * v;
        outer.r4 = rhoE;

        cells.at(k - 1).w = outer;
    }
}

void GAMM::updateOutlet(std::unordered_map<int, Cell> &cells, const std::vector<double> &boundaryCondition) {
    double p_inlet = boundaryCondition[0];
    double rho_inlet = boundaryCondition[1];
    double alpha_inlet = boundaryCondition[2];
    double p_outlet = boundaryCondition[3];

    for (int j = 0; j < GAMM::yInner; ++j) {
        int k = GAMM::firstInner + GAMM::xInner - 1 + j * GAMM::xCells;
        Primitive innerPV = Primitive::computePV(cells.at(k).w);

        double rhoE = p_outlet / (Def::KAPPA - 1) + 0.5 * innerPV.rho * pow(innerPV.U, 2);

        Conservative outer;
        outer.r1 = innerPV.rho;
        outer.r2 = innerPV.rhoU;
        outer.r3 = innerPV.rhoV;
        outer.r4 = rhoE;

        cells.at(k + 1).w = outer;
    }
}

void GAMM::updateWalls(std::unordered_map<int, Cell> &cells,
                       const std::unordered_map<std::pair<int, int>, Interface, pair_hash> &faces) {

    for (int i = 0; i < GAMM::xInner; ++i) {
        int k = GAMM::firstInner + i;
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

        cells.at(k - GAMM::xCells).w = outer;
    }

    for (int i = 0; i < GAMM::xInner; ++i) {
        int k = GAMM::firstInner + (GAMM::yInner) * GAMM::xCells + i;
        Conservative inner = cells.at(k - GAMM::xCells).w;

        double u = inner.r2 / inner.r1;
        double v = inner.r3 / inner.r1;
        double nx = faces.at(std::make_pair(k, k + 1)).nx;
        double ny = faces.at(std::make_pair(k, k + 1)).ny;

        Conservative outer;
        outer.r1 = inner.r1;
        outer.r2 = inner.r1 * (u - 2 * (u * nx + v * ny) * nx);
        outer.r3 = inner.r1 * (v - 2 * (u * nx + v * ny) * ny);
        outer.r4 = inner.r4;

        cells.at(k).w = outer;
    }
}

void GAMM::updateBounds(std::unordered_map<int, Cell> &cells,
                        const std::unordered_map<std::pair<int, int>, Interface, pair_hash> &faces,
                        const std::vector<double> &boundaryCondition) {
    updateInlet(cells, boundaryCondition);
    updateOutlet(cells, boundaryCondition);
    updateWalls(cells, faces);
}
