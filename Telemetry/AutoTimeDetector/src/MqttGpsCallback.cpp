#include "MqttGpsCallback.hpp"
#include "mqtt_constants.hpp"


void MqttGpsCallback::message_arrived(mqtt::const_message_ptr msg) {
    double val = std::stod(msg->to_string());
    if (msg->get_topic() == mqtt_constants::LATITUDE_TOPIC) {
        last_latitude = val;
        if (val != 0) {
            seen_non_zero = true;
        }
    } else if (msg->get_topic() == mqtt_constants::LONGITUDE_TOPIC) {
        last_longitude = val;
        if (seen_non_zero) {
            if (!reference_set) {
                conv.setReferenceLatitude(last_latitude);
                conv.setReferenceLongitude(last_longitude);
                reference_set = true;
            }
            positions.emplace_back(conv.gps_to_len(last_latitude, last_longitude));
            circleChecker.circle_check(positions);
        }
    }
}

MqttGpsCallback::MqttGpsCallback(std::vector<Point> &positions, CoordinateConverter &conv, CircleChecker &circleChecker)
        : positions(positions), conv(conv), circleChecker(circleChecker) {}
