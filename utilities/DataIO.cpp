//
// Created by petrs on 19.04.2024.
//

#include <ctime>
#include <fstream>
#include <chrono>
#include "DataIO.h"
#include "../fluid_dynamics/Def.h"
#include "../structures/Primitive.h"
#include "../fluid_dynamics/NACA.h"
#include "../fluid_dynamics/Scheme.h"
#include "Instructions.h"

/*--------------------------------------------------------------------------------------------------------------------*/

const std::string DataIO::CSV_HEADER = "\"X\", \"Y\", \"Z\", \"M\", \"c_p\"\n";

/*--------------------------------------------------------------------------------------------------------------------*/

std::string DataIO::getDate ()
{
  // Get the current time
  std::time_t currentTime = std::time(nullptr);

  // Convert the current time to a local time
  std::tm * lt = std::localtime(&currentTime);

  // string stream for formatted date string
  std::ostringstream formattedDate;
  formattedDate << std::setfill('0')
                << std::setw(2) << lt->tm_year % 100 << "_"
                << std::setw(2) << lt->tm_mon + 1 << "_"
                << std::setw(2) << lt->tm_mday;

  // return res
  return formattedDate.str();
}

/*--------------------------------------------------------------------------------------------------------------------*/

std::string DataIO::getTime ()
{
  // Get current time
  auto now = std::chrono::system_clock::now();
  std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

  // Convert current time to tm struct for easy manipulation
  std::tm * timeInfo = std::localtime(&currentTime);

  // Create a string stream to build the formatted time string
  std::ostringstream formattedTime;
  formattedTime << std::setfill('0')
                << std::setw(2) << timeInfo->tm_hour << "h"
                << std::setw(2) << timeInfo->tm_min << "m";

  // Return the formatted time string
  return formattedTime.str();
}

/*--------------------------------------------------------------------------------------------------------------------*/

void DataIO::exportPointsToCSV (const MeshParams & mp, std::vector<Point> & updatedPoints, const std::string & dir,
                                const std::string & name)
{
  // open a stream and input a header
  std::ofstream stream(dir + name);
  stream << DataIO::CSV_HEADER;

  // input data from each point
  for (int i = 0; i < mp.TOTAL_INNER_POINTS; ++i) {
    int k = mp.innerPointIndex(i);

    stream << updatedPoints[k].x << ", " << updatedPoints[k].y << ", " << "1" << ", "
           << updatedPoints[k].values[0] << ", " << updatedPoints[k].values[1] << std::endl;
  }

  // close the stream
  stream.close();
}

/*--------------------------------------------------------------------------------------------------------------------*/

void
DataIO::exportMachWallToDat (std::vector<Point> & updatedPoints, const std::string & dir, const std::string & name,
                             int bottomStart, int topStart, int len)
{
  // open the stream
  std::ofstream stream(dir + name);

  // BOTTOM WALL

  // pass header
  stream << "boundaryField\n"
            "{\n"
            "    emptyPlanes\n"
            "    {\n"
            "        type            empty;\n"
            "    }\n"
            "    lowerWall\n"
            "    {\n"
            "        type            calculated;\n"
            "        value           nonuniform List<scalar> \n";
  // pass points count
  stream << len << std::endl;
  stream << "(" << std::endl;

  // pass data
  for (int i = bottomStart; i < bottomStart + len; ++i)
    stream << updatedPoints[i].values[0] << std::endl;

  // pass footer
  stream << ")\n"
            ";\n"
            "    }"
            << std::endl;

  // TOP WALL

  // pass header
  stream << "boundaryField\n"
            "{\n"
            "    emptyPlanes\n"
            "    {\n"
            "        type            empty;\n"
            "    }\n"
            "    upperWall\n"
            "    {\n"
            "        type            calculated;\n"
            "        value           nonuniform List<scalar> \n";
  // pass points count
  stream << len << std::endl;
  stream << "(" << std::endl;

  // pass data
  for (int i = topStart; i < topStart + len; ++i)
    stream << updatedPoints[i].values[0] << std::endl;

  // pass footer
  stream << ")\n"
            ";\n"
            "    }"
         << std::endl;

  // close stream
  stream.close();
}

