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

void DataIO::exportPointsToCSV (const MeshParams & mp, const std::vector<Cell> & cells, std::vector<Point> & points,
                                const std::string & dir, const std::string & name)
{
  // prepare points with data from the simulation
  std::vector<Point> newPoints = DataIO::updatePointValues(mp, cells, points);

  // open a stream and input a header
  std::ofstream stream(dir + "\\" + name);
  stream << DataIO::CSV_HEADER;

  // input data from each point
  for (int i = 0; i < mp.TOTAL_INNER_POINTS; ++i) {
    int k = mp.innerPointIndex(i);

    stream << newPoints[k].x << ", " << newPoints[k].y << ", " << "1" << ", "
           << newPoints[k].values[0] << ", " << newPoints[k].values[1] << std::endl;
  }

  // close the stream
  stream.close();
}

/*--------------------------------------------------------------------------------------------------------------------*/

/**
 * Used to input mach number and pressure coefficient along the aerodynamic profile (NACA) or bottom wall (GAMM)
 * @param cells
 * @param points
 * @param dir
 * @param name
 */
void DataIO::exportPointsToDat (const MeshParams & mp, const std::vector<Cell> & cells, std::vector<Point> & points,
                                const std::string & dir, const std::string & name)
{
  // prepare points with data from the simulation
  std::vector<Point> newPoints = DataIO::updatePointValues(mp, cells, points);

  // open the stream
  std::ofstream stream(dir + "\\" + name);

  int upperBound = Def::isNaca ? NACA::wingLength + 1 : Def::xInner + 1; // +1 -> vertices in row = points in row + 1
  int offset = Def::isNaca ? Def::firstInnerPoint + NACA::wingStart : Def::firstInnerPoint;

  for (int i = 0; i < upperBound; ++i) {
    int k = offset + i;
    stream << newPoints[k].x << " " << newPoints[k].y << " 1";

    // first: mach, second: cp
    for (int j = 0; j < newPoints[k].values.size(); ++j) {
      stream << " " << newPoints[k].values[j];
    }
    stream << std::endl;
  }

  // close stream
  stream.close();
}

/*--------------------------------------------------------------------------------------------------------------------*/

void DataIO::exportVectorToDat (const std::vector<double> & vector, const std::string & dir, const std::string & name)
{
  std::ofstream stream(dir + "\\" + name);

  int vectorSize = vector.size();
  for (int i = 0; i < vectorSize; ++i) {
    stream << i << " " << vector[i] << std::endl;
  }
  stream.close();
}

/*--------------------------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*/

std::vector<Point>
DataIO::updatePointValues (const MeshParams & mp, const std::vector<Cell> & cells, const std::vector<Point> & points)
{
  std::vector<Point> res = points;

  // iterate over inner cells
  for (int i = 0; i < mp.TOTAL_INNER; ++i) {
    int k = mp.innerIndex(i);

    // compute mach number and cp
    Primitive pv(cells.at(k).w);
    double mach = Scheme::computeMach(pv);
    double cp = Scheme::computeCP(pv);

    // update cell's corners
    updateCorners(mp, res, k, mach, cp);
  }

  if (Def::isNaca) {
    // taken from NACA::updatePeriodicity(...)
    // periodicity - start
    for (int i = 0; i < NACA::wingStart; ++i) {
      //o řadu níž, poslední index - jede v protisměru // l viz BP, p. 13
      int l = mp.FIRST_INNER_POINT - mp.X_POINTS + mp.X_INNER - 1 - i;

      Primitive pv(cells.at(l).w);
      double mach = Scheme::computeMach(pv);
      double cp = Scheme::computeCP(pv);

      // update cell's corners
      updateCorners(mp, res, l, mach, cp);
    }

    // periodicity - finish
    for (int i = 0; i < NACA::wingStart; ++i) {
      int l = mp.FIRST_INNER_POINT - mp.X_POINTS + NACA::wingStart - 1 - i; // -1 = těsně před koncem

      Primitive pv(cells.at(l).w);
      double mach = Scheme::computeMach(pv);
      double cp = Scheme::computeCP(pv);

      // update cell's corners
      updateCorners(mp, res, l, mach, cp);
    }
  }

  // averaging values based on number of contributors
  averagePointValues(res);

  // find bounds for good visualisation in ParaView
  Instructions::getMinMaxValues(res);

  // return result
  return res;
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
