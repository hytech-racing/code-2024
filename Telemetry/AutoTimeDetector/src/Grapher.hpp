#ifndef AUTOTIMEDETECTOR_GRAPHER_HPP
#define AUTOTIMEDETECTOR_GRAPHER_HPP

#include <vector>
#include <string>
#include "Point.hpp"
#include "math.hpp"

class Grapher {
private:
    int cnt = 0;

    std::string get_image_name();

public:
    Grapher();

    void graph(const std::vector<Point>& positions, int begin_idx, math::Circle c, double total_delta);
};


#endif //AUTOTIMEDETECTOR_GRAPHER_HPP
