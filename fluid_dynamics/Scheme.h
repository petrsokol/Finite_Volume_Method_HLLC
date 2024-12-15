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
  static Conservative HLL (const std::vector<Cell> & cells, const Interface & face);

  static Conservative HLLC (const std::vector<Cell> & cells, const Interface & face);

  // second order
  Conservative minmod (Conservative a, Conservative b);

  double minmod (double a, double b);

private:
  // support methods
  static Conservative flux (Interface face, Conservative w, double q, double p);

  static Conservative fluxStar (Interface face, Conservative w, double q, double S, double SM, double p, double p_star);

  static double bar (double rho_l, double rho_r, double vl, double vr);
};


#endif //GAMM_SCHEME_H
