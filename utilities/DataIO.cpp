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
                << std::setw(4) << lt->tm_year + 1900 << "_"
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

std::string DataIO::getTimeStamp ()
{
  return getDate() + "_" + getTime();
}

/*--------------------------------------------------------------------------------------------------------------------*/

void
DataIO::exportMachWallToDat (std::vector<Point> & points, const std::string & name, int bottomStart,
                             int topStart, int len, const std::filesystem::path & dir)
{
  // open the stream
  std::ofstream stream(dir / name);

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
  for (int k = bottomStart; k < bottomStart + len; ++k) {
    double mach = Scheme::computeMach(Primitive(points.at(k).w));
    stream << mach << std::endl;
  }

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
  for (int k = topStart; k < topStart + len; ++k) {
    double mach = Scheme::computeMach(Primitive(points.at(k).w));
    stream << mach << std::endl;
  }

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
 * @param points
 * @param dir
 * @param name
 */
void
DataIO::exportWallPointsToDat (const MeshParams & mp, std::vector<Point> & points, const std::string & name,
                               const std::filesystem::path & dir)
{
  // open the stream
  std::filesystem::path filePath = dir / name;
  std::ofstream stream(filePath);

  for (int k = mp.WALL_START; k < mp.WALL_START + mp.WALL_LENGTH; ++k) {
    const Primitive & pointPV = Primitive(points.at(k).w);
    double mach = Scheme::computeMach(pointPV);
    double c_p = Scheme::computeCP(pointPV);

    // do not use commas as separators
    stream << points.at(k).x << " " << points.at(k).y << " " << "1" << " "
           << mach << " " << c_p << '\n';
  }

  // close stream
  stream.close();
}

/*--------------------------------------------------------------------------------------------------------------------*/

void DataIO::exportVector (const std::vector<double> & vector, const std::string & name,
                           const std::filesystem::path & dir)
{
  // Open the file stream
  std::filesystem::path filePath = dir / name;
  std::ofstream stream(filePath);

  size_t len = vector.size();
  for (size_t i = 0; i < len; ++i) {
    stream << i << " " << vector[i] << std::endl;
  }
  stream.close();

  std::cout << "DataIO::exportVector - Exported dat files to: " << filePath << "\n";
}

/*--------------------------------------------------------------------------------------------------------------------*/

void
DataIO::updatePointValues (const MeshParams & mp, const std::vector<Cell> & cells, std::vector<Point> & points)
{
  // iterate over inner cells
  for (int i = 0; i < mp.TOTAL_INNER; ++i) {
    int k = mp.innerIndex(i);

    // update the vertices of each cell
    const Conservative & cellW = cells.at(k).w;
    updateCorners(mp, points, k, cellW);
  }

  // averaging values based on number of contributors
  averagePointValues(points);
}

/*--------------------------------------------------------------------------------------------------------------------*/

void DataIO::resetPointValues (std::vector<Point> & points)
{
  for (auto & point: points) {
    point.resetW();
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void DataIO::averagePointValues (std::vector<Point> & points)
{
  for (auto & point: points) {
    int c = point.contributors;
    if (c == 0)
      continue;

    point.w = point.w / c;
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void DataIO::updateCorners (const MeshParams & mp, std::vector<Point> & points, int k, const Conservative & cellW)
{
  int pointIndex = mp.cellIndexToPointIndex(k);

  // bottom l corner
  points.at(pointIndex).updateW(cellW);

  // bottom r corner
  points.at(pointIndex + 1).updateW(cellW);

  // top l corner
  points.at(pointIndex + mp.X_POINTS).updateW(cellW);

  // top r corner
  points.at(pointIndex + mp.X_POINTS + 1).updateW(cellW);
}

/*--------------------------------------------------------------------------------------------------------------------*/
