//
// Created by petrs on 03.03.2025.
//

#include "Mesh.h"

/*--------------------------------------------------------------------------------------------------------------------*/

Mesh::Mesh (const std::string & pointMeshDir, const std::string & pointMeshFileName,
            int X_INNER, int Y_INNER, int GHOST_LAYERS) :
        // initialize ghost layers
        GHOST_LAYERS(GHOST_LAYERS),
        // initialize cells
        X_INNER(X_INNER), Y_INNER(Y_INNER), TOTAL_INNER(X_INNER * Y_INNER),
        X_CELLS(X_INNER + 2 * GHOST_LAYERS), Y_CELLS(Y_INNER + 2 * GHOST_LAYERS), TOTAL_CELLS(X_CELLS * Y_CELLS),
        // initialize points
        X_INNER_POINTS(X_INNER + 1), Y_INNER_POINTS(Y_INNER + 1), TOTAL_INNER_POINTS(X_INNER_POINTS * Y_INNER_POINTS),
        // initialize
        X_POINTS(X_INNER + 2 * GHOST_LAYERS + 1), Y_POINTS(Y_INNER + 2 * GHOST_LAYERS + 1),
        TOTAL_POINTS(X_POINTS * Y_POINTS),
        // initialize first inner cell and point indices
        FIRST_INNER(GHOST_LAYERS * X_CELLS + GHOST_LAYERS),
        FIRST_INNER_POINT(GHOST_LAYERS * X_POINTS + GHOST_LAYERS)
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
