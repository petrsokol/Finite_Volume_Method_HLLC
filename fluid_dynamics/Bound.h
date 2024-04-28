//
// Created by petrs on 02.12.2023.
//

#ifndef GAMM_BOUND_H
#define GAMM_BOUND_H


#include <unordered_map>
#include "../geometry/Cell.h"
#include "../geometry/Interface.h"

class Bound {

public:
    static Conservative updateInletCell(const Conservative &innerW);
    static Conservative updateOutletCell(const Conservative &innerW);
    static Conservative updateWallCell(const Conservative &innerW, const Interface &face);
};


#endif //GAMM_BOUND_H
