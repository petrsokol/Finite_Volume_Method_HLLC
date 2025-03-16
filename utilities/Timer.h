//
// Created by petrs on 16.03.2025.
//

#ifndef GAMM_TIMER_H
#define GAMM_TIMER_H


#include <vector>

class Timer
{
public:
  static std::vector<double> reziTimer;
  static std::vector<double> updateCellsTimer;
  static std::vector<double> cellDtTimer;
  static std::vector<double> computeSchemeTimer;
  static std::vector<double> boundsIteratorTimer;
};


#endif //GAMM_TIMER_H
