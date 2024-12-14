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
    updatePointValues(const std::vector<Cell> & cells, const std::vector<Point> &points);

    static void
    exportPointsToCSV(const std::vector<Cell> & cells, std::vector<Point> points, const std::string &dir,
                      const std::string &name);

    static void
    exportPointsToDat(const std::vector<Cell> & cells, std::vector<Point> &points, const std::string &dir,
                      const std::string &name);

    static void exportVectorToDat(const std::vector<double> &vector, const std::string &dir, const std::string &name);

    static std::string getTime();

  static void updatePoint (Point & p, double mach, double cp);

  static void updateCorners (std::vector<Point> & points, int l, double mach, double cp);
};


#endif //GAMM_DATAIO_H
