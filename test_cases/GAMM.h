//
// Created by petrs on 15.04.2024.
//

#ifndef GAMM_GAMM_H
#define GAMM_GAMM_H


#include <vector>
#include <unordered_map>
#include "../geometry/Point.h"
#include "../geometry/Interface.h"
#include "../geometry/Cell.h"

class GAMM {

private:
    static int bound(double l);

public:
    static const int xInner;
    static const int yInner;
    static const int firstInner;

    static const int xCells;
    static const int yCells;

    static const int cells;
    static const double yLowerBound;
    static const double yUpperBound;
    static const double xLowerBound;

    static const double xUpperBound;
    static const double dx;

    static const int gl;
    static void arc(std::vector<double> x, std::vector<double> &y, double l1, double l2, double d);

    static void line(std::vector<double> &y, double l1, double l2, double yValue);
    static std::vector<Point> createPoints(std::vector<double> bot, std::vector<double> top);

    static void savePointsToFile(const std::string dir, const std::string name, std::vector<Point> points);


    static void
    solveHLLC(std::string name, std::string exportDestination,
              const std::unordered_map<std::pair<int, int>, Interface, pair_hash> &faces,
              std::unordered_map<int, Cell> &cells, std::vector<double> &boundaryCondition, int epsilon,
              const bool useLocalTimeStep, const bool useHLLC);

    static void updateInlet(std::unordered_map<int, Cell> &cells, const std::vector<double> &boundaryCondition);

    static void updateOutlet(std::unordered_map<int, Cell> &cells, const std::vector<double> &boundaryCondition);

    static void updateWalls(std::unordered_map<int, Cell> &cells,
                            const std::unordered_map<std::pair<int, int>, Interface, pair_hash> &faces);

    static void updateBounds(std::unordered_map<int, Cell> &cells,
                             const std::unordered_map<std::pair<int, int>, Interface, pair_hash> &faces,
                             const std::vector<double> &boundaryCondition);
};


#endif //GAMM_GAMM_H
