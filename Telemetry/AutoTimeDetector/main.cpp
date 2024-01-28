#include <iostream>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <cmath>
#include "mqtt/async_client.h"
#include "pbPlots.hpp"
#include "supportLib.hpp"

/**
 * MQTT Variables
 */
const std::string SERVER_ADDRESS = "localhost:1883";
const std::string CLIENT_ID = "AutoTimeDetector";
const std::string LATITUDE_TOPIC = "MCU/GPS/latitude";
const std::string LONGITUDE_TOPIC = "MCU/GPS/longitude";

/**
 * Reference gps coordinates that are used as the origin
 */
double reference_latitude = 0;
double reference_longitude = 0;

/**
 * Returns the radian equivalent of an angle in degrees
 */
double deg_to_rad(double deg) {
    return deg * M_PI / 180.0;
}

/**
 * Converts from gps to coordinates to distances relative to the reference
 * Formula from: https://en.wikipedia.org/wiki/Geographic_coordinate_system#Length_of_a_degree
 * @param latitude latitude value in degrees
 * @param longitude longitude value in degrees
 * @return pair with the x and y differences in meters
 */
std::pair<double, double> gps_to_len(double latitude, double longitude) {
    double delta_latitude = latitude - reference_latitude;
    double delta_longitude = longitude - reference_longitude;
    double phi = deg_to_rad((latitude + reference_latitude) / 2.0);
    double y = delta_latitude * (111132.92 - 559.82 * std::cos(phi) + 1.175 *
                                                                      std::cos(2 * phi) - 0.0023 * std::cos(6 * phi));
    double x = delta_longitude * (111412.84 * std::cos(phi) - 93.5 * std::cos(3 * phi) + 0.118 * cos(5 * phi));
    return std::make_pair(x, y);
}

/**
 * Stores position of the car over time
 */
struct Point {
    std::chrono::steady_clock::time_point time;
    double x;
    double y;

    explicit Point(std::pair<double, double> pos) {
        x = pos.first;
        y = pos.second;
        time = std::chrono::high_resolution_clock::now();
    }
};

std::vector<Point> positions;

void debugVec(){
    std::cout << "Debug Vector: " << std::endl;
    for(Point p : positions){
        std::cout << p.x << " " << p.y << std::endl;
    }
    std::cout << "---" << std::endl;
}

/**
 * Implementation of Gauss-Jordan for solving systems of linear equations
 * Verification: https://open.kattis.com/contests/h2jb5c/problems/equationsolver
 * @param a augmented matrix representing system
 * @param ans solution vector
 * @return 0 if no sol, 1 if 1 sol, 2 if inf sol
 */
int gauss_jordan(std::vector<std::vector<double>> a, std::vector<double> &ans) {
    int n = int(a.size());
    int m = int(a[0].size()) - 1;

    std::vector<int> where(m, -1);
    for (int col = 0, row = 0; col < m && row < n; col++) {
        int sel = row;
        for (int i = row; i < n; ++i) {
            if (abs(a[i][col]) > abs(a[sel][col])) {
                sel = i;
            }
        }
        if (abs(a[sel][col]) < 1e-9) {
            continue;
        }
        for (int i = col; i <= m; ++i) {
            std::swap(a[sel][i], a[row][i]);
        }
        where[col] = row;
        for (int i = 0; i < n; ++i) {
            if (i != row) {
                double c = a[i][col] / a[row][col];
                for (int j = col; j <= m; ++j)
                    a[i][j] -= a[row][j] * c;
            }
        }
        row++;
    }

    ans.assign(m, 0);
    for (int i = 0; i < m; ++i) {
        if (where[i] != -1)
            ans[i] = a[where[i]][m] / a[where[i]][i];
    }
    for (int i = 0; i < n; ++i) {
        double sum = 0;
        for (int j = 0; j < m; ++j) {
            sum += ans[j] * a[i][j];
        }
        if (abs(sum - a[i][m]) > 1e-9) {
            return 0;
        }
    }

    for (int i = 0; i < m; ++i) {
        if (where[i] == -1)
            return 2;
    }
    return 1;
}

