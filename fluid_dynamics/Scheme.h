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

    static void computeHLLC_localTimeStep(std::unordered_map<int, Cell>& cells,
                                          const std::unordered_map<std::pair<int, int>, Interface, pair_hash>& faces);

    static double computeDT(const std::unordered_map<int, Cell>& cells, double CFL);

    static std::unordered_map<int, double> LocalTimeStep(const std::unordered_map<int, Cell>& cells, double CFL);

    static void updateCellDT(std::unordered_map<int, Cell> &cells, double CFL);

    static double computeRezi(std::unordered_map<int, Cell> &cells, double dt);

    static double computeRezi_localTimeStep(const std::unordered_map<int, Cell>& cells);

    static double computeCP(double p_inner);

    static void updateCells(std::unordered_map<int, Cell> &cells);

    static void compute(std::unordered_map<int, Cell>& cells,
                        const std::unordered_map<std::pair<int, int>, Interface, pair_hash>& faces,const bool HLLC,const bool globalTimeStep);
};


#endif //GAMM_SCHEME_H
