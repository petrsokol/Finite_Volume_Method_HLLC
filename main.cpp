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
#include "fluid_dynamics/NACA.h"
#include "utilities/DataIO.h"

int main() {
    std::cout << "Dobry DEN!!!!" << std::endl;
    std::string inputDir = R"(C:\Users\petrs\Documents\CTU\BP\FVM_Geometry)";
    std::string outputDir = R"(C:\Users\petrs\Documents\CTU\BP\FVM_Data)";

    std::vector<Point> points = Point::loadPointsFromFile(inputDir, "gammMesh.dat");
    std::unordered_map<std::pair<int, int>, Interface, pair_hash> faces = Interface::createInnerFaces(points);
    std::unordered_map<int, Cell> cells = Cell::createCells(points);

    std::cout << DataIO::getDate();

    int reps = 0;
    double rezi = 1;
    while (rezi > Def::EPSILON && !Def::error && reps < 30000) {
        reps++;

        Scheme::updateCellDT(cells, 0.5);
        NACA::updateBounds(cells, faces);
        Scheme::computeHLLC_localTimeStep(cells, faces);
        rezi = Scheme::computeRezi_localTimeStep(cells);
        Scheme::updateCells(cells);

        if (reps % 100 == 0) {
            std::cout << "reps: " << reps << ", rezi: " << rezi << std::endl;
        }

        if (reps % 1000 == 0) {
            DataIO::exportToCSV(cells, outputDir, "naca_mesh", reps);
            DataIO::exportToDAT(cells, outputDir, "mach_along_wall", reps);
        }
    }

//    double t = 0;
//    reps = 0;
//    while (rezi > Def::EPSILON && !Def::error && reps < 30000) {
//        reps++;
//        double dt = Scheme::computeDT(cells, 0.5);
//        std::unordered_map<int, double> deltaT = Scheme::LocalTimeStep(cells, 0.5);
//        t += dt;
//
//        NACA::updateBounds(cells, faces);
//        Scheme::computeHLLC(cells, faces, dt);
//        rezi = Scheme::computeRezi(cells, dt);
//        Scheme::updateCells(cells);
//
//        if (reps % 100 == 0) {
//            std::cout << "reps: " << reps << ", rezi: " << rezi << ", dt: " << dt << std::endl;
//        }
//
//        if (reps % 1000 == 0) {
//            DataIO::exportToCSV(cells, outputDir, "naca_mesh_global", reps);
//            DataIO::exportToDAT(cells, outputDir, "mach_along_wall_global", reps);
//        }
//    }



    if(Def::error) {
        std::cout << "error at rep " << reps << std::endl;
    }

    std::cout << "nashledanou" << std::endl;
    return 0;
}

//GAMM CHANNEL GEOMETRY
/*
std::vector<double> x_vector(Def::xCells);
std::vector<double> bot(Def::xCells, Def::yLowerBound);
std::vector<double> top(Def::xCells, Def::yUpperBound);

for (int i = 0; i < Def::xCells; ++i) { x_vector[i] = (i - Def::gl) * Def::dx;}

Lines::arc(x_vector, bot, 1, 2, 0.1);

std::vector<Point> points = Point::createPoints(bot, top);
 */

