//
// Created by petrs on 02.12.2023.
//

#ifndef GAMM_SCHEME_H
#define GAMM_SCHEME_H


#include <unordered_map>
#include <chrono>
#include "../structures/Conservative.h"
#include "../geometry/Cell.h"
#include "../geometry/Interface.h"
#include "../utilities/DataIO.h"
#include "../utilities/Instructions.h"
#include "../geometry/Mesh.h"
#include "../structures/Primitive.h"
#include "../utilities/Timer.h"

class Scheme
{

public:
  static void updateCellDT (std::vector<Cell> & cells, double CFL, bool useGlobalTimeStep);

  static void updateCells (const MeshParams & mp, std::vector<Cell> & cells);

  static double computeRezi (const MeshParams & mp, const std::vector<Cell> & cells);

  static double computeCP (const Primitive & pv);

  static double computeMach (const Primitive & pv);

  static void setInitialCondition (std::vector<Cell> & cells, const Conservative & wInitial);

  // numerical schemes
  static Conservative HLL (const Interface & f, Conservative & wl, Conservative & wr);

  static Conservative HLLC (const Interface & f, Conservative & wl, Conservative & wr);

  // second order
  static Conservative minmod (Conservative a, Conservative b);

  static double minmod (double a, double b);

  /*------------------------------------------------------------------------------------------------------------------*/

  // iterator functions
  template <typename NumericalScheme>
  static void computeScheme (const MeshParams & mp, std::vector<Cell> & cells,
                             const std::vector<Interface> & faces, NumericalScheme scheme)
  {
    // iterate over all *inner* interfaces
    int xLim = 2 * mp.X_INNER + 1;
    for (int j = 0; j < mp.Y_INNER; ++j) {
      for (int i = 0; i < xLim; ++i) {
        int k = 2 * (mp.FIRST_INNER + j * mp.X_CELLS) + i;
        //todo statická funkce
        updateInterface(cells, faces.at(k), scheme);
      }
    }

    // iterate over top row - only horizontal interfaces
    for (int i = 0; i < mp.X_INNER; ++i) {
      int k = 2 * (mp.FIRST_INNER + mp.Y_INNER * mp.X_CELLS + i) + 1;
      updateInterface(cells, faces.at(k), scheme);
    }

    // place for alternative wall flux
  }

  /*------------------------------------------------------------------------------------------------------------------*/

