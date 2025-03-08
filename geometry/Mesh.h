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

  // how many inner cells there are
  const int X_INNER, Y_INNER, TOTAL_INNER;

  // how many ghost cell layers there are
  const int GHOST_LAYERS;

  // how many cells there are in total (including ghost cells)
  const int X_CELLS, Y_CELLS, TOTAL_CELLS;

  // how many inner vertices there are
  const int X_INNER_POINTS, Y_INNER_POINTS, TOTAL_INNER_POINTS;

  // how many total vertices there are
  const int X_POINTS, Y_POINTS, TOTAL_POINTS;

  // index of the first inner cell or first inner vertex
  const int FIRST_INNER, FIRST_INNER_POINT;

  /*------------------------------------------------------------------------------------------------------------------*/

  void centroidsToVerticesNaca();

  void centroidsToVerticesGamm();

  void centroidsToVertices();

  void exportPoints (const std::string & dir, const std::string & name);
};


#endif //GAMM_MESH_H
