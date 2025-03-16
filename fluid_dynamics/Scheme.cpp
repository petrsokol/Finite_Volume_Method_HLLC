//
// Created by petrs on 02.12.2023.
//

#include <cmath>
#include "Scheme.h"
#include "Def.h"
#include "Bound.h"

/*--------------------------------------------------------------------------------------------------------------------*/

void Scheme::updateCellDT (std::vector<Cell> & cells, double CFL, bool useGlobalTimeStep)
{
  // todo paralelizace
  double globalDT = 1e9;

  for (auto & cell: cells) {
    Primitive pv(cell.w);

    double u_xi = fabs(pv.u * cell.xi.ux + pv.v * cell.xi.uy);
    double u_eta = fabs(pv.u * cell.eta.ux + pv.v * cell.eta.uy);

    double d_xi = (u_xi + pv.c) / cell.xi.length;
    double d_eta = (u_eta + pv.c) / cell.eta.length;

    double res = CFL / (d_xi + d_eta);

    if (useGlobalTimeStep) {
      globalDT = fmin(globalDT, res);
    } else {
      cell.dt = res;
    }
  }

  // global time step is assigned to every cell
  if (useGlobalTimeStep)
    for (auto & cell: cells)
      cell.dt = globalDT;
}

/*--------------------------------------------------------------------------------------------------------------------*/

