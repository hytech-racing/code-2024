#ifndef AUTOTIMEDETECTOR_POINT_HPP
#define AUTOTIMEDETECTOR_POINT_HPP

#include <chrono>

/**
 * Stores position of the car with a timestamp
 */
struct Point {
    std::chrono::high_resolution_clock::time_point time;
    double x;
    double y;

    explicit Point(std::pair<double, double> pos) {
        x = pos.first;
        y = pos.second;
        time = std::chrono::high_resolution_clock::now();
    }
};

#endif //AUTOTIMEDETECTOR_POINT_HPP
