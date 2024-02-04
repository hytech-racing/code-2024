#ifndef AUTOTIMEDETECTOR_MQTTGPSCALLBACK_HPP
#define AUTOTIMEDETECTOR_MQTTGPSCALLBACK_HPP

#include "mqtt/callback.h"
#include "Point.hpp"
#include "CoordinateConverter.hpp"
#include "CircleChecker.hpp"

/**
 * Callback for mqtt client that handles GPS data
 */
class MqttGpsCallback : public virtual mqtt::callback{

private:

    std::vector<Point> &positions;
    CoordinateConverter &conv;
    CircleChecker &circleChecker;

    double last_latitude = 0;
    double last_longitude = 0;
    bool seen_non_zero = false;
    bool reference_set = false;

public:
    MqttGpsCallback(std::vector<Point> &positions, CoordinateConverter &conv, CircleChecker &circleChecker);

    void message_arrived(mqtt::const_message_ptr msg) override;

};


#endif //AUTOTIMEDETECTOR_MQTTGPSCALLBACK_HPP
