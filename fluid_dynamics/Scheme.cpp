//
// Created by petrs on 02.12.2023.
//

#include <cmath>
#include "Scheme.h"
#include "../structures/Primitive.h"
#include "Def.h"
#include "Bound.h"
#include "NACA.h"

void Scheme::updateCellDT (std::vector<Cell> & cells, double CFL, bool useGlobalTimeStep)
{
  double globalDT = 1e9;

  for (auto & cell: cells) {
    Primitive pv = Primitive::computePV(cell.w);

    double u_xi = fabs(pv.u * cell.xi.ux + pv.v * cell.xi.uy);
    double u_eta = fabs(pv.u * cell.eta.ux + pv.v * cell.eta.uy);

    double d_xi = (u_xi + pv.c) / cell.xi.length;
    double d_eta = (u_eta + pv.c) / cell.eta.length;

    double res = CFL / (d_xi + d_eta);

    if (useGlobalTimeStep) {
      globalDT = fmin(globalDT, res);
    } else {
      // local time step is assigned to each cell
      cell.dt = res;
    }
  }

  if (useGlobalTimeStep) {
    // global time step is assigned to every cell
    for (auto & cell: cells) {
      cell.dt = globalDT;
    }
  }
}

Conservative Scheme::HLL (const std::vector<Cell> & cells, const Interface & face)
{
  Conservative res{};
  Conservative wl = cells.at(face.l).w;
  Conservative wr = cells.at(face.r).w;
  Primitive pvl = Primitive::computePV(wl);
  Primitive pvr = Primitive::computePV(wr);

  if (_isnan(pvr.p)) {
    std::cout << "tlak je divnej";
    wr.toString();
    face.toString();
  }

  double ql = pvl.u * face.nx + pvl.v * face.ny; // normálová rychlost
  double qr = pvr.u * face.nx + pvr.v * face.ny;

  double SL = fmin(ql - pvl.c, qr - pvr.c);
  double SR = fmax(ql + pvl.c, qr + pvr.c);

  Conservative FL = Scheme::flux(face, wl, ql, pvl.p);
  Conservative FR = Scheme::flux(face, wr, qr, pvr.p);

  if (_isnan(FL.r1)) {
    std::cout << "error at l side " << face.l << std::endl;
    wl.toString();
  }
  if (_isnan(FR.r1)) {
    std::cout << "error at r side " << face.r << std::endl;
    wr.toString();
    Def::errorCount++;
  }

  if (SL > 0) {
    res = FL;
  } else if (SL <= 0 && 0 <= SR) {
    res = (SR * FL - SL * FR + SR * SL * (wr - wl)) / (SR - SL);
  } else if (SR < 0) {
    res = FR;
  } else {
    std::cout << "error in HLL scheme" << std::endl;
    Def::error = true;
  }

  return res;
}

void Scheme::computeScheme (std::vector<Cell> & cells,
                            const std::vector<Interface> & faces)
{
  // for every inner face in one row (+1 -> one extra vertical face at the end of the row)
  int xLim = 2 * Def::xInner + 1;

  // nested for loops iterate over inner faces (apart from the top horizontal row)
  for (int j = 0; j < Def::yInner; ++j) {
    for (int i = 0; i < xLim; ++i) {

      int index = 2 * (Def::firstInner + j * Def::xCells) + i;
      const Interface & face = faces.at(index);
      Conservative flux = Def::isHLLC ? HLLC(cells, face) : HLL(cells, face);
      cells.at(face.l).rezi -= cells.at(face.l).dt / cells.at(face.l).area * flux * face.len;
      cells.at(face.r).rezi += cells.at(face.r).dt / cells.at(face.r).area * flux * face.len;
    }
  }
  // final top row
  for (int i = 0; i < Def::xInner; ++i) {
    int index = 2 * (Def::firstInner + Def::yInner * Def::xCells + i) + 1;
    const Interface & face = faces.at(index);
    Conservative flux = Def::isHLLC ? HLLC(cells, face) : HLL(cells, face);
    cells.at(face.l).rezi -= cells.at(face.l).dt / cells.at(face.l).area * flux * face.len;
    cells.at(face.r).rezi += cells.at(face.r).dt / cells.at(face.r).area * flux * face.len;
  }

  bool test = true;
  if (test && Def::isNaca) {
    for (int i = 0; i < NACA::wingLength; ++i) {
      int k = Def::firstInnerPoint + NACA::wingStart + i;
      // there are two faces for every cell - horizontal indices are even
      Interface face = faces.at(2 * k);

      // subtract original flux
      Conservative flux = Def::isHLLC ? HLLC(cells, face) : HLL(cells, face);
      cells.at(face.r).rezi -= cells.at(face.r).dt / cells.at(face.r).area * flux * face.len;

      // create new flux
      Conservative updatedFlux = Conservative(0, face.nx, face.ny, 0);
      double p_1 = Primitive::computePV(cells.at(k).w).p;
      double p_2 = Primitive::computePV(cells.at(k + Def::xCells).w).p;
      double p_w = 1.5 * p_1 - 0.5 * p_2;
      updatedFlux = updatedFlux * p_w;
      cells.at(face.r).rezi += cells.at(face.r).dt / cells.at(face.r).area * updatedFlux * face.len;
    }
  }
}

