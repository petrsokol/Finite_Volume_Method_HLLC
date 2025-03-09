//
// Created by petrs on 08.03.2025.
//

#ifndef GAMM_MESHPARAMS_H
#define GAMM_MESHPARAMS_H


class MeshParams
{
public:
  // constructor
  MeshParams (int X_INNER, int Y_INNER, int GHOST_LAYERS, int WALL_START, int WALL_LENGTH);

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

  // start index and wall length
  const int WALL_LENGTH, WALL_START;

  // functions for index conversion between points and cells
  int innerIndex (int i) const;

  int innerPointIndex (int i) const;

  int pointIndexToCellIndex(int k) const;

  int cellIndexToPointIndex(int k) const;
};


#endif //GAMM_MESHPARAMS_H
