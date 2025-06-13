//
// Created by petrs on 01.12.2023.
//

#include "Interface.h"
#include <cmath>
#include <iostream>


/*--------------------------------------------------------------------------------------------------------------------*/

Interface::Interface (Line line, int ll, int l, int r, int rr, const Point & p1, const Point & p2, double area,
                      Line BR, Line RT, Line TL, Line LB)
        : line(line), ll(ll), l(l), r(r), rr(rr), p1(p1), p2(p2), dualArea(area),
          BR(BR), RT(RT), TL(TL), LB(LB)
{
}

/*--------------------------------------------------------------------------------------------------------------------*/

// used for dummy interfaces outside the computational domain - a workaround
Interface::Interface (Line line, int ll, int l, int r, int rr, const Point & p1, const Point & p2)
        : Interface(line, ll, l, r, rr, p1, p2, 0, line, line, line, line)
{
}

/*--------------------------------------------------------------------------------------------------------------------*/

/* point distribution:
 * interfaces created in each cycle are displayed with full line ( | or ___ )
 *
 * LT ... T ... RT
 *  :     |     :
 *  L ... C ___ R
 *  :     :     :
 *  - ... B ... RB
 */
std::vector<Interface> Interface::createFaces (const std::vector<Point> & points, const MeshParams & mp)
{
  std::vector<Interface> res = {};
  for (int j = 0; j < mp.Y_CELLS; ++j) {
    for (int i = 0; i < mp.X_CELLS; ++i) {
      int k = i + j * mp.X_POINTS;
      int faceIndex = i + j * mp.X_CELLS;

      /* the algorithm first attempts to create an interface using all six points,
       * but interfaces near the edge do not have these neighboring points;
       * in this case it will default to create an interface with an dualArea of 0
       */

      // zero-dualArea interface
      if (k - mp.X_POINTS < 0 || i == 0) {
        const Point & T = points.at(k + mp.X_POINTS);
        const Point & C = points.at(k);
        const Point & R = points.at(k + 1);

        res.emplace_back(constructVertical(faceIndex, C, T));
        res.emplace_back(constructHorizontal(faceIndex, C, R, mp));
      }
        // proper interface
      else {
        const Point & LT = points.at(k + mp.X_POINTS - 1);
        const Point & T = points.at(k + mp.X_POINTS);
        const Point & RT = points.at(k + mp.X_POINTS + 1);
        const Point & L = points.at(k - 1);
        const Point & C = points.at(k);
        const Point & R = points.at(k + 1);
        const Point & B = points.at(k - mp.X_POINTS);
        const Point & RB = points.at(k - mp.X_POINTS + 1);

        res.emplace_back(constructVerticalFromSixPoints(faceIndex, mp, L, C, R, RT, T, LT));
        res.emplace_back(constructHorizontalFromSixPoints(faceIndex, mp, B, RB, R, RT, T, C));
      }
    }
  }
  std::cout << "Loaded " << res.size() << "faces." << std::endl;
  return res;
}

/*--------------------------------------------------------------------------------------------------------------------*/

/* point distribution:
 * * ... *
 * :  RR :
 * * --- *
 * :  R  :
 * k --- *
 * :  L  :
 * * --- *
 * :  LL :
 * * ... *
 */
Interface Interface::constructHorizontal (int k, const Point & A, const Point & B, const MeshParams & mp)
{
  int ll = k - 2 * mp.X_CELLS;
  int l = k - mp.X_CELLS;
  int r = k;
  int rr = k + mp.X_CELLS;

  // !!!
  // REVERSED ORDER: for correct normal direction
  // !!!
  Line line(B, A);

  // construct result
  // un-reversed order of points!
  return {line, ll, l, r, rr, A, B};
}

/*--------------------------------------------------------------------------------------------------------------------*/

/* point distribution:
 * * --- * --- * --- * --- *
 * :  LL :  L  |  R  :  RR :
 * * --- * --- k --- * --- *
 */
Interface Interface::constructVertical (int k, const Point & A, const Point & D)
{
  int ll = k - 2;
  int l = k - 1;
  int r = k;
  int rr = k + 1;

  Line line(A, D);

  return {line, ll, l, r, rr, A, D};
}

/*--------------------------------------------------------------------------------------------------------------------*/

const double & Interface::len () const
{
  return line.len;
}

/*--------------------------------------------------------------------------------------------------------------------*/

const double & Interface::nx () const
{
  return line.nx;
}

/*--------------------------------------------------------------------------------------------------------------------*/

const double & Interface::ny () const
{
  return line.ny;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void Interface::toString () const
{
  printf("face between cells [ %d : %d : %d : %d ], len = %f, normal vector (%f, %f)\n", ll, l, r, rr, len(), nx(),
         ny());
}

/*--------------------------------------------------------------------------------------------------------------------*/

/* point distribution:
 * E ... D
 * :  R  :
 * F --- C
 * :  L  :
 * A ... B
 */
Interface Interface::constructHorizontalFromSixPoints (int k, const MeshParams & mp,
                                                       const Point & A, const Point & B, const Point & C,
                                                       const Point & D, const Point & E, const Point & F)
{
  // reversed order for correct normals
  Line line(C, F);

  Point R = Point::centroidQuadrilateral(E, F, C, D);
  Point L = Point::centroidQuadrilateral(A, B, C, F);

  double dualArea = Point::areaQuadrilateral(F, L, C, R);

  int ll = k - 2 * mp.X_CELLS;
  int l = k - mp.X_CELLS;
  int r = k;
  int rr = k + mp.X_CELLS;

  Line BR(F, L);
  Line RT(L, C);
  Line TL(C, R);
  Line LB(R, F);

  return {line, ll, l, r, rr, F, C, dualArea, BR, RT, TL, LB};
}

/*--------------------------------------------------------------------------------------------------------------------*/

/* point distribution:
 * F --- E --- D
 * :  L  |  R  :
 * A --- B --- C
 */
Interface Interface::constructVerticalFromSixPoints (int k, const MeshParams & mp,
                                                     const Point & A, const Point & B, const Point & C,
                                                     const Point & D, const Point & E, const Point & F)
{
  Line line(B, E);

  Point L = Point::centroidQuadrilateral(A, B, E, F);
  Point R = Point::centroidQuadrilateral(B, C, D, E);

  double dualArea = Point::areaQuadrilateral(L, B, R, E);

  int ll = k - 2;
  int l = k - 1;
  int r = k;
  int rr = k + 1;

  Line BR(B, R);
  Line RT(R, E);
  Line TL(E, L);
  Line LB(L, B);

  return {line, ll, l, r, rr, B, E, dualArea, BR, RT, TL, LB};
}

/*--------------------------------------------------------------------------------------------------------------------*/

std::ostream & operator << (std::ostream & os, const Interface & f)
{
  // interface between two points (list of neighbours),
  os << "(" << f.p1 << "---" << f.p2 << ")";
  return os;
}

/*--------------------------------------------------------------------------------------------------------------------*/