double Scheme::computeRezi (const std::vector<Cell> & cells)
{
  double res = 0;

  for (int j = 0; j < Def::yInner; ++j) {
    for (int i = 0; i < Def::xInner; ++i) {
      int k = Def::firstInner + i + j * Def::xCells;
      res += pow(cells.at(k).rezi.r1 / cells.at(k).dt, 2) * cells.at(k).area;
    }
  }

  return log(sqrt(res));
}

void Scheme::updateCells (std::vector<Cell> & cells)
{
  for (int i = 0; i < Def::inner; ++i) {
    int k = Def::innerIndex(i);
    cells.at(k).w += cells.at(k).rezi;
    cells.at(k).rezi = 0;
  }
}

double Scheme::computeCP (double p_inner)
{
  return (p_inner - Bound::p_infty) / (0.5 * Bound::rho_infty * (pow(Bound::u_infty, 2) + pow(Bound::v_infty, 2)));
}

Conservative Scheme::HLLC (const std::vector<Cell> & cells, const Interface & face)
{
  Conservative wl = cells.at(face.l).w;
  Conservative wr = cells.at(face.r).w;
  Primitive pvl = Primitive::computePV(wl);
  Primitive pvr = Primitive::computePV(wr);

  double ql = pvl.u * face.nx + pvl.v * face.ny; // normálová rychlost
  double qr = pvr.u * face.nx + pvr.v * face.ny; // DP - \tilde u

  double q_bar = Scheme::bar(pvl.rho, pvr.rho, ql, qr); // viz Toro
  double h_bar = Scheme::bar(pvl.rho, pvr.rho, pvl.h, pvr.h);
  double u_bar = Scheme::bar(pvl.rho, pvr.rho, pvl.u, pvr.u);
  double v_bar = Scheme::bar(pvl.rho, pvr.rho, pvl.v, pvr.v);
  double U_bar_sq = pow(u_bar, 2) + pow(v_bar, 2); // certified J. Holman verze
  double c_bar = sqrt((Def::KAPPA - 1) * (h_bar - 0.5 * U_bar_sq));

  double lambda_1 = ql - pvl.c;
  double lambda_m = qr + pvr.c;
  double lambda_1Roe = q_bar - c_bar;
  double lambda_mRoe = q_bar + c_bar;

  double SL = fmin(lambda_1, lambda_1Roe);
  double SR = fmax(lambda_m, lambda_mRoe);
  double SM = (pvr.rho * qr * (SR - qr) - pvl.rho * ql * (SL - ql) + pvl.p - pvr.p) /
              (pvr.rho * (SR - qr) - pvl.rho * (SL - ql));

  double p_star = pvl.rho * (ql - SL) * (ql - SM) + pvl.p;

  Conservative wlStar = 1 / (SL - SM) * Scheme::fluxStar(face, wl, ql, SL, SM, pvl.p, p_star);
  Conservative wrStar = 1 / (SR - SM) * Scheme::fluxStar(face, wr, qr, SR, SM, pvr.p, p_star);

  if (SL > 0) {
    return Scheme::flux(face, wl, ql, pvl.p);
  } else if (SL <= 0 && 0 < SM) {
    return Scheme::flux(face, wlStar, SM, p_star);
  } else if (SM <= 0 && 0 <= SR) {
    return Scheme::flux(face, wrStar, SM, p_star);
  } else {
    return Scheme::flux(face, wr, qr, pvr.p);
  }
}

Conservative Scheme::flux (Interface face, Conservative w, double q, double p)
{
  Conservative res{};

  res.r1 = w.r1 * q;
  res.r2 = w.r2 * q + p * face.nx;
  res.r3 = w.r3 * q + p * face.ny;
  res.r4 = (w.r4 + p) * q;
  return res;
}

Conservative Scheme::fluxStar (Interface face, Conservative w, double q, double S, double SM, double p, double p_star)
{
  Conservative res{};

  res.r1 = w.r1 * (S - q) + 0;
  res.r2 = w.r2 * (S - q) + (p_star - p) * face.nx;
  res.r3 = w.r3 * (S - q) + (p_star - p) * face.ny;
  res.r4 = w.r4 * (S - q) + p_star * SM - p * q;
  return res;
}

double Scheme::bar (double rho_l, double rho_r, double vl, double vr)
{
  return (sqrt(rho_l) * vl + sqrt(rho_r) * vr) / (sqrt(rho_l) + sqrt(rho_r));
}

/*--------------------------------------------------------------------------------------------------------------------*/
// SECOND ORDER

double Scheme::minmod (double a, double b)
{
  if (a * b <= 0) {
    return 0;
  } else if (fabs(a) <= fabs(b) && a * b > 0) {
    return a;
  } else if (fabs(a) > fabs(b) && a * b > 0) {
    return b;
  } else {
    printf("sus\n");
    return 0;
  }
}

Conservative Scheme::minmod (Conservative a, Conservative b)
{
  Conservative res;
  res.r1 = minmod(a.r1, b.r1);
  res.r2 = minmod(a.r2, b.r2);
  res.r3 = minmod(a.r3, b.r3);
  return res;
}

/*--------------------------------------------------------------------------------------------------------------------*/
