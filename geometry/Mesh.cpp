//
// Created by petrs on 03.03.2025.
//

#include <filesystem>
#include <fstream>
#include <utility>
#include "Mesh.h"
#include "../utilities/DataIO.h"
#include "../utilities/Timer.h"

/*--------------------------------------------------------------------------------------------------------------------*/

Mesh::Mesh (const std::string & name, const std::filesystem::path & path, const MeshParams & mp)
 : name(name), mp(mp)
{
  Mesh::points = Point::loadPointsFromFile(path, mp);
  Mesh::cells = Cell::createCells(points, mp);
  Mesh::faces = Interface::createFaces(points, mp);
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Mesh::centroidsToVertices ()
{

}

/*--------------------------------------------------------------------------------------------------------------------*/

void Mesh::exportPoints (const std::string & fileName, const std::filesystem::path & dir)
{
    // Create the full file path
    std::filesystem::path filePath = dir / fileName;
    std::ofstream stream(filePath);

    // Write the header
    stream << DataIO::CSV_HEADER;

    // Write the data
    for (int i = 0; i < mp.TOTAL_INNER_POINTS; ++i) {
      int k = mp.innerPointIndex(i);

      stream << points[k].x << ", " << points[k].y << ", " << "1" << ", "
             << points[k].values[0] << ", " << points[k].values[1] << '\n';
    }

    stream.close();
    std::cout << "Exported CSV to: " << filePath << "\n";
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Mesh::exportResults (const std::filesystem::path & parentDir)
{
  // create subdirectory name
  std::string childDir = DataIO::getTimeStamp();

  // get the path this program is running in
  auto programPath = std::filesystem::current_path();

  // navigate to directory and create a subdirectory
  try {
    // Change the working directory
    std::filesystem::current_path(parentDir);
    std::cout << "Changed to directory: " << std::filesystem::current_path() << std::endl;

    // Create a subdirectory if it doesn't exist
    std::filesystem::create_directory(childDir);
    std::cout << "Created subdirectory: " << childDir << "\n";
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "Filesystem error: " << e.what() << "\n";
  }

  // set current path to child directory
  std::filesystem::current_path(childDir);
  auto childDirPath = std::filesystem::current_path();

  // export data for ParaView
  exportPoints("paraview_vertices.csv");

  // export data for Mach and c_p along bottom wall
  DataIO::exportWallPointsToDat(mp, points, "wall_points.dat");

  // export data for rezi chart
  DataIO::exportVector(reziVec, "rezi_vector.dat");

  // export timers
  DataIO::exportVector(Timer::reziTimer, "reziTimer.dat");
  DataIO::exportVector(Timer::computeSchemeTimer, "computeSchemeTimer.dat");
  DataIO::exportVector(Timer::cellDtTimer, "cellDtTimer.dat");
  DataIO::exportVector(Timer::updateCellsTimer, "updateCellsTimer.dat");
  DataIO::exportVector(Timer::boundsIteratorTimer, "boundsIteratorTimer.dat");

  // export Mach values along both walls (GAMM)
  int topWallStart = mp.WALL_START + mp.X_POINTS * (mp.Y_INNER_POINTS - 1);
  DataIO::exportMachWallToDat(points, "GAMM_bot_wall.dat",
                              mp.WALL_START, topWallStart, mp.WALL_LENGTH);
}

/*--------------------------------------------------------------------------------------------------------------------*/
