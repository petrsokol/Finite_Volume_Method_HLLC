//
// Created by petrs on 02.12.2023.
//

#include <cmath>
#include "Scheme.h"
#include "../structures/Primitive.h"
#include "Def.h"

double Scheme::computeDT(const std::unordered_map<int, Cell>& cells, double CFL) {
    double res = 1e9;
    for (const auto &cell: cells) {
        Primitive pv = Primitive::computePV(cell.second.w);

        double u_xi = fabs(pv.u * cell.second.xi.ux + pv.v * cell.second.xi.uy);
        double u_eta = fabs(pv.u * cell.second.eta.ux + pv.v * cell.second.eta.uy);

        double d_xi = (u_xi + pv.c) / cell.second.xi.length;
        double d_eta = (u_eta + pv.c) / cell.second.eta.length;

        double candidate = CFL / (d_xi + d_eta);
        res = fmin(res, candidate);
    }
    return res;
}

Conservative Scheme::HLLC(const std::unordered_map<int, Cell> &cells, const Interface &face) {
    Conservative res{};
    Conservative wl = cells.at(face.left).w;
    Conservative wr = cells.at(face.right).w;
    Primitive pvl = Primitive::computePV(wl);
    Primitive pvr = Primitive::computePV(wr);

    double ql = pvl.u * face.nx + pvl.v * face.ny; // normálová rychlost
    double qr = pvr.u * face.nx + pvr.v * face.ny; // DP - u_wave

    double q_bar = (sqrt(pvl.rho) * ql + sqrt(pvr.rho) * qr) / (sqrt(pvl.rho) + sqrt(pvr.rho));
    double h_bar = (sqrt(pvl.rho) * pvl.h + sqrt(pvr.rho) * pvr.h) / (sqrt(pvl.rho) + sqrt(pvr.rho));
    double U_bar_sq = (sqrt(pvl.rho) * pvl.U * pvl.U + sqrt(pvr.rho) * pvr.U * pvr.U) / (sqrt(pvl.rho) + sqrt(pvr.rho)); //TODO DOTAZ
//    double U_bar = (sqrt(pvl.rho) * pvl.U + sqrt(pvr.rho) * pvr.U) / (sqrt(pvl.rho) + sqrt(pvr.rho));
    double c_bar = sqrt((Def::KAPPA - 1) * (h_bar - 0.5 * U_bar_sq));

    double lambda_1 = ql - pvl.c;
    double lambda_m = qr + pvr.c;
    double lambda_1Roe = q_bar - c_bar;
    double lambda_mRoe = q_bar + c_bar;

    double SL = fmin(lambda_1, lambda_1Roe);
    double SR = fmax(lambda_m, lambda_mRoe);
    double SM = (pvr.rho * qr * (SR - qr) - pvl.rho * ql * (SL - ql) + pvl.p - pvr.p) / (pvr.rho * (SR - qr) - pvl.rho * (SL - ql));

    double p_star = pvl.rho * (ql - SL) * (ql - SM) + pvl.p;

    if (SL > 0) {
        res.r1 = pvl.rho * ql;
        res.r2 = pvl.rhoU * ql + pvl.p * face.nx;
        res.r3 = pvl.rhoV * ql + pvl.p * face.ny;
        res.r4 = (pvl.rhoE + pvl.p) * ql;

    } else if (SL <= 0 && 0 < SM) {
        double omegaL = 1 / (SL - SM);
        Conservative wStar{};
        wStar.r1 = (SL - ql) * pvl.rho; //násob omegou
        wStar.r2 = (SL - ql) * pvl.rhoU + (p_star - pvl.p) * face.nx;
        wStar.r3 = (SL - ql) * pvl.rhoV + (p_star - pvl.p) * face.ny;
        wStar.r4 = (SL - ql) * pvl.rhoE - pvl.p * ql + p_star * SM;
        wStar = omegaL * wStar;

        res.r1 = wStar.r1 * SM;
        res.r2 = wStar.r2 * SM + p_star * face.nx;
        res.r3 = wStar.r3 * SM + p_star * face.ny;
        res.r4 = (wStar.r4 + p_star) * SM;

    } else if (SM <= 0 && 0 <= SR) {
        double omegaR = 1 / (SR - SM);
        Conservative wStar{};
        wStar.r1 = (SR - qr) * pvr.rho; //násob omegou
        wStar.r2 = (SR - qr) * pvr.rhoU + (p_star - pvr.p) * face.nx;
        wStar.r3 = (SR - qr) * pvr.rhoV + (p_star - pvr.p) * face.ny;
        wStar.r4 = (SR - qr) * pvr.rhoE - pvr.p * qr + p_star * SM;
        wStar = omegaR * wStar;

        res.r1 = wStar.r1 * SM;
        res.r2 = wStar.r2 * SM + p_star * face.nx;
        res.r3 = wStar.r3 * SM + p_star * face.ny;
        res.r4 = (wStar.r4 + p_star) * SM; //opsaný s výměnou indexů, takže kdyžtak duplikát chyby

    } else if (SR < 0) {
        res.r1 = pvr.rho * qr; //opsaný zvrchu, takže kdyžtak chyba i tady
        res.r2 = pvr.rhoU * qr + pvr.p * face.nx;
        res.r3 = pvr.rhoV * qr + pvr.p * face.ny;
        res.r4 = (pvr.rhoE + pvr.p) * qr;

    } else {
        std::cout << "WHAT!!! \n";
        std::cout << "SL = " << SL << ", SM = " << SM << ", SR = " << SR << "\n";
        Def::error = true;
    }

    return res;
}

void Scheme::computeHLLC(std::unordered_map<int, Cell>& cells,
                         const std::unordered_map<std::pair<int, int>, Interface, pair_hash>& faces, double dt) {
    for (const auto &face: faces) {
        Conservative flux = HLLC(cells, face.second);

//        cells.at(face.second.left).toString();
//        std::cout << dt << " " << cells.at(face.second.left).area << "\n";
//        std::cout << dt / cells.at(face.second.left).area << "\n";
//        flux.toString();
//        (flux * face.second.length).toString();
//        (dt / cells.at(face.second.right).area * flux * face.second.length).toString();
//        std::cout << std::endl;

        cells.at(face.second.left).rezi -= dt / cells.at(face.second.left).area * flux * face.second.length;
        cells.at(face.second.right).rezi += dt / cells.at(face.second.right).area * flux * face.second.length;
    }

    for (int i = 0; i < Def::inner; ++i) {
        int k = Def::innerIndex(i);
        cells.at(k).w += cells.at(k).rezi; // TODO Dotaz
        cells.at(k).rezi = 0;
    }
}
