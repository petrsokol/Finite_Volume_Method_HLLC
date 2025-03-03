//
// Created by petrs on 01.12.2023.
//

#ifndef GAMM_PRIMITIVE_H
#define GAMM_PRIMITIVE_H


#include <iostream>
#include "Conservative.h"

class Primitive {

public:
    double rho, rhoU, rhoV, rhoE;
    double U, u, v, c, p, h;

    // Constructors
    Primitive() = default;

    Primitive (Conservative w);
};


#endif //GAMM_PRIMITIVE_H
