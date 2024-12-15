//
// Created by petrs on 02.12.2023.
//

#ifndef GAMM_BOUND_H
#define GAMM_BOUND_H


#include <unordered_map>
#include "../geometry/Cell.h"
#include "../geometry/Interface.h"

class Bound
{

public:
  static double p_infty, rho_infty, u_infty, v_infty;

  static Conservative updateInletCell (const Conservative & innerW);

  static Conservative updateOutletCell (const Conservative & innerW);

  static Conservative updateWallCell (const Conservative & innerW, const Interface & face);

  static void subsonicInlet2ndOrder (Conservative & outer2, Conservative & outer1,
                                     const Conservative & inner1, const Conservative inner2);

  static void supersonicInlet2ndOrder (Conservative & outer2, Conservative & outer1,
                                       const Conservative & inner1, const Conservative inner2);

  static void subsonicOutlet2ndOrder (Conservative & outer2, Conservative & outer1,
                                      const Conservative & inner1, const Conservative inner2);

  static void supersonicOutlet2ndOrder (Conservative & outer2, Conservative & outer1,
                                        const Conservative & inner1, const Conservative & inner2);

  static void wall2ndOrder (const Interface & f, Conservative & outer2, Conservative & outer1,
                            const Conservative & inner1, const Conservative & inner2);

  static void inlet2ndOrder (Conservative & outer2, Conservative & outer1, const Conservative & inner1);

  static void outlet2ndOrder (Conservative & outer2, Conservative & outer1, const Conservative & inner1);

};


#endif //GAMM_BOUND_H
