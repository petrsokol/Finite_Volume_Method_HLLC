//
// Created by petrs on 02.12.2023.
//

#include <cmath>
#include <omp.h>

#include "Def.h"
#include "Scheme.h"
#include "Bound.h"

/*--------------------------------------------------------------------------------------------------------------------*/

Conservative Scheme::HLL (const Interface & f, Conservative & wl, Conservative & wr)
{
  Conservative res{};

  Primitive pvl(wl);
  Primitive pvr(wr);

  double ql = pvl.u * f.nx() + pvl.v * f.ny();
  double qr = pvr.u * f.nx() + pvr.v * f.ny();

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
    double centroidDist = Cell::centroidDistance(cr, cl);

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

Conservative Scheme::HLLC (const Interface & f, Conservative & wl, Conservative & wr)
{
  Primitive pvl(wl);
  Primitive pvr(wr);

  double ql = pvl.u * f.nx() + pvl.v * f.ny(); // normálová rychlost
  double qr = pvr.u * f.nx() + pvr.v * f.ny(); // DP - \tilde u

  double q_bar = bar(pvl.rho, pvr.rho, ql, qr); // viz Toro
  double h_bar = bar(pvl.rho, pvr.rho, pvl.h, pvr.h);
  double u_bar = bar(pvl.rho, pvr.rho, pvl.u, pvr.u);
  double v_bar = bar(pvl.rho, pvr.rho, pvl.v, pvr.v);
  double U_bar_sq = pow(u_bar, 2) + pow(v_bar, 2); // certified J. Holman verze
  double c_bar = sqrt((KAPPA - 1) * (h_bar - 0.5 * U_bar_sq));

  double lambda_1 = ql - pvl.c;
  double lambda_m = qr + pvr.c;
  double lambda_1Roe = q_bar - c_bar;
  double lambda_mRoe = q_bar + c_bar;

  double SL = fmin(lambda_1, lambda_1Roe);
  double SR = fmax(lambda_m, lambda_mRoe);
  double SM = (pvr.rho * qr * (SR - qr) - pvl.rho * ql * (SL - ql) + pvl.p - pvr.p) /
              (pvr.rho * (SR - qr) - pvl.rho * (SL - ql));

  double p_star = pvl.rho * (ql - SL) * (ql - SM) + pvl.p;

  Conservative wlStar = 1 / (SL - SM) * fluxStar(f, wl, ql, SL, SM, pvl.p, p_star);
  Conservative wrStar = 1 / (SR - SM) * fluxStar(f, wr, qr, SR, SM, pvr.p, p_star);

  if (SL > 0) {
    return flux(f, wl, ql, pvl.p);
  } else if (SL <= 0 && 0 < SM) {
    return flux(f, wlStar, SM, p_star);
  } else if (SM <= 0 && 0 <= SR) {
    return flux(f, wrStar, SM, p_star);
  } else {
    return flux(f, wr, qr, pvr.p);
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

Conservative Scheme::flux (Interface face, Conservative w, double q, double p)
{
  Conservative res{};

  res.r1 = w.r1 * q;
  res.r2 = w.r2 * q + p * face.nx();
  res.r3 = w.r3 * q + p * face.ny();
  res.r4 = (w.r4 + p) * q;

  return res;
}

/*--------------------------------------------------------------------------------------------------------------------*/

Conservative Scheme::fluxStar (Interface face, Conservative w, double q, double S, double SM, double p, double p_star)
{
  Conservative res{};

  res.r1 = w.r1 * (S - q) + 0;
  res.r2 = w.r2 * (S - q) + (p_star - p) * face.nx();
  res.r3 = w.r3 * (S - q) + (p_star - p) * face.ny();
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

double Scheme::computeMach (const Primitive & pv)
{
  return pv.U / pv.c;
}

/*--------------------------------------------------------------------------------------------------------------------*/

// (dt / dualArea) * flux * face.len
Conservative Scheme::eulerIncrement (const Cell & c, const Interface & f, const Conservative & flux)
{
  return c.dt / c.area * flux * f.len();
}

/*--------------------------------------------------------------------------------------------------------------------*/

// dt / (dualArea * Re) * (R * n_x + S * n_y) * f.len
// dt / (dualArea * Re) * rHat * f.len
Conservative Scheme::viscousTerms (const Cell & c, const Interface & f, const Conservative & rHat)
{
  Conservative res = c.dt / (Def::Re * c.area) * rHat * f.len();
//  if (res.r1 > 1e-8 || res.r2 > 1e-8 || res.r3 > 1e-8)
//    std::cout << "viscous term! " << std::endl;
  return res;
}

/*--------------------------------------------------------------------------------------------------------------------*/

// returns rHat
Conservative Scheme::computeViscousFlux (const Interface & f, const Cell & cl, const Cell & cr)
{
  const double & mu = Def::mu;
  const double & Pr = Def::Pr;

  Primitive pvl(cl.w);
  Primitive pvr(cr.w);

  double u = 0.5 * (pvl.u + pvr.u);
  double v = 0.5 * (pvl.v + pvr.v);

  Primitive d_phi_dx = getDerivativesX(cl, cr, f);
  Primitive d_phi_dy = getDerivativesY(cl, cr, f);

  double du_dx = d_phi_dx.u;
  double du_dy = d_phi_dy.u;
  double dv_dx = d_phi_dx.v;
  double dv_dy = d_phi_dy.v;

  double tau_xx = mu * (4.0 / 3 * du_dx - 2.0 / 3 * dv_dy);
  double tau_xy = mu * (du_dy + dv_dx);
  double tau_yy = mu * (4.0 / 3 * dv_dy - 2.0 / 3 * du_dx);

  // derivative of division: d/dx (f/g) = (f'g - fg') / g^2
  // d/dx p/rho = (p'rho - p*rho') / rho^2
  double p = 0.5 * (pvl.p + pvr.p);
  double rho = 0.5 * (pvl.rho + pvr.rho);

//  double d_p_rho_dx = d_phi_dx.p / d_phi_dx.rho;
//  double d_p_rho_dy = d_phi_dy.p / d_phi_dy.rho;
  double d_p_rho_dx = (d_phi_dx.p * rho - p * d_phi_dx.rho) / (rho * rho);
  double d_p_rho_dy = (d_phi_dy.p * rho - p * d_phi_dy.rho) / (rho * rho);

  double q_x = -KAPPA / (KAPPA - 1) * mu / Pr * d_p_rho_dx;
  double q_y = -KAPPA / (KAPPA - 1) * mu / Pr * d_p_rho_dy;

  Conservative rHat;

  rHat.r1 = 0;
  rHat.r2 = tau_xx * f.nx() + tau_xy * f.ny();
  rHat.r3 = tau_xy * f.nx() + tau_yy * f.ny();
  rHat.r4 = u * rHat.r2 + v * rHat.r3 - (q_x * f.nx() + q_y * f.ny());

  return rHat;
}

/*--------------------------------------------------------------------------------------------------------------------*/

Primitive Scheme::getDerivativesX (const Cell & cl, const Cell & cr, const Interface & f)
{
  Primitive pvb(f.p1.w);
  Primitive pvt(f.p2.w);
  Primitive pvl(cl.w);
  Primitive pvr(cr.w);

  Primitive pv_br = 0.5 * (pvb + pvr);
  Primitive pv_rt = 0.5 * (pvr + pvt);
  Primitive pv_tl = 0.5 * (pvt + pvl);
  Primitive pv_lb = 0.5 * (pvl + pvb);

  Primitive pv_dx = 1 / f.dualArea *
                    (
                            pv_br * f.BR.len * f.BR.nx
                            + pv_rt * f.RT.len * f.RT.nx
                            + pv_tl * f.TL.len * f.TL.nx
                            + pv_lb * f.LB.len * f.LB.nx
                    );

  return pv_dx;
}

/*--------------------------------------------------------------------------------------------------------------------*/

Primitive Scheme::getDerivativesY (const Cell & cl, const Cell & cr, const Interface & f)
{
  Primitive pvb(f.p1.w);
  Primitive pvt(f.p2.w);
  Primitive pvl(cl.w);
  Primitive pvr(cr.w);

  Primitive pv_br = 0.5 * (pvb + pvr);
  Primitive pv_rt = 0.5 * (pvr + pvt);
  Primitive pv_tl = 0.5 * (pvt + pvl);
  Primitive pv_lb = 0.5 * (pvl + pvb);

  Primitive pv_dy = 1 / f.dualArea *
                    (
                            pv_br * f.BR.len * f.BR.ny
                            + pv_rt * f.RT.len * f.RT.ny
                            + pv_tl * f.TL.len * f.TL.ny
                            + pv_lb * f.LB.len * f.LB.ny
                    );

  return pv_dy;
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
      Conservative updatedFlux = Conservative(0, face.nx(), face.ny(), 0);
      double p_1 = Primitive::computePV(cells.at(k).w).p;
      double p_2 = Primitive::computePV(cells.at(k + Def::xCells).w).p;
      double p_w = 1.5 * p_1 - 0.5 * p_2;
      updatedFlux = updatedFlux * p_w;
      cells.at(face.r).rezi += cells.at(face.r).dt / cells.at(face.r).area * updatedFlux * face.len;
      cells.at(face.rr).rezi += cells.at(face.rr).dt / cells.at(face.rr).area * updatedFlux * face.len;
    }
  }
 */