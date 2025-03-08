//
// Created by petrs on 19.12.2023.
//

#ifndef GAMM_NACA_H
#define GAMM_NACA_H


#include <unordered_map>
#include "../geometry/Cell.h"
#include "../geometry/Interface.h"

class NACA
{

public:
  static const int wingStart;
  static const int wingLength;

  static void updateInlet (const MeshParams & mp, std::vector<Cell> & cells);

  static void updateOutlet (const MeshParams & mp, std::vector<Cell> & cells);

  static void updateWalls (const MeshParams & mp, std::vector<Cell> & cells, const std::vector<Interface> & faces);

  static void updatePeriodicity (const MeshParams & mp, std::vector<Cell> & cells);

  static void updateBounds (const MeshParams & mp, std::vector<Cell> & cells, const std::vector<Interface> & faces);

  // todo to je ten ládikův problém jak všem metodám předávám to samý žejo, to by se dalo možná udělat líp
  // todo tahle třída je hrozně podobná GAMM.h, možná by se to dalo nějak chytře vyřešit
};


#endif //GAMM_NACA_H
