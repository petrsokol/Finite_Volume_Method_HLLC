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
  std::cout << "Dobry DEN!!!!" << std::endl;
  std::cout << "program started at " << DataIO::getDate() << "_" << DataIO::getTime() << std::endl;

  std::string name = Def::isNaca ? "naca" : "gamm";
  std::string scheme = Def::isHLLC ? "hllc" : "hll";
  std::string order = Def::isSecOrd ? "2nd" : "1st";
  name += "_" + scheme + "_" + order + "_" + DataIO::getDate() + "_" + DataIO::getTime();


  Instructions::verticesName = name + "_vertices.csv";
  Instructions::wallName = name + "_wall.dat";
  Instructions::reziName = name + "_rezi.dat";
  Instructions::overlayName = Def::isNaca ? "only-naca.csv" : "only-gamm.csv";

  // change starting conditions accordingly
  Def::setConditions(1, 1, 0, 0.843);
  // subsonic p2 = 0.843019
  // transonic p2 = 0.623512

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
  while (rezi > Def::EPSILON && !Def::error && reps < 10000) {
    reps++;

    Scheme::updateCellDT(cells, 0.7, true);
    Def::isNaca ? NACA::updateBounds(cells, faces) : GAMM::updateBounds(cells, faces);
    Scheme::computeScheme(cells, faces);

    rezi = Scheme::computeRezi(cells);
    reziVec.push_back(rezi);
    Scheme::updateCells(cells);

    if (reps % 50 == 0) {
      std::cout << "reps: " << reps << ", rezi: " << rezi << std::endl;
    }
  }

//  std::string command = "python \"C:\\Users\\petrs\\Documents\\CTU\\BP\\PYTHON-scripts\\compareCSV.py\" \"C:\\Users\\petrs\\Documents\\CTU\\BP\\FVM_REF\\gamm_hll_vert_REF.csv\" \"C:\\Users\\petrs\\Documents\\CTU\\BP\\FVM_Data\\gamm_test_vertices.csv\" 1e-6";
//  int result = std::system(command.c_str());
//  if (result == 0) {
//    std::cout << "Files match within tolerance!" << std::endl;
//  } else {
//    std::cout << "Files do not match!" << std::endl;
//  }


  if (!Def::error) {
    DataIO::exportPointsToCSV(cells, points, Instructions::dataInput, Instructions::verticesName);
    DataIO::exportPointsToDat(cells, points, Instructions::dataInput, Instructions::wallName);
    DataIO::exportVectorToDat(reziVec, Instructions::dataInput, Instructions::reziName);
    Instructions::generateInstructions();
    Instructions::generateBackup();

    std::system(R"(python C:\Users\petrs\Documents\CTU\BP\PYTHON-scripts\mach-cp-charts.py)");
    std::system(R"(python C:\Users\petrs\Documents\CTU\BP\PYTHON-scripts\rezi-chart.py)");
  }

  if (Def::error) { std::cout << "error at rep " << reps << std::endl; }

  std::cout << "error count: " << Def::errorCount << std::endl;
  std::cout << "program ended at " << DataIO::getTime() << std::endl;
  std::cout << "nashledanou" << std::endl;
  return 0;

  /*
   * TODO
   *  k čemu je v Point.h index?
   *  change HLL and HLLC to only take certain cells instead of whole vector
   */
}
