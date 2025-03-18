#include <iostream>
#include <cmath>
#include "structures/Conservative.h"
#include "fluid_dynamics/Def.h"
#include "fluid_dynamics/Scheme.h"
#include "fluid_dynamics/NACA.h"
#include "utilities/DataIO.h"
#include "utilities/Instructions.h"
#include "geometry/Mesh.h"
#include "fluid_dynamics/GAMM.h"

int main ()
{
  Instructions::createName();

  /*------------------------------------------------------------------------------------------------------------------*/
  // BOUNDARY CONDITIONS
  // set conditions by rho, p_in, alpha, p_out
  Def::setConditions(1, 1, 0, 0.737);
  // todo mach 0.8 nesymetricky, 0.5 symm

  // set conditions by mach number and angle of attack
  //  Def::setConditions(0.85, 0);

  /*------------------------------------------------------------------------------------------------------------------*/
  // INITIAL CONDITIONS
  Def::setInitialCondition(Def::wInitialSubsonic);

  /*------------------------------------------------------------------------------------------------------------------*/
  // PREPARE MESH
  // todo add boundsIterator as a mesh parameter, since it makes sense

  // naca mesh
  MeshParams nacaMP(260, 60, NACA::WALL_START, NACA::WALL_LENGTH);
  Mesh naca("files/nacaMesh.dat", nacaMP);

  // gamm mesh
  MeshParams gammMP(150, 50, GAMM::WALL_START, GAMM::WALL_LENGTH);
  Mesh gamm("files/gammMesh.dat", gammMP);
  // run experiments

  /*------------------------------------------------------------------------------------------------------------------*/
  // RUN EXPERIMENT
  // todo jeden parametr - struct
  Scheme::runExperiment(gamm, Scheme::HLL, GAMM::updateBounds, Def::wInitial,
                -15, 2000, 0.7, false);

  /*------------------------------------------------------------------------------------------------------------------*/

  std::cout << "program ended at " << DataIO::getTime() << std::endl;
  std::cout << "nashledanou" << std::endl;
  return 0;
}

/*
 * UŽITEČNÝ KLÁVESOVÝ ZKRATKY V CLION
 * ctrl + shift + V - historie vkládání
 * ctrl + p - nápověda parametrů funkce
 */