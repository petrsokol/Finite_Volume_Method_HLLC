//
// Created by petrs on 19.04.2024.
//

#include <ctime>
#include <fstream>
#include <chrono>
#include <cmath>
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

void
DataIO::exportToCSV (const std::unordered_map<int, Cell> & cells, const std::string & dir, const std::string & name,
                     int reps)
{
  std::ofstream stream(dir + "\\" + name + "_" + getTime() + "_" + std::to_string(reps) + ".csv");
  stream << DataIO::CSV_HEADER;

  for (int i = 0; i < Def::inner; ++i) {
    int k = Def::innerIndex(i);
    Primitive pv(cells.at(k).w);
    double mach = pv.U / pv.c;
    stream << cells.at(k).tx << ", " << cells.at(k).ty << ", 1, " << mach << ", " << pv.p << "\n";
  }
  stream.close();
}

void
DataIO::exportToDAT (const std::unordered_map<int, Cell> & cells, const std::string & dir, const std::string & name,
                     int reps)
{
  std::ofstream stream(dir + "\\" + name + "_" + getTime() + "_" + std::to_string(reps) + ".dat");

  int upperBound = Def::isNaca ? NACA::wingLength : Def::xInner;
  int offset = Def::isNaca ? Def::firstInnerPoint + NACA::wingStart : Def::firstInnerPoint;

  for (int i = 0; i < upperBound; ++i) {
    int k = offset + i;
    Primitive pv(cells.at(k).w);
    double mach = pv.U / pv.c;
    stream << cells.at(k).tx << " " << cells.at(k).ty << " " << mach << " " << pv.p << "\n";
  }

  stream.close();
}

void DataIO::exportPointsToCSV (const std::vector<Cell> & cells, std::vector<Point> points, const std::string & dir,
                                const std::string & name)
{
  // prepare points with data from the simulation
  std::vector<Point> newPoints = DataIO::updatePointValues(cells, points);

  // open a stream and input a header
  std::ofstream stream(dir + "\\" + name);
  stream << DataIO::CSV_HEADER;

  // inner vertices - same as inner cells, plus one on each side
  int innerVertices = (Def::xInner + 1) * (Def::yInner + 1);

  // input data from each point
  for (int i = 0; i < innerVertices; ++i) {
    int k = Def::innerPointIndex(i);

    // input point coordinates
    stream << newPoints[k].x << ", " << newPoints[k].y << ", 1";

    // input data entries
    int dataEntries = newPoints[k].values.size();
    for (int j = 0; j < dataEntries; ++j) {
      stream << ", " << newPoints[k].values[j];
    }

    stream << std::endl;
  }

  // close the stream
  stream.close();
}

/**
 * Used to input mach number and pressure coefficient along the aerodynamic profile (NACA) or bottom wall (GAMM)
 * @param cells
 * @param points
 * @param dir
 * @param name
 */
void DataIO::exportPointsToDat (const std::vector<Cell> & cells, std::vector<Point> & points,
                                const std::string & dir, const std::string & name)
{
  // prepare points with data from the simulation
  std::vector<Point> newPoints = DataIO::updatePointValues(cells, points);

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

void DataIO::exportVectorToDat (const std::vector<double> & vector, const std::string & dir, const std::string & name)
{
  std::ofstream stream(dir + "\\" + name);

  int vectorSize = vector.size();
  for (int i = 0; i < vectorSize; ++i) {
    stream << i << " " << vector[i] << std::endl;
  }
  stream.close();
}

std::vector<Point> DataIO::updatePointValues (const std::vector<Cell> & cells, const std::vector<Point> & points)
{
  std::vector<Point> res = points;

  // reference cp and mach values
  Primitive pv(cells.at(Def::firstInnerPoint).w);
  Instructions::machLB = pv.U / pv.c;
  Instructions::machUB = pv.U / pv.c;
  Instructions::cpLB = Scheme::computeCP(pv.p);
  Instructions::cpUB = Scheme::computeCP(pv.p);

  // iterate over inner cells
  for (int i = 0; i < Def::inner; ++i) {
    int k = Def::innerIndex(i);
    Primitive pv(cells.at(k).w);
    double mach = pv.U / pv.c;
    double cp = Scheme::computeCP(pv.p);

    // for paraview charts
    Instructions::machUB = fmax(Instructions::machUB, mach);
    Instructions::machLB = fmin(Instructions::machLB, mach);
    Instructions::cpUB = fmax(Instructions::cpUB, cp);
    Instructions::cpLB = fmin(Instructions::cpLB, cp);

    // update cell's corners
    updateCorners(res, k, mach, cp);
  }

  if (Def::isNaca) {
    // taken from NACA::updatePeriodicity(...)
    // periodicity - start
    for (int i = 0; i < NACA::wingStart; ++i) {
      int l = Def::firstInnerPoint - Def::xPoints + Def::xInner - 1 -
              i; //o řadu níž, poslední index - jede v protisměru // l viz BP, p. 13

      Primitive pv(cells.at(l).w);
      double mach = pv.U / pv.c;
      double cp = Scheme::computeCP(pv.p);

      // update cell's corners
      updateCorners(res, l, mach, cp);
    }
    // periodicity - finish
    for (int i = 0; i < NACA::wingStart; ++i) {
      int l = Def::firstInnerPoint - Def::xPoints + NACA::wingStart - 1 - i; // -1 = těsně před koncem

      Primitive pv(cells.at(l).w);
      double mach = pv.U / pv.c;
      double cp = Scheme::computeCP(pv.p);

      // update cell's corners
      updateCorners(res, l, mach, cp);
    }
  }

  // averaging values based on number of contributors
  averagePointValues(res);

  // return result
  return res;
}

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

void DataIO::updatePoint (Point & p, double mach, double cp)
{
  p.values[0] += mach;
  p.values[1] += cp;
  p.contributors++;
}

void DataIO::updateCorners (std::vector<Point> & points, int l, double mach, double cp)
{
  int pointIndex = Def::cellIndexToPointIndex(l);

  // bottom l corner
  updatePoint(points.at(pointIndex), mach, cp);

  // bottom r corner
  updatePoint(points.at(pointIndex + 1), mach, cp);

  // top l corner
  updatePoint(points.at(pointIndex + Def::xPoints), mach, cp);

  // top r corner
  updatePoint(points.at(pointIndex + Def::xPoints + 1), mach, cp);
}

