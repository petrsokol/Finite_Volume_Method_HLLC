//
// Created by petrs on 01.12.2023.
//

#include <cmath>
#include "Primitive.h"
#include "../fluid_dynamics/Def.h"

Primitive Primitive::computePV(Conservative w) {
    Primitive res{};

    res.rho = w.r1;
    res.rhoU = w.r2;
    res.rhoV = w.r3;
    res.rhoE = w.r4;

    res.u = w.r2 / w.r1;
    res.v = w.r3 / w.r1;
    res.U = sqrt(pow(res.u, 2) + pow(res.v, 2));
    res.p = (Def::KAPPA - 1) * (res.rhoE - 0.5 * res.rho * pow(res.U, 2));

    res.c = sqrt((Def::KAPPA*res.p) / res.rho);
    res.h = (Def::KAPPA * res.p) / (res.rho * (Def::KAPPA - 1));

    return res;
} //kontrolováno - přestos tam měl chybu blbče
