#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "structures/Conservative.h"
#include "fluid_dynamics/Def.h"
#include "utilities/Lines.h"
#include "geometry/Point.h"
#include "geometry/Interface.h"
#include "geometry/Cell.h"
#include "fluid_dynamics/Scheme.h"
#include "fluid_dynamics/Bound.h"
#include "structures/Primitive.h"

int main() {
    std::cout << "Dobry DEN!!!!" << std::endl;

    std::vector<double> x(Def::xCells);
    std::vector<double> bot(Def::xCells, Def::yLowerBound);
    std::vector<double> top(Def::xCells, Def::yUpperBound);

    for (int i = 0; i < Def::xCells; ++i) {x[i] = (i - Def::gl) * Def::dx;}

    Lines::arc(x, bot, 1, 2, 0.1);

    std::vector<Point> points = Point::createPoints(bot, top);
    std::unordered_map<std::pair<int, int>, Interface, pair_hash> faces = Interface::createInnerFaces(points);
    std::unordered_map<int, Cell> cells = Cell::createCells(points);

//    for (const auto &item: faces) item.second.toString();

    double t = 0;
    int reps = 0;
    while (reps < 8000 && !Def::error) {
        reps++;
        double dt = Scheme::computeDT(cells, 0.5);
        t += dt;

        Bound::updateBounds(cells, faces);
        Scheme::computeHLLC(cells, faces, dt);
    }


    std::ofstream outputFile(Def::defaultPath + "\\" + "airflow_data_8000_M_150x50.dat");

    // Write data
    for (int i = 0; i < Def::inner; ++i) {
        int k = Def::innerIndex(i);
        Primitive pv = Primitive::computePV(cells.at(k).w);
        double mach = pv.U / pv.c;
//        outputFile << cells.at(k).tx << " " << cells.at(k).ty << " " << cells.at(k).w.r1 << "\n";
        outputFile << cells.at(k).tx << " " << cells.at(k).ty << " " << mach << "\n";
    }

    outputFile.close();

    std::ofstream outputVec(Def::defaultPath + "\\" + "mach_along_bot_8000_150x50.dat");
    for (int i = 0; i < Def::xInner; ++i) {
        int k = Def::firstInner + i;
        Primitive pv = Primitive::computePV(cells.at(k).w);
        double mach = pv.U / pv.c;
        outputVec << cells.at(k).tx << " " << mach << "\n";
    } outputVec.close();

    if(Def::error) {
        std::cout << "error at rep " << reps << std::endl;
    }

    std::cout << "nashledanou" << std::endl;
    return 0;
}