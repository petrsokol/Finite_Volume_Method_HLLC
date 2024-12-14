//
// Created by petrs on 19.12.2023.
//

#ifndef GAMM_NACA_H
#define GAMM_NACA_H


#include <unordered_map>
#include "../geometry/Cell.h"
#include "../geometry/Interface.h"

class NACA {

public:
    static const int wingStart;
    static const int wingLength;

    static void updateInlet(std::vector<Cell> & cells);

    static void updateOutlet(std::vector<Cell> & cells);

    static void updateWalls(std::vector<Cell> & cells, const std::vector<Interface> & faces);

    static void updatePeriodicity(std::vector<Cell> & cells);

    static void updateBounds(std::vector<Cell> & cells, const std::vector<Interface> & faces);

};


#endif //GAMM_NACA_H
