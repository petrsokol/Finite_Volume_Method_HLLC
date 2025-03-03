//
// Created by petrs on 02.12.2023.
//

#ifndef GAMM_SCHEME_H
#define GAMM_SCHEME_H


#include <unordered_map>
#include "../structures/Conservative.h"
#include "../geometry/Cell.h"
#include "../geometry/Interface.h"
#include "Def.h"

class Scheme
{

public:
  static void updateCellDT (std::vector<Cell> & cells, double CFL, bool useGlobalTimeStep);

  static void updateCells (std::vector<Cell> & cells);

  static double computeRezi (const std::vector<Cell> & cells);

  static double computeCP (double p_inner);

  // numerical schemes
  static Conservative HLL (const Interface & f, Conservative & wl, Conservative & wr);

  static Conservative HLLC (const Interface & f, Conservative & wl, Conservative & wr);

  // second order
  static Conservative minmod (Conservative a, Conservative b);

  static double minmod (double a, double b);

  // iterator functions
  template <typename NumericalScheme>
  static void computeScheme (std::vector<Cell> & cells, const std::vector<Interface> & faces, NumericalScheme scheme)
  {
    // iterate over all *inner* interfaces
    int xLim = 2 * Def::xInner + 1;
    for (int j = 0; j < Def::yInner; ++j) {
      for (int i = 0; i < xLim; ++i) {
        int k = 2 * (Def::firstInner + j * Def::xCells) + i;
        updateInterface(cells, faces.at(k), scheme);
      }
    }

    // iterate over top row - only horizontal interfaces
    for (int i = 0; i < Def::xInner; ++i) {
      int k = 2 * (Def::firstInner + Def::yInner * Def::xCells + i) + 1;
      updateInterface(cells, faces.at(k), scheme);
    }

    // place for alternative wall flux
  }

private:
  // support methods
  static Conservative flux (Interface face, Conservative w, double q, double p);

  static Conservative fluxStar (Interface face, Conservative w, double q, double S, double SM, double p, double p_star);

  static double bar (double rho_l, double rho_r, double vl, double vr);

  static double centroidDistance (const Cell & c1, const Cell & c2);

  static void computeW(Conservative & wl, Conservative & wr,
                       const Cell & cll, const Cell & cl, const Cell & cr, const Cell & crr);

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
};


#endif //GAMM_SCHEME_H
