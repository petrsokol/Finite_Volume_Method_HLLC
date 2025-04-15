#include <iostream>
#include <cmath>
#include <omp.h>
#include "fluid_dynamics/Def.h"
#include "fluid_dynamics/Scheme.h"
#include "fluid_dynamics/NACA.h"
#include "utilities/DataIO.h"
#include "geometry/Mesh.h"
#include "fluid_dynamics/GAMM.h"

/*
 * HOW TO RUN WITH POST PROCESSING
 *
 * (optional):
 * /mnt/c/cpp/BP/GAMM/
 * cmake -B build -S .
 *
 * to run the script:
 * /mnt/c/cpp/BP/GAMM/build
 * make
 *
 * to run the post-processing
 * /mnt/c/cpp/BP/GAMM_results
 * ./runPython 2025_04_14_09h36m/ naca
 */

int main ()
{
  // parallel testing section
  #pragma omp parallel default(none) shared(std::cout)
  {
    std::cout << "pragma testing, thread num: "
              << omp_get_thread_num() << " of "
              << omp_get_num_threads() << "threads"
              << std::endl;
  }

  #if DEBUG_MODE
    std::cout << "running in debug mode." << std::endl;
  #endif

  // windows paths
  std::filesystem::path fullInputPath = "C:/cpp/BP/GAMM/files/";
  std::filesystem::path fullOutputPath = "C:/cpp/BP/GAMM_results/";

  // linux paths
//  std::filesystem::path fullInputPath = "/mnt/c/cpp/BP/GAMM/files";
//  std::filesystem::path fullOutputPath = "/mnt/c/cpp/BP/GAMM_results";

  // BOUNDARY CONDITIONS
  // set conditions by rho, p_in, alpha, p_out
  Def::setConditions(1, 1, 0, 0.737);

  // INITIAL CONDITIONS
  Def::setInitialCondition(Def::wInitialSubsonic);

  // naca mesh
  MeshParams nacaMP(260, 60, NACA::WALL_START, NACA::WALL_LENGTH);
  Mesh naca("naca", fullInputPath / "nacaMesh.dat", nacaMP);

  // gamm mesh
   MeshParams gammMP(150, 50, GAMM::WALL_START, GAMM::WALL_LENGTH);
   Mesh gamm("gamm", fullInputPath / "gammMesh.dat", gammMP);


  // RUN EXPERIMENT
  Scheme::runExperiment(gamm, Scheme::HLL, GAMM::updateBounds, Def::wInitial, -15, 5000, 0.7, false);

  // export results
  gamm.exportResults(fullOutputPath.string());


  /*------------------------------------------------------------------------------------------------------------------*/

  std::cout << "program ended at " << DataIO::getTime() << std::endl;
  std::cout << "nashledanou" << std::endl;

  return 0;
}

/*
 * UŽITEČNÝ KLÁVESOVÝ ZKRATKY V CLIONu
 * ctrl + shift + V - historie vkládání
 * ctrl + p - nápověda parametrů funkce
 * ctrl + w - označování větší a větší části kódu
 * ctrl + alt + t - wrap selection with
 *
 *   // set conditions by mach number and angle of attack
 *   //  Def::setConditions(0.85, 0);
 *
 * todo
 *    add boundsIterator as a mesh parameter, since it makes sense
 *    mach 0.8 nesymetricky, 0.5 symm
 *    jeden parametr - struct
 *    -- better naming system for different cases
 *    -- save data to folders instead of the current dumpsterfire mess
 */