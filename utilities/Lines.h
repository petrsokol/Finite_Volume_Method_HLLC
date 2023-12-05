//
// Created by petrs on 05.11.2023.
//

#ifndef MESHGENERATOR_LINES_H
#define MESHGENERATOR_LINES_H

#include <vector>

class Lines {

private:
    static int bound(double l);

public:
    static void arc(std::vector<double> x, std::vector<double> &y, double l1, double l2, double d);
    static void line(std::vector<double> &y, double l1, double l2, double yValue);

};


#endif //MESHGENERATOR_LINES_H
