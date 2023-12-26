//
// Created by petrs on 01.12.2023.
//

#ifndef GAMM_DEF_H
#define GAMM_DEF_H


#include "../structures/Conservative.h"

class Def {

public:
    static constexpr int xInner = 260;
    static constexpr int yInner = 60;
    static const int inner;

    static const int xCells;
    static const int yCells;
    static const int cells;

    static constexpr int gl = 2;
    static const int firstInner;
    static const double dx;

    static const double yLowerBound;
    static const double yUpperBound;
    static const double xLowerBound;
    static const double xUpperBound;

    static const double KAPPA;
    static const double P0;
    static const double P2;
    static const double RHO;
    static const double ALPHA_INFINITY;
    static const double EPSILON;

    static constexpr int NACA_X_INNER = 261; //points, not cells (260x60 inner cells)
    static constexpr int NACA_Y_INNER = 61;
    static constexpr int NACA_X = NACA_X_INNER + 2 * gl;
    static constexpr int NACA_Y = NACA_Y_INNER + 2 * gl;

    static const double rhoInitial;
    static const double uInitial;
    static const double vInitial;
    static const double pInitial;
    static const double rhoEInitial;
    static const Conservative wInitial;

    static const std::string defaultPath;
    static const std::string defaultExtension;

    static int innerIndex(int i);

    static bool error;
};


#endif //GAMM_DEF_H
