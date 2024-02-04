#ifndef AUTOTIMEDETECTOR_MQTT_CONSTANTS_HPP
#define AUTOTIMEDETECTOR_MQTT_CONSTANTS_HPP

#include <string>

namespace mqtt_constants {

    const std::string SERVER_ADDRESS = "localhost:1883";
    const std::string CLIENT_ID = "AutoTimeDetector";
    const std::string LATITUDE_TOPIC = "MCU/GPS/latitude";
    const std::string LONGITUDE_TOPIC = "MCU/GPS/longitude";

}

#endif //AUTOTIMEDETECTOR_MQTT_CONSTANTS_HPP
