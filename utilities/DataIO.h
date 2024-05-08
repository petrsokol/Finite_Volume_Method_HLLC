//
// Created by petrs on 19.04.2024.
//

#ifndef GAMM_DATAIO_H
#define GAMM_DATAIO_H


#include <string>
#include <unordered_map>
#include "../geometry/Cell.h"

class DataIO {

private:
    static const std::string CSV_HEADER;

public:
    static std::string getDate();

    static void exportToCSV(const std::unordered_map<int, Cell> &cells, const std::string &dir, const std::string &name, int reps);

    static void exportToDAT(const std::unordered_map<int, Cell> &cells, const std::string &dir, const std::string &name, int reps);

    static std::vector<Point>
    updatePointValues(const std::unordered_map<int, Cell>& cells, const std::vector<Point> &points);

    static void
    exportPointsToCSV(const std::unordered_map<int, Cell> &cells, std::vector<Point> points, const std::string &dir,
                      const std::string &name);

    static void
    exportPointsToDat(const std::unordered_map<int, Cell> &cells, std::vector<Point> &points, const std::string &dir,
                      const std::string &name);

    static void exportVectorToDat(const std::vector<double> &vector, const std::string &dir, const std::string &name);

    static std::string getTime();
};


#endif //GAMM_DATAIO_H
