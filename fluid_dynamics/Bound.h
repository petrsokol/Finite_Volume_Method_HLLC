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
    static void updateInlet(std::unordered_map<int, Cell>& cells);

    static void updateOutlet(std::unordered_map<int, Cell>& cells);

    static void updateWalls(std::unordered_map<int, Cell>& cells, const std::unordered_map<std::pair<int, int>, Interface, pair_hash>& faces);

    static void updateBounds(std::unordered_map<int, Cell>& cells, const std::unordered_map<std::pair<int, int>, Interface, pair_hash>& faces);

};


#endif //GAMM_BOUND_H
