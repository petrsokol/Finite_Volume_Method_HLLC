//
// Created by petrs on 01.12.2023.
//

#ifndef GAMM_INTERFACE_H
#define GAMM_INTERFACE_H


#include <unordered_map>
#include "Point.h"

// Custom hash function for std::pair<int, int>
struct pair_hash
{
  template<class T1, class T2>
  std::size_t operator() (const std::pair<T1, T2> & p) const
  {
    auto hash1 = std::hash<T1>{}(p.first);
    auto hash2 = std::hash<T2>{}(p.second);

    // Simple hash combining technique
    return hash1 ^ hash2;
  }
};

class Interface
{

public:
  double len, nx, ny;
  int ll, l, r, rr;

  // Constructor
  Interface () = default;

//  Interface (const Point & p1, const Point & p2);

  Interface (double len, double nx, double ny, int ll, int l, int r, int rr);

  // Methods
//  static std::unordered_map<std::pair<int, int>, Interface, pair_hash> createInnerFaces (std::vector<Point> points);

  static std::vector<Interface> createFaces (const std::vector<Point> & points);

  void toString () const;

private:
  static Interface horizontalFace (int k, const Point & a, const Point & b);

  static Interface verticalFace (int k, const Point & a, const Point & c);
};


#endif //GAMM_INTERFACE_H
