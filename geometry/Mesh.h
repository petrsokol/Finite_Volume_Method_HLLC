//
// Created by petrs on 03.03.2025.
//

#ifndef GAMM_MESH_H
#define GAMM_MESH_H


#include <vector>
#include "Point.h"
#include "Cell.h"
#include "Interface.h"

class Mesh
{
public:
  // attributes
  std::vector<Point> points;
  std::vector<Cell> cells;
  std::vector<Interface> faces;

  // constructor
  Mesh (const std::string & pointMeshDir, const std::string & pointMeshFileName);
};


#endif //GAMM_MESH_H