/*--------------------------------------------------------------------------------------------------------------------*/

/**
 * Used to input mach number and pressure coefficient along the aerodynamic profile (NACA) or bottom wall (GAMM)
 * @param cells
 * @param updatedPoints
 * @param dir
 * @param name
 */
void
DataIO::exportWallPointsToDat (const MeshParams & mp, std::vector<Point> & updatedPoints, const std::string & dir,
                               const std::string & name)
{
  // open the stream
  std::ofstream stream(dir + name);

  for (int i = mp.WALL_START; i < mp.WALL_START + mp.WALL_LENGTH; ++i)
    stream << updatedPoints[i].x << " " << updatedPoints[i].y << " " << "1" << " "
           << updatedPoints[i].values[0] << " " << updatedPoints[i].values[1] << std::endl;

  // close stream
  stream.close();
}

/*--------------------------------------------------------------------------------------------------------------------*/

void DataIO::exportVectorToDat (const std::vector<double> & vector, const std::string & dir, const std::string & name)
{
  std::cout << "DataIO::exportVectorToDat - exporting " << name << " to " << dir << std::endl;
  std::ofstream stream(dir + name);

  size_t len = vector.size();
  for (size_t i = 0; i < len; ++i) {
    stream << i << " " << vector[i] << std::endl;
  }
  stream.close();
}

/*--------------------------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*/

void
DataIO::updatePointValues (const MeshParams & mp, const std::vector<Cell> & cells, std::vector<Point> & points)
{
  // iterate over inner cells
  for (int i = 0; i < mp.TOTAL_INNER; ++i) {
    int k = mp.innerIndex(i);

    // compute mach number and cp
    Primitive pv(cells.at(k).w);
    double mach = Scheme::computeMach(pv);
    double cp = Scheme::computeCP(pv);

    // update cell's corners
    updateCorners(mp, points, k, mach, cp);
  }

  if (Def::isNaca && false) {
    // taken from NACA::updatePeriodicity(...)
    // periodicity - start
    for (int i = 0; i < NACA::WALL_START; ++i) {
      //o řadu níž, poslední index - jede v protisměru // l viz BP, p. 13
      int l = mp.FIRST_INNER_POINT - mp.X_POINTS + mp.X_INNER - 1 - i;

      Primitive pv(cells.at(l).w);
      double mach = Scheme::computeMach(pv);
      double cp = Scheme::computeCP(pv);

      // update cell's corners
      updateCorners(mp, points, l, mach, cp);
    }

    // periodicity - finish
    for (int i = 0; i < NACA::WALL_START; ++i) {
      int l = mp.FIRST_INNER_POINT - mp.X_POINTS + NACA::WALL_START - 1 - i; // -1 = těsně před koncem

      Primitive pv(cells.at(l).w);
      double mach = Scheme::computeMach(pv);
      double cp = Scheme::computeCP(pv);

      // update cell's corners
      updateCorners(mp, points, l, mach, cp);
    }
  }

  // averaging values based on number of contributors
  averagePointValues(points);

  // find bounds for good visualisation in ParaView
  Instructions::getMinMaxValues(points);
}

/*--------------------------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*/

void DataIO::averagePointValues (std::vector<Point> & points)
{
  // average Mach number and pressure coefficient based on the number of neighbouring cells
  for (auto & point: points) {
    if (point.contributors == 0)
      continue;
    for (auto & value: point.values)
      value = value / point.contributors;
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void DataIO::updateCorners (const MeshParams & mp, std::vector<Point> & points, int l, double mach, double cp)
{
  int pointIndex = mp.cellIndexToPointIndex(l);

  // bottom l corner
  points.at(pointIndex).updateValues(mach, cp);

  // bottom r corner
  points.at(pointIndex + 1).updateValues(mach, cp);

  // top l corner
  points.at(pointIndex + mp.X_POINTS).updateValues(mach, cp);

  // top r corner
  points.at(pointIndex + mp.X_POINTS + 1).updateValues(mach, cp);
}

/*--------------------------------------------------------------------------------------------------------------------*/