struct Circle {
    double x;
    double y;
    double radius;
};


int num = 0;
std::string getImageName(int n, int begin_idx){
//    std::string n_str = std::to_string(n);
//    while(int(n_str.size()) < 3) n_str = "0" + n_str;
//    std::string idx_str = std::to_string(begin_idx);
//    while(int(idx_str.size()) < 3) idx_str = "0" + idx_str;
//    return "../graphs/graph_" + n_str + "_" + idx_str + ".png";
    std::string n_str = std::to_string(num++);
    while(int(n_str.size()) < 3) n_str = "0" + n_str;
    return "../graphs/graph_" + n_str + ".png";
}

void create_graph(int n, int begin_idx, Circle c, double total_delta){
    bool success;
    StringReference *errorMessage = CreateStringReferenceLengthValue(0, L' ');
    RGBABitmapImageReference *imageReference = CreateRGBABitmapImageReference();

    std::vector<double> computed_xs;
    std::vector<double> computed_ys;
    std::vector<double> other_xs;
    std::vector<double> other_ys;

    for(int i = 0; i < int(positions.size()); i++){
        if(i >= begin_idx){
            computed_xs.push_back(positions[i].x);
            computed_ys.push_back(positions[i].y);
        }else{
            other_xs.push_back(positions[i].x);
            other_ys.push_back(positions[i].y);
        }
    }

    ScatterPlotSeries *series = GetDefaultScatterPlotSeriesSettings();
    series->xs = &computed_xs;
    series->ys = &computed_ys;
    series->linearInterpolation = false;
    series->pointType = toVector(L"dots");
    series->color = CreateRGBColor(1, 0, 0);

    ScatterPlotSeries *series2 = GetDefaultScatterPlotSeriesSettings();
    series2->xs = &other_xs;
    series2->ys = &other_ys;
    series2->linearInterpolation = false;
    series2->pointType = toVector(L"dots");
    series2->lineThickness = 2;
    series2->color = CreateRGBColor(0, 0, 1);

    std::vector<double> circle_x;
    std::vector<double> circle_y;

    for(double i = 0; i < 2*M_PI+0.1; i+=0.1){
        circle_x.push_back(c.x + c.radius*std::cos(i));
        circle_y.push_back(c.y + c.radius*std::sin(i));
    }

    ScatterPlotSeries *series3 = GetDefaultScatterPlotSeriesSettings();
    series3->xs = &circle_x;
    series3->ys = &circle_y;
    series3->linearInterpolation = true;
    series3->lineType = toVector(L"solid");
    series3->color = CreateRGBColor(1, 0, 0);

    std::string title = std::to_string(total_delta);
    std::wstring wtitle = std::wstring(title.begin(), title.end());

    ScatterPlotSettings *settings = GetDefaultScatterPlotSettings();
    settings->width = 600;
    settings->height = 600;
    settings->autoBoundaries = true;
    settings->autoPadding = true;
    settings->title = toVector(wtitle.c_str());
    settings->xLabel = toVector(L"");
    settings->yLabel = toVector(L"");
    if(!computed_xs.empty()) settings->scatterPlotSeries->push_back(series);
    if(!other_xs.empty()) settings->scatterPlotSeries->push_back(series2);
    settings->scatterPlotSeries->push_back(series3);

    success = DrawScatterPlotFromSettings(imageReference, settings, errorMessage);

    if(success){
        std::vector<double> *pngdata = ConvertToPNG(imageReference->image);
        WriteToFile(pngdata, getImageName(n, begin_idx));
        DeleteImage(imageReference->image);
    }else{
        std::cerr << "Error: ";
        for(wchar_t c : *errorMessage->string){
            std::cerr << c;
        }
        std::cerr << std::endl;
    }
}

/**
 * Computes the circle that best fits the set of points from begin_index to the end
 * of positions
 * Algorithm from: https://dtcenter.org/sites/default/files/community-code/met/docs/write-ups/circle_fit.pdf
 * @param begin_index first point to include in circle fit
 * @return the best fit circle
 */
