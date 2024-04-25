//
// Created by petrs on 01.12.2023.
//

#ifndef GAMM_DEF_H
#define GAMM_DEF_H


#include "../structures/Conservative.h"

class Def {

public:
    static const std::string defaultPath;
    static const std::string defaultExtension;

    static const bool isNaca;

    static const int xInner;
    static const int yInner;
    static const int inner;

    static const int gl;

    static const int xCells;
    static const int yCells;
    static const int cells;

    static const int firstInner;

    static const double yLowerBound;
    static const double yUpperBound;
    static const double xLowerBound;
    static const double xUpperBound;
    static const double dx;

    static const double KAPPA;
    static double p_inlet;
    static double p_outlet;
    static double rho_inlet;
    static double alpha_inlet;
    static const double EPSILON;

    static const double rhoInitial;
    static const double uInitial;
    static const double vInitial;
    static const double pInitial;
    static const double rhoEInitial;
    static const Conservative wInitial;



    static bool error;

    static int innerIndex(int i);

    static int innerPointIndex(int i);

    static void setConditions(double mach_infinity, double alpha_inlet);

    static void setConditions(double p_inlet, double rho_inlet, double alpha_inlet, double p_outlet);
};


#endif //GAMM_DEF_H
