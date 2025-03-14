#include <iostream>
#include <cmath>
#include "structures/Conservative.h"
#include "fluid_dynamics/Def.h"
#include "fluid_dynamics/Scheme.h"
#include "fluid_dynamics/NACA.h"
#include "utilities/DataIO.h"
#include "utilities/Instructions.h"
#include "geometry/Mesh.h"
#include "fluid_dynamics/GAMM.h"

template <typename NumericalScheme, typename BoundsIterator>
void runExperiment (Mesh & mesh, NumericalScheme scheme, BoundsIterator boundsIterator, const Conservative & wInitial,
                    double epsilon, int repsMax, double CFL, bool useGlobalTimeStep)
{

  std::cout << "runExperiment() STARTED" << std::endl;
  int reps = 0;
  double rezi = 1;

  // set initial condition
  Scheme::setInitialCondition(mesh.cells, wInitial);

  while (rezi > epsilon && reps < repsMax) {
    reps++;

    Scheme::updateCellDT(mesh.cells, CFL, useGlobalTimeStep);
    boundsIterator(mesh.mp, mesh.cells, mesh.faces);
    Scheme::computeScheme(mesh.mp, mesh.cells, mesh.faces, scheme);

    rezi = Scheme::computeRezi(mesh.mp, mesh.cells);
    mesh.reziVec.push_back(rezi);
    Scheme::updateCells(mesh.mp, mesh.cells);

    if (reps % 50 == 0) {
      std::cout << "reps: " << reps << ", rezi: " << rezi << std::endl;
    }
  }

  /*
   * todo
   *  nepředávej tolik parametrů
   *  zkus paralelizovat co jde
   *  ať funguje cmake
   *
   */

  DataIO::updatePointValues(mesh.mp, mesh.cells, mesh.points);
  DataIO::exportPointsToCSV(mesh.mp, mesh.points, Instructions::dataInput, Instructions::verticesName);
  DataIO::exportWallPointsToDat(mesh.mp, mesh.points, Instructions::dataInput, Instructions::wallName);
  DataIO::exportVectorToDat(mesh.reziVec, Instructions::dataInput, Instructions::reziName);

  Instructions::generateInstructions();
  std::system("python3 ../post_processing_python_scripts/mach-cp-charts.py");
  std::system("python3 ../post_processing_python_scripts/rezi-chart.py");
  std::system(
          "python3 ../post_processing_python_scripts/paraView-macro-minimal.py\"\"");

}

int main ()
{
  Instructions::createName();
  // set conditions by rho, p_in, alpha, p_out
  // Def::setConditions(1, 1, 1.25, 0.623512);

  // set conditions by mach number and angle of attack
  Def::setConditions(0.8, 0);

  Def::wInitial = Def::wInitialSubsonic;
  Def::wInitial.toString(); // [1;0.65;0;2.46125]
  // subsonic p2 = 0.843019
  // transonic p2 = 0.623512

  // todo mach 0.8 nesymetricky, 0.5 symm

  // todo add boundsIterator as a mesh parameter, since it makes sense
  Mesh nacaMesh(Instructions::geometryInput, "nacaMesh.dat", 260, 60, 2, NACA::WALL_START, NACA::WALL_LENGTH);
  Mesh gammMesh(Instructions::geometryInput, "gammMesh.dat", 150, 50, 2, 0, 150); // will not work -> Def issues
  // run experiments

  // exp 1
  // todo jeden parametr - struct
  runExperiment(nacaMesh, Scheme::HLLC, NACA::updateBounds, Def::wInitial,
                -4, 1000, 0.5, false);

  // exp 2
  // exp 3



  std::cout << "program ended at " << DataIO::getTime() << std::endl;
  std::cout << "nashledanou" << std::endl;
  return 0;
}

/*
 * UŽITEČNÝ KLÁVESOVÝ ZKRATKY V CLION
 * ctrl + shift + V - historie vkládání
 * ctrl + p - nápověda parametrů funkce
 */