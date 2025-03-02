//
// Created by petrs on 05.05.2024.
//

#ifndef GAMM_INSTRUCTIONS_H
#define GAMM_INSTRUCTIONS_H


#include <string>

class Instructions
{

public:
  // where the data is stored
  static std::string geometryInput;
  static std::string dataInput;

  // where the charts are to be saved
  static std::string outputDir;

  // input data file names
  static std::string verticesName;
  static std::string wallName;
  static std::string reziName;

  // output file names
  static std::string overlayName;
  static std::string outputParaView1;
  static std::string outputParaView2;
  static std::string outputRezi;
  static std::string outputWall;

  // value bounds for paraview visualisations
  static double machLB;
  static double machUB;
  static double cpLB;
  static double cpUB;

  // methods
  static void createName (bool isNaca, bool isHLLC, bool isSecondOrder);

  static void generateInstructions ();

};


#endif //GAMM_INSTRUCTIONS_H
