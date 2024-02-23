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

    Grapher grapher;

public:

    CircleChecker(Grapher &grapher);

    void circle_check(std::vector<Point> &positions);

};


#endif //AUTOTIMEDETECTOR_CIRCLECHECKER_HPP
