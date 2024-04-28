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

int main() {
    std::cout << "Dobry DEN!!!!" << std::endl;
    std::cout << "program started at " << DataIO::getDate() << "_" << DataIO::getTime() << std::endl;

    std::string inputDir = R"(C:\Users\petrs\Documents\CTU\BP\FVM_Geometry)";
    std::string outputDir = R"(C:\Users\petrs\Documents\CTU\BP\FVM_Data)";
    std::string name = Def::isNaca ? "naca-mesh-vertices" : "gamm-mesh-vertices";
    std::string time = DataIO::getDate() + "_" + DataIO::getTime();

//    Def::setConditions(1, 1, 1.25 * M_PI / 180, 0.656);
    Def::setConditions(1, 1, 0, 0.84);
//    Def::setConditions(0.5, 0);

    std::vector<Point> points = Point::loadPointsFromFile(inputDir, Def::isNaca ? "nacaMesh.dat" : "gammMesh.dat");
    std::unordered_map<std::pair<int, int>, Interface, pair_hash> faces = Interface::createInnerFaces(points);
    std::unordered_map<int, Cell> cells = Cell::createCells(points);
    std::vector<double> reziVec{};

    std::cout << "lesgou" << std::endl;
    int reps = 0;
    double rezi = 1;
    while (rezi > Def::EPSILON && !Def::error && reps < 30000) {
        reps++;

        Scheme::updateCellDT(cells, 0.5);
        Def::isNaca ? NACA::updateBounds(cells, faces) : GAMM::updateBounds(cells, faces);
        Scheme::computeHLLC_localTimeStep(cells, faces);
        rezi = Scheme::computeRezi_localTimeStep(cells);
        Scheme::updateCells(cells);

        reziVec.push_back(rezi);

        //TODO p_inlet = isHypersonic ? p1 : p2;

        if (reps % 100 == 0) {
            std::cout << "reps: " << reps << ", rezi: " << rezi << std::endl;
        }

        if (reps % 5000 == 0) {
            DataIO::exportPointsToCSV(cells, points, outputDir, Def::isNaca ? "naca-mesh-vertices" : "gamm-mesh-vertices", reps, time);
            DataIO::exportPointsToDat(cells, points, outputDir, Def::isNaca ? "naca-wall-vertices" : "gamm-wall-vertices", time, reps);
            std::cout << "data written hopefully. " << std::endl;
        }
    }

    DataIO::exportPointsToCSV(cells, points, outputDir, Def::isNaca ? "naca-mesh-vertices-DONE" : "gamm-mesh-vertices-DONE", reps, time);
    DataIO::exportToCSV(cells, outputDir, Def::isNaca ? "naca-mesh-DONE" : "gamm-mesh-DONE", reps);
    DataIO::exportPointsToDat(cells, points, outputDir, Def::isNaca ? "naca-wall-vertices-DONE" : "gamm-wall-vertices-DONE", time, reps);
    DataIO::exportVectorToDat(reziVec, outputDir, Def::isNaca ? "naca-rezi-vertices-DONE" : "gamm-rezi-vertices-DONE", time);

    if(Def::error) {std::cout << "error at rep " << reps << std::endl;}

    std::cout << "program ended at " << DataIO::getTime() << std::endl;
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

// GLOBAL TIME STEP
/*
    double t = 0;
    reps = 0;
    while (rezi > Def::EPSILON && !Def::error && reps < 30000) {
        reps++;
        double dt = Scheme::computeDT(cells, 0.5);
        std::unordered_map<int, double> deltaT = Scheme::LocalTimeStep(cells, 0.5);
        t += dt;

        NACA::updateBounds(cells, faces);
        Scheme::computeHLLC(cells, faces, dt);
        rezi = Scheme::computeRezi(cells, dt);
        Scheme::updateCells(cells);

        if (reps % 100 == 0) {
            std::cout << "reps: " << reps << ", rezi: " << rezi << ", dt: " << dt << std::endl;
        }

        if (reps % 1000 == 0) {
            DataIO::exportToCSV(cells, outputDir, "naca_mesh_global", reps);
            DataIO::exportToDAT(cells, outputDir, "mach_along_wall_global", reps);
        }
    }
    */

// create a profile overlay
/*
    std::string fileName = Def::isNaca ? "\\only-naca.csv" : "\\only-gamm.csv";
    std::ofstream stream(outputDir + "\\" + fileName);
    int upperBound = Def::isNaca ? NACA::wingLength : 50;
    int offset = Def::isNaca ? Def::firstInner + NACA::wingStart : Def::firstInner + 50;

    stream << "\"X\", \"Y\", \"Z\", \"Z_MACH_NUMBER\", \"Z_PRESSURE\"\n";
    for (int i = 0; i < upperBound; ++i) {
        int k = offset + i;
        Primitive pv = Primitive::computePV(cells.at(k).w);
        double mach = pv.U / pv.c;
        stream << points.at(k).x << ", " << points.at(k).y << ", 1.1, " << mach << ", " << pv.p << "\n";
    }
    if (!Def::isNaca) {
        for (int i = upperBound; i > 0; --i) {
            int k = offset + i;
            Primitive pv = Primitive::computePV(cells.at(k).w);
            double mach = pv.U / pv.c;
            stream << points.at(k).x << ", " << 0 << ", 1.1, " << mach << ", " << pv.p << "\n";
        }
    }
    stream.close();
    */