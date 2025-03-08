//
// Created by petrs on 03.03.2025.
//

#ifndef GAMM_MESH_H
#define GAMM_MESH_H


#include <vector>
#include "Point.h"
#include "Cell.h"
#include "Interface.h"
#include "MeshParams.h"

class Mesh
{
public:
  // attributes
  const MeshParams mp;
  std::vector<Point> points;
  std::vector<Cell> cells;
  std::vector<Interface> faces;
  std::vector<double> reziVec;

  // constructor
  Mesh (const std::string & pointMeshDir, const std::string & pointMeshFileName, int X_INNER, int Y_INNER,
        int GHOST_LAYERS);

  // methods
  void produceParaViewPictures(const std::string & dir, const std::string & name);

  void produceWallChart();

  void produceReziChart();

private:
  // attributes regarding mesh dimensions

  /*------------------------------------------------------------------------------------------------------------------*/



  /*------------------------------------------------------------------------------------------------------------------*/

  void centroidsToVerticesNaca();

  void centroidsToVerticesGamm();

  void centroidsToVertices();

  void exportPoints (const std::string & dir, const std::string & name);
};


#endif //GAMM_MESH_H
