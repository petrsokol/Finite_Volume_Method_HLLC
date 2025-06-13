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
#include "../geometry/Mesh.h"
#include "../structures/Primitive.h"
#include "../utilities/Timer.h"
#include "Def.h"

class Scheme
{

public:
  static void updateCellDT (std::vector<Cell> & cells, double CFL, bool useGlobalTimeStep);

  static void updateCells (const MeshParams & mp, std::vector<Cell> & cells);

  static void updatePoints (const MeshParams & mp, const std::vector<Cell> & cells, std::vector<Point> & points);

  static void resetPoints (std::vector<Point> & points);

  static double computeRezi (const MeshParams & mp, const std::vector<Cell> & cells);

  static double computeCP (const Primitive & pv);

  static double computeMach (const Primitive & pv);

  static void setInitialCondition (std::vector<Cell> & cells, const Conservative & wInitial);

  static Conservative HLL (const Interface & f, Conservative & wl, Conservative & wr);

  static Conservative HLLC (const Interface & f, Conservative & wl, Conservative & wr);

  static Conservative minmod (Conservative a, Conservative b);

  static double minmod (double a, double b);

private:
  static Conservative flux (Interface face, Conservative w, double q, double p);

  static Conservative fluxStar (Interface face, Conservative w, double q, double S, double SM, double p, double p_star);

  static double bar (double rho_l, double rho_r, double vl, double vr);

  // todo move to cell.h
  static double centroidDistance (const Cell & c1, const Cell & c2);

  static void computeW (Conservative & wl, Conservative & wr,
                        const Cell & cll, const Cell & cl, const Cell & cr, const Cell & crr);

  static void
  updateCellVertices (const MeshParams & mp, std::vector<Point> & points, int k, const Conservative & cellW);

  static void averagePointValues (std::vector<Point> & points);

  static Conservative eulerIncrement (const Cell & c, const Interface & f, const Conservative & flux);

  static Conservative viscousTerms (const Cell & c, const Interface & f, const Conservative & rHat);

public:

  /*------------------------------------------------------------------------------------------------------------------*/

  // iterator functions
  template <typename NumericalScheme>
  static void computeScheme (const MeshParams & mp, std::vector<Cell> & cells,
                             const std::vector<Interface> & faces, NumericalScheme scheme)
  {
    // iterate over all *inner* interfaces
    int xLim = 2 * mp.X_INNER + 1;
    #pragma omp parallel for collapse(2) default(none) shared(mp, cells, faces, scheme, xLim)
    for (int j = 0; j < mp.Y_INNER; ++j) {
      for (int i = 0; i < xLim; ++i) {
        int k = 2 * (mp.FIRST_INNER + j * mp.X_CELLS) + i;
        // todo šablony?
        updateInterface(cells, faces.at(k), scheme);
      }
    }

    // iterate over top row - only horizontal interfaces
    #pragma omp parallel for default(none) shared(mp, cells, faces, scheme)
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

    // set initial condition for point values as well!
    Scheme::updatePoints(mesh.mp, mesh.cells, mesh.points);

    while (rezi > epsilon && reps < repsMax) {
      reps++;

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

      // update cell values and point values at every iteration
      Scheme::updateCells(mesh.mp, mesh.cells);
      Scheme::updatePoints(mesh.mp, mesh.cells, mesh.points);


      // timer 6
      auto t6 = std::chrono::high_resolution_clock::now();

      // mark time steps
      Timer::cellDtTimer.push_back(std::chrono::duration<double, std::milli>(t2 - t1).count());
      Timer::boundsIteratorTimer.push_back(std::chrono::duration<double, std::milli>(t3 - t2).count());
      Timer::computeSchemeTimer.push_back(std::chrono::duration<double, std::milli>(t4 - t3).count());
      Timer::reziTimer.push_back(std::chrono::duration<double, std::milli>(t5 - t4).count());
      Timer::updateCellsTimer.push_back(std::chrono::duration<double, std::milli>(t6 - t5).count());

      if (reps % 200 == 0) std::cout << "reps: " << std::setw(5) << reps << ", rezi: " << rezi << std::endl;
    }

    auto end = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double, std::milli>(end - start).count() / 1000.0;
    std::cout << "total time passed: " << totalTime << " s." << std::endl;

    /*
     * todo
     *  nepředávej tolik parametrů
     */

    // update points
    Scheme::updatePoints(mesh.mp, mesh.cells, mesh.points);
  }

  /*------------------------------------------------------------------------------------------------------------------*/



  static Conservative computeViscousFlux (const Interface & f, const Cell & cl, const Cell & cr);

/*------------------------------------------------------------------------------------------------------------------*/

  template <typename NumericalScheme>
  static void updateInterface (std::vector<Cell> & cells, const Interface & f, NumericalScheme scheme)
  {
    // extract participating cells for code clarity
    Cell & cl = cells.at(f.l);
    Cell & cr = cells.at(f.r);
    const Cell & cll = cells.at(f.ll);
    const Cell & crr = cells.at(f.rr);

    // compute conservative variables wl and wr
    Conservative wl, wr;
    computeW(wl, wr, cll, cl, cr, crr);

    // compute flux between two cells sharing the interface
    Conservative flux = scheme(f, wl, wr);

    // add flux to cells neighboring the interface
    if (Def::useEuler) {

      // EULER EQUATIONS
      cl.rezi -= eulerIncrement(cl, f, flux);
      cr.rezi += eulerIncrement(cr, f, flux);

    } else {

      // NAVIER-STOKES EQUATIONS
      Conservative rHat = computeViscousFlux(f, cl, cr);
      cl.rezi -= (eulerIncrement(cl, f, flux) - viscousTerms(cl, f, rHat));
      cr.rezi += (eulerIncrement(cr, f, flux) - viscousTerms(cr, f, rHat));
    }
  }

  /*------------------------------------------------------------------------------------------------------------------*/

  static Primitive getDerivativesX (const Cell & cl, const Cell & cr, const Interface & f);

  static Primitive getDerivativesY (const Cell & cl, const Cell & cr, const Interface & f);
};


#endif //GAMM_SCHEME_H
