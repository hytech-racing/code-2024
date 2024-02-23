#include "CircleChecker.hpp"
#include "math.hpp"
#include <iostream>

CircleChecker::CircleChecker(Grapher &grapher) : grapher(grapher) {}

void CircleChecker::circle_check(std::vector<Point> &positions) {
    if (positions.size() < MIN_CHECK) {
        return;
    }
    for (int begin_idx = int(positions.size()) - MIN_CHECK; begin_idx >= 0; begin_idx--) {

        auto initialTime = positions[0].time;
        auto currentTime = positions[positions.size()-1].time;

        math::Circle circle = math::least_squares_circle_fit(positions, begin_idx);

        if(circle.radius < CIRCLE_RADIUS - MAX_RADIUS_ERROR || circle.radius > CIRCLE_RADIUS + MAX_RADIUS_ERROR){
//            grapher.graph(positions, begin_idx, circle, 0);
            break;
        }
        double total_delta = 0;
        for(int i = begin_idx; i+1 < int(positions.size()); i++){
            double dt = math::delta_theta(circle, positions[i], positions[i+1]);
            total_delta += dt;
        }
//        grapher.graph(positions, begin_idx, circle, total_delta);
        if(total_delta >= 2*M_PI || total_delta <= -2*M_PI){
            //circle completed
            Point final_pos = positions.back();
            auto lap_time = final_pos.time - positions[begin_idx].time;
            auto lap_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(lap_time);
            std::cout << "Lap completed. Time: " << lap_time_ms.count() << "ms" << std::endl;

            positions.clear();
            positions.push_back(final_pos);
        }
    }
}
