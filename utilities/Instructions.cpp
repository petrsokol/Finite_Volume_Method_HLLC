//
// Created by petrs on 05.05.2024.
//

#include <fstream>
#include <algorithm>
#include "Instructions.h"
#include "../fluid_dynamics/Def.h"
#include "DataIO.h"

/*--------------------------------------------------------------------------------------------------------------------*/

// todo move all relevant files closer to project files to dedicated diretories

std::string Instructions::geometryInput = R"(C:\Users\petrs\Documents\CTU\BP\FVM_Geometry)";
std::string Instructions::dataInput = R"(C:\Users\petrs\Documents\CTU\BP\FVM_Data\)";
std::string Instructions::outputDir = R"(C:\Users\petrs\Documents\CTU\BP\FVM_Charts\)";

double Instructions::machUB = 5;
double Instructions::machLB = 5;
double Instructions::cpUB = 5;
double Instructions::cpLB = 5;

std::string Instructions::name;
std::string Instructions::verticesName;
std::string Instructions::wallName;
std::string Instructions::reziName;
std::string Instructions::overlayName;

std::string Instructions::outputParaView1 = name + "_1";
std::string Instructions::outputParaView2 = name + "_2";
std::string Instructions::outputRezi = name + "_rezi";
std::string Instructions::outputWall = name + "_wall";

/*--------------------------------------------------------------------------------------------------------------------*/

void Instructions::generateInstructions ()
{
  // creates a file in a folder with the rest of the data
  std::ofstream stream(dataInput + "\\" + "instructions.dat");

  stream << dataInput << std::endl; // 0 - folder with data
  stream << outputDir << std::endl; // 1 - destination for charts and figures

  stream << verticesName << std::endl; // 2 - data for paraview
  stream << wallName << std::endl; // 3 - data for python wall figures
  stream << reziName << std::endl; // 4 - data for residue figure
  stream << overlayName << std::endl; // 5 - overlay for paraview

  stream << outputParaView1 << std::endl; // 6 - paraview zoomed out
  stream << outputParaView2 << std::endl; // 7 - paraview zoomed in
  stream << outputRezi << std::endl; // 8 - residue figure name
  stream << outputWall << std::endl; // 9 - wall figure name

  stream << machUB << std::endl; // 10
  stream << machLB << std::endl; // 11
  stream << cpUB << std::endl; // 12
  stream << cpLB << std::endl; // 13

  stream.close();
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Instructions::createName ()
{
  std::string geometry = Def::isNaca ? "NACA" : "GAMM";
  std::string scheme = Def::isHLLC ? "HLLC" : "HLL";
  std::string order = Def::isSecOrd ? "2nd" : "1st";
  Instructions::name =
          geometry + "___" + scheme + "___" + order + "___" + DataIO::getDate() + "___" + DataIO::getTime();

  Instructions::verticesName = name + "_vertices.csv";
  Instructions::wallName = name + "_wall.dat";
  Instructions::reziName = name + "_rezi.dat";

  Instructions::outputParaView1 = name + "_1";
  Instructions::outputParaView2 = name + "_2";
  Instructions::outputRezi = name + "_rezi";
  Instructions::outputWall = name + "_wall";

  Instructions::overlayName = Def::isNaca ? "only-naca.csv" : "only-gamm.csv";


}

/*--------------------------------------------------------------------------------------------------------------------*/

void Instructions::getMinMaxValues (const std::vector<Point> & updatedPoints)
{
  std::vector<Point> ip = updatedPoints;

  // select all inner points
  ip.erase(std::remove_if(ip.begin(), ip.end(), [] (const Point & p) {
    return p.contributors == 0;
  }), ip.end());

  std::vector<Point>::iterator maxCP;
  std::vector<Point>::iterator minCP;
  std::vector<Point>::iterator maxMach;
  std::vector<Point>::iterator minMach;

  // find the maximum value of mach number in inner points
  maxMach = std::max_element(ip.begin(), ip.end(), [] (const Point & a, const Point & b) {
    return a.values[0] < b.values[0];
  });
  machUB = (*maxMach).values[0];

  // find the minimum value of mach number in inner points
  minMach = std::max_element(ip.begin(), ip.end(), [] (const Point & a, const Point & b) {
    return a.values[0] > b.values[0];
  });
  machLB = (*minMach).values[0];

  // find the maximum value of cp number in inner points
  maxCP = std::max_element(ip.begin(), ip.end(), [] (const Point & a, const Point & b) {
    return a.values[1] < b.values[1];
  });
  cpUB = (*maxCP).values[1];

  // find the minimum value of cp number in inner points
  minCP = std::max_element(ip.begin(), ip.end(), [] (const Point & a, const Point & b) {
    return a.values[1] > b.values[1];
  });
  cpLB = (*minCP).values[1];

  std::cout << "minmax values of mach, then cp:"
            << machLB << ", " << machUB << ", "
            << cpLB << ", " << cpUB << std::endl;

  // see cppreference - algorithm/std::max_element, remove_if
}
