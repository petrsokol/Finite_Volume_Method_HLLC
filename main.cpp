#include <iostream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include "structures/Conservative.h"
#include "fluid_dynamics/Def.h"
#include "geometry/Point.h"
#include "geometry/Interface.h"
#include "geometry/Cell.h"
#include "fluid_dynamics/Scheme.h"
#include "fluid_dynamics/NACA.h"
#include "utilities/DataIO.h"
#include "fluid_dynamics/GAMM.h"
#include "utilities/Instructions.h"

int main() {

    // sets initail conditions using M_\infty and alpha_\infty
    Def::setConditions(2, 0);

    // extracts points from a '.dat' file
    std::vector<Point> points;
    points = Point::loadPointsFromFile(
            Instructions::geometryInput, "nacaMesh.dat");

    // create interfaces using points and store them in a map
    std::unordered_map<std::pair<int, int>, Interface, pair_hash> faces;
    faces = Interface::createInnerFaces(points);

    // create cells, store them in a map by their lower right point index
    std::unordered_map<int, Cell> cells;
    cells = Cell::createCells(points);

    // prepare empty vector for values of rezi
    std::vector<double> reziVec{};

    Def::wInitial.toString();

    int reps = 0;
    double rezi = 1;

    // main loop
    while (rezi > Def::EPSILON && !Def::error && reps < 30000) {
        reps++;

        // compute time step: bool useGlobalTimeStep
        Scheme::updateCellDT(cells, Def::CFL, false);

        // update boundary conditions
        NACA::updateBounds(cells, faces);

        // compute HLL or HLLC flux
        Scheme::computeScheme(cells, faces);

        // compute the value of rezi
        reziVec.push_back(Scheme::computeRezi(cells));

        // prepare cells for next iteration
        Scheme::updateCells(cells);

        // print a progress report
        if (reps % 100 == 0) {
            std::cout << "reps: " << reps << ", rezi: " << rezi << "\n";
        }
    }

    // export data obtained by the simulation
    DataIO::exportPointsToCSV(
            cells, points, Instructions::dataInput, "vertices.csv");
    DataIO::exportPointsToDat(
            cells, points, Instructions::dataInput, "wall.dat");
    DataIO::exportVectorToDat(
            reziVec, Instructions::dataInput, "rezi.dat");

    // generate instructions for paraView
    Instructions::generateInstructions();
    Instructions::generateBackup();

    // run external python scripts for generating charts for the thesis
    std::system(R"(python C:\CTU\BP\PYTHON-scripts\mach-cp-charts.py)");
    std::system(R"(python C:\CTU\BP\PYTHON-scripts\rezi-chart.py)");

    return 0;
}