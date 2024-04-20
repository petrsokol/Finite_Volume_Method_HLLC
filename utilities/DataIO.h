//
// Created by petrs on 19.04.2024.
//

#ifndef GAMM_DATAIO_H
#define GAMM_DATAIO_H


#include <string>
#include <unordered_map>
#include "../geometry/Cell.h"

class DataIO {

public:
    static std::string getDate();
    static void exportToCSV(const std::unordered_map<int, Cell> &cells, const std::string& dir, const std::string& name, int reps);
    static void exportToDAT(const std::unordered_map<int, Cell> &cells, const std::string& dir, const std::string& name, int reps);

};


#endif //GAMM_DATAIO_H
