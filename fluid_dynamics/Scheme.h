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
  static double computeCP (const Primitive & pv, const double p_1);

  static double computeMach (const Primitive & pv);

  static Conservative HLL (const Interface & f, Conservative & wl, Conservative & wr);

  static Conservative HLLC (const Interface & f, Conservative & wl, Conservative & wr);

  static Conservative minmod (Conservative a, Conservative b);

  static double minmod (double a, double b);

  static Primitive getDerivativesX (const Cell & cl, const Cell & cr, const Interface & f);

  static Primitive getDerivativesY (const Cell & cl, const Cell & cr, const Interface & f);

  static Conservative computeViscousFlux (const Interface & f, const Cell & cl, const Cell & cr);

private:
  static Conservative flux (Interface face, Conservative w, double q, double p);

  static Conservative fluxStar (Interface face, Conservative w, double q, double S, double SM, double p, double p_star);

  static double bar (double rho_l, double rho_r, double vl, double vr);

  static void computeW (Conservative & wl, Conservative & wr,
                        const Cell & cll, const Cell & cl, const Cell & cr, const Cell & crr);


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

    // set initial condition
    mesh.setInitialCondition(wInitial);

    // set initial condition for point values as well!
    mesh.updatePoints();

    while (rezi > epsilon && reps < repsMax) {
      reps++;

      // compute time step size
      mesh.updateCellDT(CFL, useGlobalTimeStep);

      // update boundary conditions
      boundsIterator(mesh.mp, mesh.cells, mesh.faces);

      // compute fluxes across inner faces
      Scheme::computeScheme(mesh.mp, mesh.cells, mesh.faces, scheme);

      // compute density residuum
      mesh.reziVec.push_back(rezi = mesh.computeRezi());

      /*
      for (const auto & cell: mesh.cells) {
        printf("cell at [%f, %f] has a rezi value of [%f, %f, %f, %f]\n",
               cell.tx, cell.ty, cell.rezi.r1, cell.rezi.r2, cell.rezi.r3, cell.rezi.r4);
      }
       */

      // update cell values and point values at every iteration
      mesh.updateCells();
      mesh.updatePoints();

      // report progress
      if (reps % 100 == 0) printf("reps: %5d, rezi: %f\n", reps, rezi);
    }
  }

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
    // printf("receiver cell at [%f, %f] has a flux value of [%f, %f, %f, %f]\n", cl.tx, cl.ty, flux.r1, flux.r2, flux.r3, flux.r4);

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
};


#endif //GAMM_SCHEME_H
