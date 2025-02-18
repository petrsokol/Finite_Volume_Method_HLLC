//
// Created by petrs on 02.12.2023.
//

#ifndef GAMM_SCHEME_H
#define GAMM_SCHEME_H


#include <unordered_map>
#include "../structures/Conservative.h"
#include "../geometry/Cell.h"
#include "../geometry/Interface.h"

class Scheme
{

public:
  // iterator functions
  static void computeScheme (std::vector<Cell> & cells, const std::vector<Interface> & faces);

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

private:
  // support methods
  static Conservative flux (Interface face, Conservative w, double q, double p);

  static Conservative fluxStar (Interface face, Conservative w, double q, double S, double SM, double p, double p_star);

  static double bar (double rho_l, double rho_r, double vl, double vr);

  static double centroidDistance (const Cell & c1, const Cell & c2);

  static void computeW(Conservative & wl, Conservative & wr,
                       const Cell & cll, const Cell & cl, const Cell & cr, const Cell & crr);

  static void updateInterface (std::vector<Cell> & cells, const Interface & face);
};


#endif //GAMM_SCHEME_H
