//
// Created by petrs on 03.03.2025.
//

#include <filesystem>
#include <fstream>
#include "Mesh.h"
#include "../utilities/DataIO.h"
#include "../utilities/Instructions.h"
#include "../utilities/Timer.h"

/*--------------------------------------------------------------------------------------------------------------------*/

Mesh::Mesh (const std::string & pointMeshDir, const std::string & pointMeshFileName, int X_INNER, int Y_INNER,
            int GHOST_LAYERS, int WALL_START, int WALL_LENGTH) :
        mp(X_INNER, Y_INNER, GHOST_LAYERS, WALL_START, WALL_LENGTH)
{
  // points
  Mesh::points = Point::loadPointsFromFile(pointMeshDir, pointMeshFileName, mp);

  // faces
  Mesh::faces = Interface::createFaces(points, mp);

  // cells
  Mesh::cells = Cell::createCells(points, mp);
}

/*--------------------------------------------------------------------------------------------------------------------*/

Mesh::Mesh (const std::string & name, const std::string & completeDir, const MeshParams & mp) :
        mp(mp), name(name)
{
  Mesh::points = Point::loadPointsFromFile(completeDir, mp);
  Mesh::faces = Interface::createFaces(points, mp);
  Mesh::cells = Cell::createCells(points, mp);
}

/*--------------------------------------------------------------------------------------------------------------------*/

Mesh::Mesh (const std::string & completeDir, const MeshParams & mp) : Mesh("unnamedMesh", completeDir, mp)
{

}

/*--------------------------------------------------------------------------------------------------------------------*/

void Mesh::centroidsToVertices ()
{

}

/*--------------------------------------------------------------------------------------------------------------------*/

void Mesh::exportPointsToCSV (const std::filesystem::path & dir, const std::string & name)
{
  try {
    // Ensure the directory exists
    if (!std::filesystem::exists(dir)) {
      // Create parent directories if needed
      std::cerr << "Mesh::exportPointsToCSV - directory does not exist, creating..." << std::endl;
      std::filesystem::create_directories(dir);
    }

    // Create the full file path
    std::filesystem::path filePath = dir / name;

    // Open the file stream
    std::ofstream stream(filePath);
    if (!stream) {
      throw std::ios_base::failure("Failed to open file: " + filePath.string());
    }

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

  } catch (const std::exception &e) {
    std::cerr << "Error exporting CSV: " << e.what() << "\n";
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Mesh::exportResults (const std::string & parentDir, const std::string & childDir)
{
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

  // export data for ParaView
  exportPointsToCSV(childDir, Instructions::verticesName);

  // export data for Mach and c_p along bottom wall
  DataIO::exportWallPointsToDat(mp, points, childDir, Instructions::wallName);

  // export data for rezi chart
  DataIO::exportVectorToDat(reziVec, childDir, Instructions::reziName);

  // export timers
  DataIO::exportVectorToDat(Timer::reziTimer, childDir, "reziTimer.dat");
  DataIO::exportVectorToDat(Timer::computeSchemeTimer, childDir, "computeSchemeTimer.dat");
  DataIO::exportVectorToDat(Timer::cellDtTimer, childDir, "cellDtTimer.dat");
  DataIO::exportVectorToDat(Timer::updateCellsTimer, childDir, "updateCellsTimer.dat");
  DataIO::exportVectorToDat(Timer::boundsIteratorTimer, childDir, "boundsIteratorTimer.dat");

  // export Mach values along both walls (GAMM)
  int topWallStart = mp.WALL_START + mp.X_POINTS * (mp.Y_INNER_POINTS - 1);
  DataIO::exportMachWallToDat(points, childDir, "GAMM_bot_wall.dat",
                              mp.WALL_START, topWallStart, mp.WALL_LENGTH);

  Instructions::generateInstructions();
  int val;
  val = std::system("python3 ../post_processing_python_scripts/mach-cp-charts.py");
  val = std::system("python3 ../post_processing_python_scripts/rezi-chart.py");
  val = std::system("python3 ../post_processing_python_scripts/timer-chart.py");
  val = std::system("python3 ../post_processing_python_scripts/paraView-macro-minimal.py");
  val = std::system("python3 /mnt/c/python/BP_Python_Charts/OpenFoam-multiple-wall-visualiser.py "
                    "/mnt/c/cpp/BP/GAMM/output_dir/ "
                    "/home/sokolpe1/OpenFOAM/myFoam/tutorials/myLusgsFoam/transonicChannel/20000/Ma "
                    "/mnt/c/cpp/BP/GAMM/output_dir/GAMM_bot_wall.dat");
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Mesh::exportResults (const std::string & parentDir)
{
  std::string timeStamp = DataIO::getTimeStamp();
  exportResults(parentDir, timeStamp);
}

/*--------------------------------------------------------------------------------------------------------------------*/
