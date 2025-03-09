//
// Created by petrs on 19.04.2024.
//

#ifndef GAMM_DATAIO_H
#define GAMM_DATAIO_H


#include <string>
#include <unordered_map>
#include "../geometry/Cell.h"

class DataIO
{

private:
  static const std::string CSV_HEADER;

public:
  static std::string getDate ();

  static void
  updatePointValues (const MeshParams & mp, const std::vector<Cell> & cells, std::vector<Point> & points);

  static void
  exportPointsToCSV (const MeshParams & mp, std::vector<Point> & updatedPoints, const std::string & dir,
                     const std::string & name);

  static void
  exportWallPointsToDat (const MeshParams & mp, std::vector<Point> & updatedPoints, const std::string & dir,
                         const std::string & name);

  static void exportVectorToDat (const std::vector<double> & vector, const std::string & dir, const std::string & name);

  static std::string getTime ();

  static void updateCorners (const MeshParams & mp, std::vector<Point> & points, int l, double mach, double cp);

  static void averagePointValues (std::vector<Point> & points);
};


#endif //GAMM_DATAIO_H
