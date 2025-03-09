//
// Created by petrs on 02.12.2023.
//

#ifndef GAMM_SCHEME_H
#define GAMM_SCHEME_H


#include <unordered_map>
#include "../structures/Conservative.h"
#include "../geometry/Cell.h"
#include "../geometry/Interface.h"
#include "../utilities/DataIO.h"
#include "../utilities/Instructions.h"
#include "../geometry/Mesh.h"

class Scheme
{

public:
  static void updateCellDT (std::vector<Cell> & cells, double CFL, bool useGlobalTimeStep);

  static void updateCells (const MeshParams & mp, std::vector<Cell> & cells);

  static double computeRezi (const MeshParams & mp, const std::vector<Cell> & cells);

  static double computeCP (double p_inner);

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
    std::vector<double> reziVec{};

    // set initial condition
    Scheme::setInitialCondition(mesh.cells, wInitial);

    // main iteration loop
    while (rezi > epsilon && reps < repsMax) {
      reps++;

      Scheme::updateCellDT(mesh.cells, CFL, useGlobalTimeStep);
      boundsIterator(mesh.mp, mesh.cells, mesh.faces);
      Scheme::computeScheme(mesh.mp, mesh.cells, mesh.faces, scheme);

      reziVec.push_back(rezi = Scheme::computeRezi(mesh.mp, mesh.cells));
      Scheme::updateCells(mesh.mp, mesh.cells);

      if (reps % 50 == 0) {
        std::cout << "reps: " << reps << ", rezi: " << rezi << std::endl;
      }
    }

    // export point vertices for paraView
    DataIO::exportPointsToCSV(mesh.mp, mesh.cells, mesh.points,
                              Instructions::dataInput, Instructions::verticesName);

    // export points to see Ma and c_p along bottom wall
    DataIO::exportPointsToDat(mesh.mp, mesh.cells, mesh.points,
                              Instructions::dataInput, Instructions::wallName);

    // export for rezi chart
    DataIO::exportVectorToDat(reziVec, Instructions::dataInput, Instructions::reziName);

    // run post-processing python scripts
    Instructions::generateInstructions();
    std::system(R"(python C:\Users\petrs\Documents\CTU\BP\PYTHON-scripts\mach-cp-charts.py)");
    std::system(R"(python C:\Users\petrs\Documents\CTU\BP\PYTHON-scripts\rezi-chart.py)");
    std::system(
            "cmd /c \"\"C:\\Program Files\\ParaView 5.10.1-Windows-Python3.9-msvc2017-AMD64\\bin\\"
            "pvpython.exe\" \"C:\\Users\\petrs\\Documents\\CTU\\BP\\PYTHON-scripts\\paraView-macro-minimal.py\"\"");

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
