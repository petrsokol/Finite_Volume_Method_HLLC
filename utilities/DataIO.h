//
// Created by petrs on 19.04.2024.
//

#ifndef GAMM_DATAIO_H
#define GAMM_DATAIO_H


#include <string>
#include <filesystem>
#include "../geometry/Cell.h"

class DataIO
{
public:
  static const std::string CSV_HEADER;

  static std::string getDate ();

  static std::string getTime ();

  static std::string getTimeStamp ();

  static void
  updatePointValues (const MeshParams & mp, const std::vector<Cell> & cells, std::vector<Point> & points);

  static void resetPointValues (std::vector<Point> & points);

  static void
  exportWallPointsToDat (const MeshParams & mp, std::vector<Point> & points, const std::string & name,
                         const std::filesystem::path & dir = "");

  static void
  exportMachWallToDat (std::vector<Point> & points, const std::string & name, int bottomStart,
                       int topStart, int len, const std::filesystem::path & dir = "");

  static void
  exportVector (const std::vector<double> & vector, const std::string & name,
                const std::filesystem::path & dir = "");

  static void updateCorners (const MeshParams & mp, std::vector<Point> & points, int k, const Conservative & cellW);

  static void averagePointValues (std::vector<Point> & points);
};


#endif //GAMM_DATAIO_H
