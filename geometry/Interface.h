//
// Created by petrs on 01.12.2023.
//

#ifndef GAMM_INTERFACE_H
#define GAMM_INTERFACE_H


#include <unordered_map>
#include "Point.h"

// Custom hash function for std::pair<int, int>
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2>& p) const {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);

        // Simple hash combining technique
        return hash1 ^ hash2;
    }
};

class Interface {

public:
    double length, nx, ny;
    int left, right;

    // Constructor
    Interface() = default;

    Interface(Point p1, Point p2);

    // Methods
    static std::unordered_map<std::pair<int, int>, Interface, pair_hash> createInnerFaces(std::vector<Point> points);

    void toString() const;
};


#endif //GAMM_INTERFACE_H
