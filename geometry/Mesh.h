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
  Mesh (const std::string & name, const std::filesystem::path & path, const MeshParams & mp);

  // methods
  void exportResults (const std::filesystem::path & parentDir);

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

  void exportPoints (const std::string & fileName, const std::filesystem::path & dir = "");
};


#endif //GAMM_MESH_H
