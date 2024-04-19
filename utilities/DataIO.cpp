//
// Created by petrs on 19.04.2024.
//

#include "DataIO.h"
#include "../fluid_dynamics/Def.h"
#include "../structures/Primitive.h"
#include <ctime>
#include <fstream>

std::string DataIO::getDate() {
    // Get the current time
    std::time_t currentTime = std::time(nullptr);

    // Convert the current time to a local time
    std::tm* localTime = std::localtime(&currentTime);

    // Extract the date components
    int year = localTime->tm_year % 100; // tm_year counts years since 1900
    int month = localTime->tm_mon + 1;    // tm_mon counts months from 0
    int day = localTime->tm_mday;
    std::string leadingZeroMonth = (month < 10) ? "0" : "";
    std::string leadingZeroDay = (day < 10) ? "0" : "";

    // Display the current date
    return std::to_string(year) + "_" + leadingZeroMonth + std::to_string(month) + "_" + leadingZeroDay + std::to_string(day);
}

void DataIO::exportToCSV(std::unordered_map<int, Cell> cells, std::string dir, std::string name, int reps) {
    std::ofstream stream(dir + "\\" + name + "_" + getDate() + "_" + std::to_string(reps) + ".csv");
    stream << "\"X\", \"Y\", \"Z\", \"MACH_NUMBER\"\n";
    for (int i = 0; i < Def::inner; ++i) {
        int k = Def::innerIndex(i);
        Primitive pv = Primitive::computePV(cells.at(k).w);
        double mach = pv.U / pv.c;
        stream << cells.at(k).tx << ", " << cells.at(k).ty << ", 1, " << mach << "\n";
    }
    stream.close();
}

void DataIO::exportToDAT(std::unordered_map<int, Cell> cells, std::string dir, std::string name, int reps) {

}