Circle least_squares_circle_fit(int begin_index) {
    double x_bar = 0;
    double y_bar = 0;
    int N = int(positions.size()) - begin_index;
    for (int i = begin_index; i < positions.size(); i++) {
        x_bar += positions[i].x;
        y_bar += positions[i].y;
    }
    x_bar /= N;
    y_bar /= N;
    std::vector<double> u;
    std::vector<double> v;
    for (int i = begin_index; i < positions.size(); i++) {
        u.push_back(positions[i].x - x_bar);
        v.push_back(positions[i].y - y_bar);
    }
    double S_uu = 0;
    double S_uv = 0;
    double S_vv = 0;
    double S_uuu = 0;
    double S_uvv = 0;
    double S_vvv = 0;
    double S_vuu = 0;
    for (int i = 0; i < N; i++) {
        S_uu += u[i] * u[i];
        S_uv += u[i] * v[i];
        S_vv += v[i] * v[i];
        S_uuu += u[i] * u[i] * u[i];
        S_uvv += u[i] * v[i] * v[i];
        S_vvv += v[i] * v[i] * v[i];
        S_vuu += v[i] * u[i] * u[i];
    }

    std::vector<std::vector<double>> augmented_matrix;
    augmented_matrix = {{S_uu, S_uv, 0.5 * (S_uuu + S_uvv)},
                        {S_uv, S_vv, 0.5 * (S_vvv + S_vuu)}};
    std::vector<double> ans;
    int res = gauss_jordan(augmented_matrix, ans);
    double u_c = ans[0];
    double v_c = ans[1];

    double alpha = u_c * u_c + v_c * v_c + (S_uu + S_vv) / N;
    double r = std::sqrt(alpha);
    return {u_c + x_bar, v_c + y_bar, r};
}

/**
 * Computes the angle between two points in a circle
 * @param circle circle to compute angle around
 * @param p1 initial point in the circle
 * @param p2 final point in the circle
 * @return angle between final and initial point in radius
 */
double delta_theta(Circle circle, int p1, int p2){
    double theta1 = std::atan2(positions[p1].y - circle.y, positions[p1].x - circle.x);
    double theta2 = std::atan2(positions[p2].y - circle.y, positions[p2].x - circle.x);

    theta1 = fmod(theta1 + 2*M_PI, 2*M_PI);
    theta2 = fmod(theta2 + 2*M_PI, 2*M_PI);
    if(theta2 < theta1) theta2 += 2*M_PI;

    double diff = theta2 - theta1;
    return (diff > M_PI) ? diff - M_PI : diff;
}

/**
 * Parameters for circle check
 */
const int MIN_CHECK = 5;
const double CIRCLE_RADIUS = 7.5;
const double MAX_RADIUS_ERROR = 2;

