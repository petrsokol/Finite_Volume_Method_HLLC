//
// Created by petrs on 01.12.2023.
//

#ifndef GAMM_POINT_H
#define GAMM_POINT_H


#include <vector>

class Point {

public:
    double x, y;
    int index;

    // Constructor
    Point(double x, double y, int index);

    // Methods
    static std::vector<Point> createPoints(std::vector<double> bot, std::vector<double> top);

    void toString() const;

    // Overload
    Point operator+(Point other) const;
    Point operator-(Point other) const;
    Point operator*(double scalar) const;
    Point operator/(double scalar) const;
};


#endif //GAMM_POINT_H
