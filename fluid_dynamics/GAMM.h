//
// Created by petrs on 27.04.2024.
//

#ifndef GAMM_GAMM_H
#define GAMM_GAMM_H


class GAMM
{

public:
  static const int WALL_START;
  static const int WALL_LENGTH;

  static void updateInlet (const MeshParams & mp, std::vector<Cell> & cells);

  static void updateOutlet (const MeshParams & mp, std::vector<Cell> & cells);

  static void updateWalls (const MeshParams & mp, std::vector<Cell> & cells, const std::vector<Interface> & faces);

  static void updateBounds (const MeshParams & mp, std::vector<Cell> & cells, const std::vector<Interface> & faces);
};


#endif //GAMM_GAMM_H