void circle_check() {
    if (positions.size() < MIN_CHECK) {
        return;
    }
    for (int begin_idx = int(positions.size()) - MIN_CHECK; begin_idx >= 0; begin_idx--) {

        auto initialTime = positions[0].time;
        auto currentTime = positions[positions.size()-1].time;

        Circle circle = least_squares_circle_fit(begin_idx);

//        std::cout << circle.radius << std::endl;
        if(circle.radius < CIRCLE_RADIUS - MAX_RADIUS_ERROR || circle.radius > CIRCLE_RADIUS + MAX_RADIUS_ERROR){
            create_graph(int(positions.size()), begin_idx, circle, 0);
            break;
        }
        double total_delta = 0;
        for(int i = begin_idx; i+1 < int(positions.size()); i++){
            double dt = delta_theta(circle, i, i+1);
            total_delta += dt;
        }
        create_graph(int(positions.size()), begin_idx, circle, total_delta);
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
//    std::cout << "---" << std::endl;
}

/**
 * Stores values for the message_arrived method
 */
double last_latitude = 0;
double last_longitude = 0;
bool seen_non_zero = false;
bool reference_set = false;

class callback : public virtual mqtt::callback {
public:
    /**
     * Method to handle arrival of MQTT messages
     */
    void message_arrived(mqtt::const_message_ptr msg) override {
//        std::cout << "message" << std::endl;
        double val = std::stod(msg->to_string());
        if (msg->get_topic() == LATITUDE_TOPIC) {
            last_latitude = val;
            if (val != 0) {
                seen_non_zero = true;
            }
        } else if (msg->get_topic() == LONGITUDE_TOPIC) {
            last_longitude = val;
            //add to list and compute time
            if (seen_non_zero) {
                if (!reference_set) {
                    reference_latitude = last_latitude;
                    reference_longitude = last_longitude;
                    reference_set = true;
                }
//                std::cout << last_latitude << " " << last_longitude;
                positions.emplace_back(gps_to_len(last_latitude, last_longitude));
                circle_check();
            }
        }
    }
};

void user_input(mqtt::async_client &client) {
    std::string input;
    while (true) {
        std::getline(std::cin, input);
        if (input == "q") {
            client.disconnect()->wait();
            break;
        }
    }
}



void test_main() {
    positions.emplace_back(std::make_pair(194.7882997, 154.5037281));
    positions.emplace_back(std::make_pair(195.5106788, 156.4072194));
    positions.emplace_back(std::make_pair(195.7699795, 158.5320469));
    positions.emplace_back(std::make_pair(195.5106347, 160.7011417));
    positions.emplace_back(std::make_pair(194.7789528, 162.8481028));
    positions.emplace_back(std::make_pair(193.6027209, 164.7515941));
    positions.emplace_back(std::make_pair(192.0097241, 166.3452147));
    positions.emplace_back(std::make_pair(190.0925792, 167.4850962));
    positions.emplace_back(std::make_pair(188.0272553, 168.1159043));
    positions.emplace_back(std::make_pair(185.9341519, 168.2487061));
    positions.emplace_back(std::make_pair(183.9244072, 167.8503009));
    positions.emplace_back(std::make_pair(182.0998977, 166.9317557));
    positions.emplace_back(std::make_pair(180.6365919, 165.515204));
    positions.emplace_back(std::make_pair(179.4789186, 163.8109153));
    positions.emplace_back(std::make_pair(178.6176152, 161.9295576));
    positions.emplace_back(std::make_pair(178.1823429, 159.9043313));
    positions.emplace_back(std::make_pair(178.1823623, 157.8237711));
    positions.emplace_back(std::make_pair(178.6547191, 155.7653444));
    positions.emplace_back(std::make_pair(179.6735048, 153.8065191));
    positions.emplace_back(std::make_pair(181.1553631, 152.2350321));
    positions.emplace_back(std::make_pair(183.01694, 151.0840838));
    positions.emplace_back(std::make_pair(185.1563579, 150.4422089));
    positions.emplace_back(std::make_pair(187.4254314, 150.3868748));
    positions.emplace_back(std::make_pair(189.7408074, 150.8516808));
    positions.emplace_back(std::make_pair(191.8894703, 151.8698273));
    positions.emplace_back(std::make_pair(193.6954514, 153.4081139));
    positions.emplace_back(std::make_pair(195.084661, 155.2230707));
    positions.emplace_back(std::make_pair(195.9830067, 157.3368314));
    positions.emplace_back(std::make_pair(196.2237833, 159.5612602));
    positions.emplace_back(std::make_pair(195.9088696, 161.7082214));
    positions.emplace_back(std::make_pair(195.0290043, 163.7777148));
    positions.emplace_back(std::make_pair(193.6768047, 165.5705381));
}

/**
 * Subscribes to MQTT and starts threads to get MQTT data
 */
int main() {
//    test_main();
//    circle_check();
    std::cout << std::setprecision(std::numeric_limits<double>::digits10);
    mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);

    callback cb;
    client.set_callback(cb);

    mqtt::connect_options connOpts;

    try {
        client.connect(connOpts)->wait();
        client.subscribe(LATITUDE_TOPIC, 1)->wait();
        client.subscribe(LONGITUDE_TOPIC, 1)->wait();

        std::thread input_thread(user_input, std::ref(client));
        input_thread.join();
    } catch (const mqtt::exception &exc) {
        std::cerr << exc.what() << std::endl;
        return 1;
    }

    return 0;
return 0;
}