Conservative Scheme::HLL (const Interface & f, Conservative & wl, Conservative & wr)
{
  Conservative res{};

  Primitive pvl(wl);
  Primitive pvr(wr);

  double ql = pvl.u * f.nx + pvl.v * f.ny;
  double qr = pvr.u * f.nx + pvr.v * f.ny;

  double SL = fmin(ql - pvl.c, qr - pvr.c);
  double SR = fmax(ql + pvl.c, qr + pvr.c);

  Conservative FL = Scheme::flux(f, wl, ql, pvl.p);
  Conservative FR = Scheme::flux(f, wr, qr, pvr.p);

  if (SL > 0) {
    res = FL;
  } else if (SL <= 0 && 0 <= SR) {
    res = (SR * FL - SL * FR + SR * SL * (wr - wl)) / (SR - SL);
  } else {
    res = FR;
  }

  return res;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Scheme::computeW (Conservative & wl, Conservative & wr,
                       const Cell & cll, const Cell & cl,
                       const Cell & cr, const Cell & crr)
{
  if (Def::isSecOrd) {
    double centroidDist = centroidDistance(cr, cl);

    const Conservative sigma_l_forward = (cr.w - cl.w) / centroidDist;
    const Conservative sigma_l_backward = (cl.w - cll.w) / centroidDist;
    const Conservative sigma_r_forward = (crr.w - cr.w) / centroidDist;
    const Conservative sigma_r_backward = (cr.w - cl.w) / centroidDist;

    Conservative sigma_l = minmod(sigma_l_forward, sigma_l_backward);
    Conservative sigma_r = minmod(sigma_r_forward, sigma_r_backward);

    wl = cl.w + centroidDist / 2 * sigma_l;
    wr = cr.w - centroidDist / 2 * sigma_r;
  } else {
    wl = cl.w;
    wr = cr.w;
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

double Scheme::computeRezi (const MeshParams & mp, const std::vector<Cell> & cells)
{
  double res = 0;

  for (int j = 0; j < mp.Y_INNER; ++j) {
    for (int i = 0; i < mp.X_INNER; ++i) {
      int k = mp.FIRST_INNER + i + j * mp.X_CELLS;
      res += pow(cells.at(k).rezi.r1 / cells.at(k).dt, 2) * cells.at(k).area;
    }
  }

  return log(sqrt(res));
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Scheme::updateCells (const MeshParams & mp, std::vector<Cell> & cells)
{
  for (int i = 0; i < mp.TOTAL_INNER; ++i) {
    int k = mp.innerIndex(i);
    cells.at(k).w += cells.at(k).rezi;
    cells.at(k).rezi = 0;
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

double Scheme::computeCP (const Primitive & pv)
{
  return (pv.p - Bound::p_infty) /
         (0.5 * Bound::rho_infty * (Bound::u_infty * Bound::u_infty + Bound::v_infty * Bound::v_infty));
}

/*--------------------------------------------------------------------------------------------------------------------*/

Conservative Scheme::HLLC (const Interface & f, Conservative & wl, Conservative & wr)
{
  Primitive pvl(wl);
  Primitive pvr(wr);

  double ql = pvl.u * f.nx + pvl.v * f.ny; // normálová rychlost
  double qr = pvr.u * f.nx + pvr.v * f.ny; // DP - \tilde u

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

  Conservative wlStar = 1 / (SL - SM) * Scheme::fluxStar(f, wl, ql, SL, SM, pvl.p, p_star);
  Conservative wrStar = 1 / (SR - SM) * Scheme::fluxStar(f, wr, qr, SR, SM, pvr.p, p_star);

  if (SL > 0) {
    return Scheme::flux(f, wl, ql, pvl.p);
  } else if (SL <= 0 && 0 < SM) {
    return Scheme::flux(f, wlStar, SM, p_star);
  } else if (SM <= 0 && 0 <= SR) {
    return Scheme::flux(f, wrStar, SM, p_star);
  } else {
    return Scheme::flux(f, wr, qr, pvr.p);
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

Conservative Scheme::flux (Interface face, Conservative w, double q, double p)
{
  Conservative res{};

  res.r1 = w.r1 * q;
  res.r2 = w.r2 * q + p * face.nx;
  res.r3 = w.r3 * q + p * face.ny;
  res.r4 = (w.r4 + p) * q;
  return res;
}

/*--------------------------------------------------------------------------------------------------------------------*/

Conservative Scheme::fluxStar (Interface face, Conservative w, double q, double S, double SM, double p, double p_star)
{
  Conservative res{};

  res.r1 = w.r1 * (S - q) + 0;
  res.r2 = w.r2 * (S - q) + (p_star - p) * face.nx;
  res.r3 = w.r3 * (S - q) + (p_star - p) * face.ny;
  res.r4 = w.r4 * (S - q) + p_star * SM - p * q;
  return res;
}

/*--------------------------------------------------------------------------------------------------------------------*/

double Scheme::bar (double rho_l, double rho_r, double vl, double vr)
{
  return (sqrt(rho_l) * vl + sqrt(rho_r) * vr) / (sqrt(rho_l) + sqrt(rho_r));
}

/*--------------------------------------------------------------------------------------------------------------------*/

double Scheme::minmod (double a, double b)
{
  if (a * b <= 0) {
    return 0;
  } else if (fabs(a) <= fabs(b) && a * b > 0) {
    return a;
  } else if (fabs(a) > fabs(b) && a * b > 0) {
    return b;
  } else {
    printf("sus: a = %f, b = %f\n", a, b);
    return 0;
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

Conservative Scheme::minmod (Conservative a, Conservative b)
{
  Conservative res;
  res.r1 = minmod(a.r1, b.r1);
  res.r2 = minmod(a.r2, b.r2);
  res.r3 = minmod(a.r3, b.r3);
  return res;
}

/*--------------------------------------------------------------------------------------------------------------------*/

double Scheme::centroidDistance (const Cell & c1, const Cell & c2)
{
  return hypot((c2.tx - c1.tx), (c2.ty - c1.ty));
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Scheme::setInitialCondition (std::vector<Cell> & cells, const Conservative & wInitial)
{
  for (auto & cell : cells)
    cell.w = wInitial;
}

/*--------------------------------------------------------------------------------------------------------------------*/

double Scheme::computeMach (const Primitive & pv)
{
  return pv.U / pv.c;
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*
   if (test && Def::isNaca) {
    for (int i = 0; i < NACA::WALL_LENGTH; ++i) {
      int k = Def::firstInnerPoint + NACA::WALL_START + i;
      // there are two faces for every cell - horizontal indices are even
      Interface face = faces.at(2 * k + 1);

      // compute flux
      Conservative flux = Def::isHLLC
                          ? HLLC(face, cells.at(face.l).w, cells.at(face.r).w)
                          : HLL(face, cells.at(face.l).w, cells.at(face.r).w);

      // subtract original flux
      cells.at(face.r).rezi -= cells.at(face.r).dt / cells.at(face.r).area * flux * face.len;
      cells.at(face.rr).rezi -= cells.at(face.rr).dt / cells.at(face.rr).area * flux * face.len;

      // create new flux
      Conservative updatedFlux = Conservative(0, face.nx, face.ny, 0);
      double p_1 = Primitive::computePV(cells.at(k).w).p;
      double p_2 = Primitive::computePV(cells.at(k + Def::xCells).w).p;
      double p_w = 1.5 * p_1 - 0.5 * p_2;
      updatedFlux = updatedFlux * p_w;
      cells.at(face.r).rezi += cells.at(face.r).dt / cells.at(face.r).area * updatedFlux * face.len;
      cells.at(face.rr).rezi += cells.at(face.rr).dt / cells.at(face.rr).area * updatedFlux * face.len;
    }
  }
 */