//NACA 0012 GEOMETRY
/*
std::vector<Point> points{};
std::ifstream input("mesh.dat");
for (int j = 0; j < Def::NACA_Y_INNER; ++j) {
    for (int i = 0; i < Def::NACA_X_INNER; ++i) {
        double x;
        double y;
        input >> x;
        input >> y;
        points.emplace_back(x, y, j * Def::NACA_X_INNER + i);
    }
}

std::vector<Point> under{};
for (int j = 0; j < Def::gl; ++j) {
    for (int i = 0; i < Def::NACA_X_INNER; ++i) {
        double dy = points[i + Def::NACA_X_INNER].y - points[i].y;
        double dx = points[i + Def::NACA_X_INNER].x - points[i].x;
        double x = points[i].x - dx * (Def::gl - j);
        double y = points[i].y - dy * (Def::gl - j);
        under.emplace_back(x, y, i);
    }
}
for (const auto &item: under) item.toString();

std::vector<Point> over{};
for (int j = 0; j < Def::gl; ++j) {
    for (int i = 0; i < Def::NACA_X_INNER; ++i) {
        double dy = points[(Def::NACA_Y_INNER - 1) * Def::NACA_X_INNER + i].y - points[(Def::NACA_Y_INNER - 2) * Def::NACA_X_INNER + i].y;
        double dx = points[(Def::NACA_Y_INNER - 1) * Def::NACA_X_INNER + i].x - points[(Def::NACA_Y_INNER - 2) * Def::NACA_X_INNER + i].x;
        double x = points[(Def::NACA_Y_INNER - 1) * Def::NACA_X_INNER + i].x + dx * (j + 1);
        double y = points[(Def::NACA_Y_INNER - 1) * Def::NACA_X_INNER + i].y + dy * (j + 1);
        over.emplace_back(x, y, i);
    }
}
for (const auto &item: over) item.toString();

std::ofstream naca_points("under_mid_over.dat");
for (const auto & point : under) {
    naca_points << point.x << " " << point.y << std::endl;
}
for (const auto &point: points) {
    naca_points << point.x << " " << point.y << std::endl;
}
for (const auto &point : over) {
    naca_points << point.x << " " << point.y << std::endl;
}
naca_points.close();

//LR
std::vector<Point> lr{};
for (int j = 0; j < Def::NACA_Y; ++j) {
    //left
    for (int i = 0; i < Def::gl; ++i) {
        int k = Def::NACA_X_INNER * j;
        double dx = points[k + 1].x - points[k].x;
        double dy = points[k + 1].y - points[k].y;
        double x = points[k].x - (Def::gl - i) * dx;
        double y = points[k].y - (Def::gl - i) * dy;
        lr.emplace_back(x, y, -1);
    }
    //right
    for (int i = 0; i < Def::gl; ++i) {
        int k = Def::NACA_X_INNER * (j + 1) - 1;
        std::cout << k << std::endl;
        double dx = points[k - 1].x - points[k].x;
        double dy = points[k - 1].y - points[k].y;
        double x = points[k].x - (i + 1) * dx;
        double y = points[k].y - (i + 1) * dy;
        lr.emplace_back(x, y, -1);
    }
}

std::vector<Point> allpoints{};
for (int j = 0; j < Def::NACA_Y; ++j) {
    for (int i = 0; i < Def::gl; ++i) {
        int k = j * 2 * Def::gl + i;
        allpoints.push_back(lr[k]);
    }
    for (int i = 0; i < Def::NACA_X_INNER; ++i) {
        int k = j * Def::NACA_X_INNER + i;
        allpoints.push_back(points[k]);
    }
    for (int i = 0; i < Def::gl; ++i) {
        int k = j * 2 * Def::gl + Def::gl + i;
        allpoints.push_back(lr[k]);
    }
}

for (int i = 0; i < 600; ++i) {
    allpoints[i].toString();
}

std::ofstream out("ghostMesh.dat");
for (const auto &point: allpoints) {
    out << point.x << " " << point.y << "\n";
} out.close();
 */

// obscure output
/*
 *     // Write data
    std::ofstream outputFile(Def::defaultPath + "\\" + "naca" + ".dat");
    for (int i = 0; i < Def::inner; ++i) {
        int k = Def::innerIndex(i);
        Primitive pv = Primitive::computePV(cells.at(k).w);
        double mach = pv.U / pv.c;
        outputFile << cells.at(k).tx << " " << cells.at(k).ty << " " << mach << "\n";
    } outputFile.close();

    std::ofstream outputVec(Def::defaultPath + "\\" + "mach_along_bot_8000_150x50.dat");
    for (int i = 0; i < Def::xInner; ++i) {
        int k = Def::firstInner + i;
        Primitive pv = Primitive::computePV(cells.at(k).w);
        double mach = pv.U / pv.c;
        outputVec << cells.at(k).tx << " " << mach << "\n";
    } outputVec.close();
 */