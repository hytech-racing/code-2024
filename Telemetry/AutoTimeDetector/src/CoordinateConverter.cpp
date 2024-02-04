#include "CoordinateConverter.hpp"

#include "math.hpp"

std::pair<double, double> CoordinateConverter::gps_to_len(double latitude, double longitude) {
    double delta_latitude = latitude - reference_latitude;
    double delta_longitude = longitude - reference_longitude;
    double phi = math::deg_to_rad((latitude + reference_latitude) / 2.0);
    double y = delta_latitude * (111132.92 - 559.82 * std::cos(phi) + 1.175 *
                                                                      std::cos(2 * phi) - 0.0023 * std::cos(6 * phi));
    double x = delta_longitude * (111412.84 * std::cos(phi) - 93.5 * std::cos(3 * phi) + 0.118 * cos(5 * phi));
    return std::make_pair(x, y);
}

double CoordinateConverter::getReferenceLatitude() const {
    return reference_latitude;
}

double CoordinateConverter::getReferenceLongitude() const {
    return reference_longitude;
}

void CoordinateConverter::setReferenceLatitude(double referenceLatitude) {
    reference_latitude = referenceLatitude;
}

void CoordinateConverter::setReferenceLongitude(double referenceLongitude) {
    reference_longitude = referenceLongitude;
}
