//
// Created by petrs on 27.04.2024.
//

#ifndef GAMM_GAMM_H
#define GAMM_GAMM_H


class GAMM
{

public:
  static void updateInlet (std::vector<Cell> & cells);

  static void updateOutlet (std::vector<Cell> & cells);

  static void updateWalls (std::vector<Cell> & cells, const std::vector<Interface> & faces);

  static void updateBounds (std::vector<Cell> & cells, const std::vector<Interface> & faces);
};


#endif //GAMM_GAMM_H
