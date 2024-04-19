//
// Created by petrs on 16.04.2024.
//

#ifndef GAMM_CASE_H
#define GAMM_CASE_H


#include <string>
#include "../structures/Conservative.h"

class Case {

public:
    // physics constants
    static const double KAPPA;

    // data shared across all test cases
    static const int gl;

    // name of the specific case
    std::string name;

    // geometry information - input by user
    int xInner;
    int yInner;

    // geometry information - calculated from user input
    int inner;
    int firstInner;

    int xCells;
    int yCells;
    int cells;

    // boundary condition - inlet
    double p_inlet;
    double rho_inlet;
    double alpha_inlet;

    // boundary condition - outlet
    double p_outlet;

    // initial condition
    Conservative wInitial;

    // misc.
    int EPSILON;

    // Constructor
    Case() = default;

    explicit Case(std::string name, int xInner, int yInner);
    explicit Case(std::string name, int xInner, int yInner, int epsilon);
};


#endif //GAMM_CASE_H
