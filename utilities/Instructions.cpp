//
// Created by petrs on 05.05.2024.
//

#include <fstream>
#include "Instructions.h"
#include "../fluid_dynamics/Def.h"
#include "DataIO.h"

std::string Instructions::geometryInput =   R"(C:\Users\petrs\Documents\CTU\BP\FVM_Geometry)";
std::string Instructions::dataInput =       R"(C:\Users\petrs\Documents\CTU\BP\FVM_Data\)";
std::string Instructions::outputDir =       R"(C:\Users\petrs\Documents\CTU\BP\FVM_Charts\)";

double Instructions::machUB = 5;
double Instructions::machLB = 5;
double Instructions::cpUB = 5;
double Instructions::cpLB = 5;

std::string Instructions::verticesName = "";
std::string Instructions::wallName = "";
std::string Instructions::reziName = "";
std::string Instructions::overlayName = "";

std::string Instructions::outputParaView1 = DataIO::getDate() + "_" + DataIO::getTime() + "_1";
std::string Instructions::outputParaView2 = DataIO::getDate() + "_" + DataIO::getTime() + "_2";
std::string Instructions::outputRezi = DataIO::getDate() + "_" + DataIO::getTime() + "_rezi";
std::string Instructions::outputWall = DataIO::getDate() + "_" + DataIO::getTime() + "_wall";

void Instructions::generateInstructions() {
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

void Instructions::generateBackup() {
    // creates a file in a folder with the rest of the data
    std::ofstream stream(dataInput + "\\" + "instructions_" + DataIO::getTime() + ".dat");

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

void Instructions::createName(bool isNaca, bool isHLLC, bool isSecondOrder) {
  std::string geometry = isNaca ? "NACA" : "GAMM";
  std::string scheme = isHLLC ? "HLLC" : "HLL";
  std::string order = isSecondOrder ? "2nd" : "1st";
  std::string name = geometry + "___" + scheme + "___" + order + "___" + DataIO::getDate() + "___" + DataIO::getTime();

  Instructions::verticesName = name + "_vertices.csv";
  Instructions::wallName = name + "_wall.dat";
  Instructions::reziName = name + "_rezi.dat";
  Instructions::overlayName = Def::isNaca ? "only-naca.csv" : "only-gamm.csv";
}