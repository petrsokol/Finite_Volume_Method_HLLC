//
// Created by petrs on 08.05.2025.
//

#ifndef GAMM_BLASIUS_H
#define GAMM_BLASIUS_H

#include "../geometry/MeshParams.h"
#include "../geometry/Cell.h"
#include "../geometry/Interface.h"

class BLASIUS
{
public:
  static const int WALL_START;
  static const int WALL_LENGTH;

  static void updateInlet (const MeshParams & mp, std::vector<Cell> & cells);

  static void updateOutlet (const MeshParams & mp, std::vector<Cell> & cells);

  static void updateWalls (const MeshParams & mp, std::vector<Cell> & cells, const std::vector<Interface> & faces);

  static void updateSymmetry (const MeshParams & mp, std::vector<Cell> & cells, const std::vector<Interface> & faces);
};


#endif //GAMM_BLASIUS_H
