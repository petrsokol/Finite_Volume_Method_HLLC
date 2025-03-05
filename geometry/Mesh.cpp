//
// Created by petrs on 03.03.2025.
//

#include "Mesh.h"

/*--------------------------------------------------------------------------------------------------------------------*/

Mesh::Mesh (const std::string& pointMeshDir, const std::string& pointMeshFileName)
{
  // points
  Mesh::points = Point::loadPointsFromFile(pointMeshDir, pointMeshFileName);

  // faces
  Mesh::faces = Interface::createFaces(points);

  // cells
  Mesh::cells = Cell::createCells(points);
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Mesh::centroidsToVertices ()
{

}

/*--------------------------------------------------------------------------------------------------------------------*/

void Mesh::exportPoints (const std::string & dir, const std::string & name)
{

}