  template <typename NumericalScheme, typename BoundsIterator>
  static void
  runExperiment (Mesh & mesh, NumericalScheme scheme, BoundsIterator boundsIterator, const Conservative & wInitial,
                      double epsilon, int repsMax, double CFL, bool useGlobalTimeStep)
  {
    int reps = 0;
    double rezi = 1;

    auto start = std::chrono::high_resolution_clock::now();

    // set initial condition
    Scheme::setInitialCondition(mesh.cells, wInitial);

    while (rezi > epsilon && reps < repsMax) {
      reps++;

      // advance by one timestep in the simulation

      // timer 1
      auto t1 = std::chrono::high_resolution_clock::now();

      Scheme::updateCellDT(mesh.cells, CFL, useGlobalTimeStep);

      // timer 2
      auto t2 = std::chrono::high_resolution_clock::now();

      boundsIterator(mesh.mp, mesh.cells, mesh.faces);

      // timer 3
      auto t3 = std::chrono::high_resolution_clock::now();

      Scheme::computeScheme(mesh.mp, mesh.cells, mesh.faces, scheme);

      // timer 4
      auto t4 = std::chrono::high_resolution_clock::now();

      rezi = Scheme::computeRezi(mesh.mp, mesh.cells);

      // timer 5
      auto t5 = std::chrono::high_resolution_clock::now();

      mesh.reziVec.push_back(rezi);
      Scheme::updateCells(mesh.mp, mesh.cells);

      // timer 6
      auto t6 = std::chrono::high_resolution_clock::now();

      // push back vectors
      Timer::cellDtTimer.push_back(std::chrono::duration<double, std::milli>(t2 - t1).count());
      Timer::boundsIteratorTimer.push_back(std::chrono::duration<double, std::milli>(t3 - t2).count());
      Timer::computeSchemeTimer.push_back(std::chrono::duration<double, std::milli>(t4 - t3).count());
      Timer::reziTimer.push_back(std::chrono::duration<double, std::milli>(t5 - t4).count());
      Timer::updateCellsTimer.push_back(std::chrono::duration<double, std::milli>(t6 - t5).count());

      if (reps % 50 == 0) std::cout << "reps: " << reps << ", rezi: " << rezi << std::endl;
    }

    auto end = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double, std::milli>(end - start).count() / 1000.0;
    std::cout << "total time passed: " << totalTime << " s." << std::endl;

    /*
     * todo
     *  nepředávej tolik parametrů
     *  zkus paralelizovat co jde
     *  ať funguje cmake
     *
     */

    // update points
    DataIO::updatePointValues(mesh.mp, mesh.cells, mesh.points);

    // export data for ParaView
    DataIO::exportPointsToCSV(mesh.mp, mesh.points, Instructions::dataInput, Instructions::verticesName);

    // export data for Mach and c_p along bottom wall
    DataIO::exportWallPointsToDat(mesh.mp, mesh.points, Instructions::dataInput, Instructions::wallName);

    // export data for rezi chart
    DataIO::exportVectorToDat(mesh.reziVec, Instructions::dataInput, Instructions::reziName);

    // export timers
    DataIO::exportVectorToDat(Timer::reziTimer, Instructions::dataInput, "reziTimer.dat");
    DataIO::exportVectorToDat(Timer::computeSchemeTimer, Instructions::dataInput, "computeSchemeTimer.dat");
    DataIO::exportVectorToDat(Timer::cellDtTimer, Instructions::dataInput, "cellDtTimer.dat");
    DataIO::exportVectorToDat(Timer::updateCellsTimer, Instructions::dataInput, "updateCellsTimer.dat");
    DataIO::exportVectorToDat(Timer::boundsIteratorTimer, Instructions::dataInput, "boundsIteratorTimer.dat");

    // export Mach values along both walls (GAMM)
    int topWallStart = mesh.mp.WALL_START + mesh.mp.X_POINTS * (mesh.mp.Y_INNER_POINTS - 1);
    DataIO::exportMachWallToDat(mesh.points, Instructions::dataInput, "GAMM_bot_wall.dat",
                                mesh.mp.WALL_START, topWallStart, mesh.mp.WALL_LENGTH);

    Instructions::generateInstructions();
    std::system("python3 ../post_processing_python_scripts/mach-cp-charts.py");
    std::system("python3 ../post_processing_python_scripts/rezi-chart.py");
    std::system("python3 ../post_processing_python_scripts/timer-chart.py");
    std::system("python3 ../post_processing_python_scripts/paraView-macro-minimal.py");

  }

  /*------------------------------------------------------------------------------------------------------------------*/


private:
  // support methods
  static Conservative flux (Interface face, Conservative w, double q, double p);

  static Conservative fluxStar (Interface face, Conservative w, double q, double S, double SM, double p, double p_star);

  static double bar (double rho_l, double rho_r, double vl, double vr);

  static double centroidDistance (const Cell & c1, const Cell & c2);

  static void computeW (Conservative & wl, Conservative & wr,
                        const Cell & cll, const Cell & cl, const Cell & cr, const Cell & crr);

  /*------------------------------------------------------------------------------------------------------------------*/

  template <typename NumericalScheme>
  static void updateInterface (std::vector<Cell> & cells, const Interface & face, NumericalScheme scheme)
  {
    // extract participating cells for code clarity
    Cell & cl = cells.at(face.l);
    Cell & cr = cells.at(face.r);
    const Cell & cll = cells.at(face.ll);
    const Cell & crr = cells.at(face.rr);

    // compute conservative variables wl and wr
    Conservative wl, wr;
    computeW(wl, wr, cll, cl, cr, crr);

    // compute flux between two cells sharing the interface
    Conservative flux = scheme(face, wl, wr);

    // add flux to cells neighboring the interface
    cl.rezi -= cl.dt / cl.area * flux * face.len;
    cr.rezi += cr.dt / cr.area * flux * face.len;
  }

  /*------------------------------------------------------------------------------------------------------------------*/
};


#endif //GAMM_SCHEME_H
