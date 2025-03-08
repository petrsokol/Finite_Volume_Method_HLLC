//
// Created by petrs on 03.03.2025.
//

#include "Mesh.h"

/*--------------------------------------------------------------------------------------------------------------------*/

Mesh::Mesh (const std::string & pointMeshDir, const std::string & pointMeshFileName,
            int X_INNER, int Y_INNER, int GHOST_LAYERS) :
        mp(X_INNER, Y_INNER, GHOST_LAYERS)
{
  // points
  Mesh::points = Point::loadPointsFromFile(pointMeshDir, pointMeshFileName, mp);

  // faces
  Mesh::faces = Interface::createFaces(points, mp);

  // cells
  Mesh::cells = Cell::createCells(points, mp);
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Mesh::centroidsToVertices ()
{

}

/*--------------------------------------------------------------------------------------------------------------------*/

void Mesh::exportPoints (const std::string & dir, const std::string & name)
{

}
