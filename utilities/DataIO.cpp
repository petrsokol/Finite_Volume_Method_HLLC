//
// Created by petrs on 19.04.2024.
//

#include <ctime>
#include <fstream>
#include "DataIO.h"
#include "../fluid_dynamics/Def.h"
#include "../structures/Primitive.h"
#include "../fluid_dynamics/NACA.h"

std::string DataIO::getDate() {
    // Get the current time
    std::time_t currentTime = std::time(nullptr);

    // Convert the current time to a local time
    std::tm* localTime = std::localtime(&currentTime);

    // Extract the date components
    int year = localTime->tm_year % 100; // tm_year counts years since 1900
    int month = localTime->tm_mon + 1;    // tm_mon counts months from 0
    int day = localTime->tm_mday;
    std::string leadZeroMonth = (month < 10) ? "0" : "";
    std::string leadZeroDay = (day < 10) ? "0" : "";
    return std::to_string(year) + "_" + leadZeroMonth + std::to_string(month) + "_" + leadZeroDay + std::to_string(day);
}

void DataIO::exportToCSV(const std::unordered_map<int, Cell> &cells, const std::string& dir, const std::string& name, int reps) {
    std::ofstream stream(dir + "\\" + name + "_" + getDate() + "_" + std::to_string(reps) + ".csv");
    stream << "\"X\", \"Y\", \"Z\", \"MACH_NUMBER\", \"PRESSURE\"\n";
    for (int i = 0; i < Def::inner; ++i) {
        int k = Def::innerIndex(i);
        Primitive pv = Primitive::computePV(cells.at(k).w);
        double mach = pv.U / pv.c;
        stream << cells.at(k).tx << ", " << cells.at(k).ty << ", 1, " << mach << ", " << pv.p << "\n";
    } stream.close();
}

void DataIO::exportToDAT(const std::unordered_map<int, Cell> &cells, const std::string& dir, const std::string& name, int reps) {
    std::ofstream stream(dir + "\\" + name + "_" + getDate() + "_" + std::to_string(reps) + ".dat");
    for (int i = 0; i < NACA::wingLength; ++i) {
        int k = Def::firstInner + NACA::wingStart + i;
        Primitive pv = Primitive::computePV(cells.at(k).w);
        double mach = pv.U / pv.c;
        stream << cells.at(k).tx << " " << cells.at(k).ty << " " << mach << " " << pv.p << "\n";
    } stream.close();
}
