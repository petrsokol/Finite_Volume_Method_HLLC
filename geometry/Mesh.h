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
  const std::string name;
  const MeshParams mp;
  std::vector<Point> points;
  std::vector<Cell> cells;
  std::vector<Interface> faces;
  std::vector<double> reziVec;

  // constructors
  Mesh (const std::string & pointMeshDir, const std::string & pointMeshFileName, int X_INNER, int Y_INNER,
        int GHOST_LAYERS, int WALL_START, int WALL_LENGTH);

  Mesh (const std::string & completeDir, const MeshParams & mp);

  Mesh (const std::string & name, const std::string & completeDir, const MeshParams & mp);

  // methods
  void exportResults (const std::string & parentDir);

  void exportResults (const std::string & parentDir, const std::string & childDir);

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

  void exportPointsToCSV (const std::filesystem::path & dir, const std::string & name);
};


#endif //GAMM_MESH_H
