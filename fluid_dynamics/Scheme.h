//
// Created by petrs on 02.12.2023.
//

#ifndef GAMM_SCHEME_H
#define GAMM_SCHEME_H


#include <unordered_map>
#include "../structures/Conservative.h"
#include "../geometry/Cell.h"
#include "../geometry/Interface.h"

class Scheme {

public:

    // Methods
    static Conservative HLLC(const std::unordered_map<int, Cell>& cells, const Interface& face);

    static void computeHLLC(std::unordered_map<int, Cell>& cells,
                            const std::unordered_map<std::pair<int, int>, Interface, pair_hash>& faces, double dt);

    static double computeDT(const std::unordered_map<int, Cell>& cells, double CFL);

};


#endif //GAMM_SCHEME_H
