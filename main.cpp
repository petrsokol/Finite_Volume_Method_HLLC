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


int main ()
{
  Instructions::createName(Def::isNaca, Def::isHLLC, Def::isSecOrd);



  // set conditions by rho, p_in, alpha, p_out
  // Def::setConditions(1, 1, 1.25, 0.623512);

  // set conditions by mach number and angle of attack
  Def::setConditions(0.8, 1.25);

  Def::wInitial = Def::wInitialSubsonic;
  Def::wInitial.toString(); // [1;0.65;0;2.46125]
  // subsonic p2 = 0.843019
  // transonic p2 = 0.623512

  // todo mach 0.8 nesymetricky, 0.5 symm

  // points
  std::string dir = Instructions::geometryInput;
  std::string fileName = Def::isNaca ? "nacaMesh.dat" : "gammMesh.dat";
  std::vector<Point> points = Point::loadPointsFromFile(dir, fileName);

  // faces
  std::vector<Interface> faces = Interface::createFaces(points);

  // cells
  std::vector<Cell> cells = Cell::createCells(points);

  // reziVec
  std::vector<double> reziVec;

  int reps = 0;
  double rezi = 1;
  while (rezi > Def::EPSILON && !Def::error && reps < 30000) {
    reps++;

    Scheme::updateCellDT(cells, 0.7, false);
    Def::isNaca ? NACA::updateBounds(cells, faces) : GAMM::updateBounds(cells, faces);
    Scheme::computeScheme(cells, faces);

    rezi = Scheme::computeRezi(cells);
    reziVec.push_back(rezi);
    Scheme::updateCells(cells);

    if (reps % 50 == 0) {
      std::cout << "reps: " << reps << ", rezi: " << rezi << std::endl;
    }
  }

  if (!Def::error) {
    DataIO::exportPointsToCSV(cells, points, Instructions::dataInput, Instructions::verticesName);
    DataIO::exportPointsToDat(cells, points, Instructions::dataInput, Instructions::wallName);
    DataIO::exportVectorToDat(reziVec, Instructions::dataInput, Instructions::reziName);
    Instructions::generateInstructions();

    std::system(R"(python C:\Users\petrs\Documents\CTU\BP\PYTHON-scripts\mach-cp-charts.py)");
    std::system(R"(python C:\Users\petrs\Documents\CTU\BP\PYTHON-scripts\rezi-chart.py)");
  }

  if (Def::error)
    std::cout << "error at rep " << reps << std::endl;

  std::cout << "error count: " << Def::errorCount << std::endl;
  std::cout << "program ended at " << DataIO::getTime() << std::endl;
  std::cout << "nashledanou" << std::endl;
  return 0;
}
