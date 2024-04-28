//
// Created by petrs on 01.12.2023.
//

#ifndef GAMM_POINT_H
#define GAMM_POINT_H


#include <vector>
#include <string>

class Point {

public:
    static const int valueCount;
    double x, y;
    std::vector<double> values;
    int index, contributors;

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

    static std::vector<Point> loadPointsFromFile(const std::string &dir, const std::string &file);
};


#endif //GAMM_POINT_H
