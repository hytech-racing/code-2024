#ifndef AUTOTIMEDETECTOR_MATH_HPP
#define AUTOTIMEDETECTOR_MATH_HPP

#include <cmath>
#include <vector>
#include "Point.hpp"

namespace math {

    struct Circle {
        double x;
        double y;
        double radius;
    };

    /**
     * Returns the radian equivalent of an angle in degrees
     */
    double deg_to_rad(double deg);

    /**
     * Implementation of Gauss-Jordan for solving systems of linear equations
     * Verification: https://open.kattis.com/contests/h2jb5c/problems/equationsolver
     * @param a augmented matrix representing system
     * @param ans solution vector
     * @return 0 if no sol, 1 if 1 sol, 2 if inf sol
     */
    int gauss_jordan(std::vector<std::vector<double>> a, std::vector<double> &ans);

    /**
     * Computes the circle that best fits the set of points from begin_index to the end
     * of positions
     * Algorithm from: https://dtcenter.org/sites/default/files/community-code/met/docs/write-ups/circle_fit.pdf
     * @param begin_index first point to include in circle fit
     * @return the best fit circle
     */
    Circle least_squares_circle_fit(std::vector<Point> &positions, int begin_index);

    /**
     * Computes the angle between two points in a circle
     * @param circle circle to compute angle around
     * @param p1 initial point in the circle
     * @param p2 final point in the circle
     * @return angle between final and initial point in radius
     */
    double delta_theta(Circle circle, Point p1, Point p2);
}

#endif //AUTOTIMEDETECTOR_MATH_HPP
