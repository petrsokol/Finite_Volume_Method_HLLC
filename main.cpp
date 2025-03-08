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
#include "geometry/Mesh.h"

template <typename NumericalScheme, typename BoundsIterator>
void runExperiment (Mesh & mesh, NumericalScheme scheme, BoundsIterator boundsIterator, const Conservative & wInitial,
                    double epsilon, int repsMax, double CFL, bool useGlobalTimeStep)
{
  int reps = 0;
  double rezi = 1;

  // set initial condition
  Scheme::setInitialCondition(mesh.cells, wInitial);

  while (rezi > epsilon && reps < repsMax) {
    reps++;

    Scheme::updateCellDT(mesh.cells, CFL, useGlobalTimeStep);
    boundsIterator(mesh.cells, mesh.faces);
    Scheme::computeScheme(mesh.cells, mesh.faces, scheme);

    rezi = Scheme::computeRezi(mesh.cells);
    mesh.reziVec.push_back(rezi);
    Scheme::updateCells(mesh.cells);

    if (reps % 50 == 0) {
      std::cout << "reps: " << reps << ", rezi: " << rezi << std::endl;
    }
  }

  DataIO::exportPointsToCSV(mesh.cells, mesh.points, Instructions::dataInput, Instructions::verticesName);
  DataIO::exportPointsToDat(mesh.cells, mesh.points, Instructions::dataInput, Instructions::wallName);
  DataIO::exportVectorToDat(mesh.reziVec, Instructions::dataInput, Instructions::reziName);

  Instructions::generateInstructions();
  std::system(R"(python C:\Users\petrs\Documents\CTU\BP\PYTHON-scripts\mach-cp-charts.py)");
  std::system(R"(python C:\Users\petrs\Documents\CTU\BP\PYTHON-scripts\rezi-chart.py)");
  std::system(
          "cmd /c \"\"C:\\Program Files\\ParaView 5.10.1-Windows-Python3.9-msvc2017-AMD64\\bin\\"
          "pvpython.exe\" \"C:\\Users\\petrs\\Documents\\CTU\\BP\\PYTHON-scripts\\paraView-macro-minimal.py\"\"");

}

int main ()
{
  Instructions::createName();
  // set conditions by rho, p_in, alpha, p_out
  // Def::setConditions(1, 1, 1.25, 0.623512);

  // set conditions by mach number and angle of attack
  Def::setConditions(0.8, 1.25);

  Def::wInitial = Def::wInitialSubsonic;
  Def::wInitial.toString(); // [1;0.65;0;2.46125]
  // subsonic p2 = 0.843019
  // transonic p2 = 0.623512

  // todo mach 0.8 nesymetricky, 0.5 symm

  Mesh nacaMesh(Instructions::geometryInput, "nacaMesh.dat", 260, 60, 2);
  // Mesh gammMesh(Instructions::geometryInput, "gammMesh.dat", 150, 50, 2); // will not work -> Def issues
  // run experiments

  // exp 1
  runExperiment(nacaMesh, Scheme::HLL, NACA::updateBounds, Def::wInitial, -4, 3000, 0.7, false);
  // exp 2
  // exp 3



  std::cout << "program ended at " << DataIO::getTime() << std::endl;
  std::cout << "nashledanou" << std::endl;
  return 0;
}
