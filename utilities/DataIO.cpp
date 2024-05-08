//
// Created by petrs on 19.04.2024.
//

#include <ctime>
#include <fstream>
#include <chrono>
#include <cmath>
#include "DataIO.h"
#include "../fluid_dynamics/Def.h"
#include "../structures/Primitive.h"
#include "../fluid_dynamics/NACA.h"
#include "../fluid_dynamics/Scheme.h"
#include "Instructions.h"

const std::string DataIO::CSV_HEADER = "\"X\", \"Y\", \"Z\", \"M\", \"c_p\"\n";

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


std::string DataIO::getTime() {
    // Get current time
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    // Convert current time to tm struct for easy manipulation
    std::tm *timeInfo = std::localtime(&currentTime);

    // Extract hours and minutes
    int hours = timeInfo->tm_hour;
    int minutes = timeInfo->tm_min;

    // Create a string stream to build the formatted time string
    std::ostringstream formattedTime;
    formattedTime << std::setfill('0') << std::setw(2) << hours << "h" << std::setw(2) << minutes << "m";

    // Return the formatted time string
    return formattedTime.str();
}

void DataIO::exportToCSV(const std::unordered_map<int, Cell> &cells, const std::string& dir, const std::string& name, int reps) {
    std::ofstream stream(dir + "\\" + name + "_" + getTime() + "_" + std::to_string(reps) + ".csv");
    stream << DataIO::CSV_HEADER;

    for (int i = 0; i < Def::inner; ++i) {
        int k = Def::innerIndex(i);
        Primitive pv = Primitive::computePV(cells.at(k).w);
        double mach = pv.U / pv.c;
        stream << cells.at(k).tx << ", " << cells.at(k).ty << ", 1, " << mach << ", " << pv.p << "\n";
    } stream.close();
}

void DataIO::exportToDAT(const std::unordered_map<int, Cell> &cells, const std::string &dir, const std::string &name, int reps) {
    std::ofstream stream(dir + "\\" + name + "_" + getTime() + "_" + std::to_string(reps) + ".dat");

    int upperBound = Def::isNaca ? NACA::wingLength : Def::xInner;
    int offset = Def::isNaca ? Def::firstInner + NACA::wingStart : Def::firstInner;

    for (int i = 0; i < upperBound; ++i) {
        int k = offset + i;
        Primitive pv = Primitive::computePV(cells.at(k).w);
        double mach = pv.U / pv.c;
        stream << cells.at(k).tx << " " << cells.at(k).ty << " " << mach << " " << pv.p << "\n";
    }

    stream.close();
}

void DataIO::exportPointsToCSV(const std::unordered_map<int, Cell> &cells, std::vector<Point> points, const std::string &dir, const std::string &name) {
    std::vector<Point> newPoints = DataIO::updatePointValues(cells, points);
    std::ofstream stream(dir + "\\" + name);

    stream << DataIO::CSV_HEADER;
    // values to cell vertices
    int innerVertices = (Def::xInner + 1) * (Def::yInner + 1);
    for (int i = 0; i < innerVertices; ++i) {
        int k = Def::innerPointIndex(i);
        stream << newPoints[k].x << ", " << newPoints[k].y << ", 1";

        for (int j = 0; j < newPoints[k].values.size(); ++j) {
            stream << ", " << newPoints[k].values[j];
        }

        stream << std::endl;
    }
    stream.close();
}

void
DataIO::exportPointsToDat(const std::unordered_map<int, Cell> &cells, std::vector<Point> &points,
                          const std::string &dir,
                          const std::string &name) {
    std::vector<Point> newPoints = DataIO::updatePointValues(cells, points);
    std::ofstream stream(dir + "\\" + name);

    int upperBound = Def::isNaca ? NACA::wingLength + 1 : Def::xInner + 1; // +1 -> vertices in row = cells in row + 1
    int offset = Def::isNaca ? Def::firstInner + NACA::wingStart : Def::firstInner;

    for (int i = 0; i < upperBound; ++i) {
        int k = offset + i;
        stream << newPoints[k].x << " " << newPoints[k].y << " 1";

        // first: mach, second: cp
        for (int j = 0; j < newPoints[k].values.size(); ++j) {
            stream << " " << newPoints[k].values[j];
        }
        stream << std::endl;
    }
    stream.close();
}

void DataIO::exportVectorToDat(const std::vector<double> &vector, const std::string &dir, const std::string &name) {
    std::ofstream stream(dir + "\\" + name);

    int vectorSize = vector.size();
    for (int i = 0; i < vectorSize; ++i) {
        stream << i << " " << vector[i] << std::endl;
    } stream.close();
}

