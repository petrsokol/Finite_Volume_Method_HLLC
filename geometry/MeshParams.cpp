//
// Created by petrs on 08.03.2025.
//

#include "MeshParams.h"

/*--------------------------------------------------------------------------------------------------------------------*/

MeshParams::MeshParams (int X_INNER, int Y_INNER, int GHOST_LAYERS) :
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

}

/*--------------------------------------------------------------------------------------------------------------------*/

int MeshParams::pointIndexToCellIndex (int k) const
{
  return k % X_POINTS + (k / X_POINTS) * X_CELLS;
}

/*--------------------------------------------------------------------------------------------------------------------*/

int MeshParams::cellIndexToPointIndex (int k) const
{
  int pointIndex = k % X_CELLS + (k / X_CELLS) * X_POINTS;
  return pointIndex;
}

/*--------------------------------------------------------------------------------------------------------------------*/

int MeshParams::innerIndex (int i) const
{
  return FIRST_INNER + i % X_INNER + (i / X_INNER) * X_CELLS;
}

/*--------------------------------------------------------------------------------------------------------------------*/

int MeshParams::innerPointIndex (int i) const
{
  return FIRST_INNER_POINT + i % (X_INNER + 1) + (i / (X_INNER + 1)) * X_POINTS;
}

/*--------------------------------------------------------------------------------------------------------------------*/
