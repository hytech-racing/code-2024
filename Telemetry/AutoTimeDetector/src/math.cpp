#include "math.hpp"

double math::deg_to_rad(double deg) {
    return deg * M_PI / 180.0;
}

int math::gauss_jordan(std::vector<std::vector<double>> a, std::vector<double> &ans) {
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

math::Circle math::least_squares_circle_fit(std::vector<Point> &positions, int begin_index) {
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

//double math::delta_theta(math::Circle circle, Point p1, Point p2){
//    double theta1 = std::atan2(p1.y - circle.y, p1.x - circle.x);
//    double theta2 = std::atan2(p2.y - circle.y, p2.x - circle.x);
//
//    theta1 = fmod(theta1 + 2*M_PI, 2*M_PI);
//    theta2 = fmod(theta2 + 2*M_PI, 2*M_PI);
//    if(theta2 < theta1) theta2 += 2*M_PI;
//
//    double diff = theta2 - theta1;
//    return (diff > M_PI) ? diff - M_PI : diff;
//}

double math::delta_theta(math::Circle circle, Point p1, Point p2) {
    double estimate_s = std::sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));
    double theta = estimate_s/circle.radius;
    return theta;
}