std::vector<Point>
DataIO::updatePointValues(const std::unordered_map<int, Cell> &cells, const std::vector<Point> &points) {
    std::vector<Point> res = points;

    // reference cp and mach values
    Primitive pv = Primitive::computePV(cells.at(Def::firstInner).w);
    Instructions::machLB = pv.U / pv.c;
    Instructions::machUB = pv.U / pv.c;
    Instructions::cpLB = Scheme::computeCP(pv.p);
    Instructions::cpUB = Scheme::computeCP(pv.p);

    // loop over all inner Cells
    // periodicity boundary condition needs special care - more contributors on the 'edge'
    bool test = false;
    int bound = Def::inner;
    for (int i = 0; i < bound; ++i) {
        int k = Def::innerIndex(i);

        Primitive pv = Primitive::computePV(cells.at(k).w);
        double mach = pv.U / pv.c;
        double cp = Scheme::computeCP(pv.p);

        // for paraview charts
        Instructions::machUB = fmax(Instructions::machUB, mach);
        Instructions::machLB = fmin(Instructions::machLB, mach);
        Instructions::cpUB = fmax(Instructions::cpUB, cp);
        Instructions::cpLB = fmin(Instructions::cpLB, cp);

        // bottom left corner
        res.at(k).values[0] += mach;
        res.at(k).values[1] += cp;
        res.at(k).contributors++;

        // bottom right corner
        res.at(k + 1).values[0] += mach;
        res.at(k + 1).values[1] += cp;
        res.at(k + 1).contributors++;

        // top left corner
        res.at(k + Def::xCells).values[0] += mach;
        res.at(k + Def::xCells).values[1] += cp;
        res.at(k + Def::xCells).contributors++;

        // top right corner
        res.at(k + Def::xCells + 1).values[0] += mach;
        res.at(k + Def::xCells + 1).values[1] += cp;
        res.at(k + Def::xCells + 1).contributors++;
    }

    if (Def::isNaca) {
        // taken from NACA::updatePeriodicity(...)
        // periodicity - start
        for (int i = 0; i < NACA::wingStart; ++i) {
            int l = Def::firstInner - Def::xCells + Def::xInner - 1 - i; //o řadu níž, poslední index - jede v protisměru // l viz BP, p. 13

            Primitive pv = Primitive::computePV(cells.at(l).w);
            double mach = pv.U / pv.c;
            double cp = Scheme::computeCP(pv.p);

            // bottom left corner
            res.at(l).values[0] += mach;
            res.at(l).values[1] += cp;
            res.at(l).contributors++;

            // bottom right corner
            res.at(l + 1).values[0] += mach;
            res.at(l + 1).values[1] += cp;
            res.at(l + 1).contributors++;

            // top left corner
            res.at(l + Def::xCells).values[0] += mach;
            res.at(l + Def::xCells).values[1] += cp;
            res.at(l + Def::xCells).contributors++;

            // top right corner
            res.at(l + Def::xCells + 1).values[0] += mach;
            res.at(l + Def::xCells + 1).values[1] += cp;
            res.at(l + Def::xCells + 1).contributors++;
        }
        // periodicity - finish
        for (int i = 0; i < NACA::wingStart; ++i) {
            int l = Def::firstInner - Def::xCells + NACA::wingStart - 1 - i; // -1 = těsně před koncem

            Primitive pv = Primitive::computePV(cells.at(l).w);
            double mach = pv.U / pv.c;
            double cp = Scheme::computeCP(pv.p);

            // bottom left corner
            res.at(l).values[0] += mach;
            res.at(l).values[1] += cp;
            res.at(l).contributors++;

            // bottom right corner
            res.at(l + 1).values[0] += mach;
            res.at(l + 1).values[1] += cp;
            res.at(l + 1).contributors++;

            // top left corner
            res.at(l + Def::xCells).values[0] += mach;
            res.at(l + Def::xCells).values[1] += cp;
            res.at(l + Def::xCells).contributors++;

            // top right corner
            res.at(l + Def::xCells + 1).values[0] += mach;
            res.at(l + Def::xCells + 1).values[1] += cp;
            res.at(l + Def::xCells + 1).contributors++;
        }
    }

    // averaging values based on number of contributors
    int pointCount = res.size();
    for (int i = 0; i < pointCount; ++i) {
        if (res[i].contributors == 0) {
            continue;
        } else {
            for (int j = 0; j < res[i].values.size(); ++j) { // for loop prepared for any number of values
                res[i].values[j] /= res[i].contributors;
            }
        }
    }
    return res;
}

