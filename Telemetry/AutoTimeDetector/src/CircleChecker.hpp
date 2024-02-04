#ifndef AUTOTIMEDETECTOR_CIRCLECHECKER_HPP
#define AUTOTIMEDETECTOR_CIRCLECHECKER_HPP

#include <vector>
#include "Point.hpp"
#include "Grapher.hpp"

class CircleChecker {

private:
    const int MIN_CHECK = 5;
    const double CIRCLE_RADIUS = 7.5;
    const double MAX_RADIUS_ERROR = 2;

    std::vector<Point> positions;

    Grapher grapher;

public:

    CircleChecker(std::vector<Point> &positions, Grapher &grapher);

    void circle_check();

};


#endif //AUTOTIMEDETECTOR_CIRCLECHECKER_HPP
