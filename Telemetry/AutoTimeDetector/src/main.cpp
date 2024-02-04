#include <iostream>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <cmath>
#include "mqtt/async_client.h"
#include "pbPlots.hpp"
#include "supportLib.hpp"
#include "Point.hpp"
#include "math.hpp"
#include "mqtt_constants.hpp"
#include "MqttGpsCallback.hpp"

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

/**
 * Subscribes to MQTT and starts threads to get MQTT data
 */
int main() {
    std::cout << std::setprecision(std::numeric_limits<double>::digits10);
    mqtt::async_client client(mqtt_constants::SERVER_ADDRESS, mqtt_constants::CLIENT_ID);

    std::vector<Point> positions;
    Grapher grapher;
    CircleChecker circleChecker(positions, grapher);
    CoordinateConverter conv;

    MqttGpsCallback mqttCallback(positions, conv, circleChecker);
    client.set_callback(mqttCallback);

    mqtt::connect_options connOpts;

    try {
        client.connect(connOpts)->wait();
        client.subscribe(mqtt_constants::LATITUDE_TOPIC, 1)->wait();
        client.subscribe(mqtt_constants::LONGITUDE_TOPIC, 1)->wait();

        std::thread input_thread(user_input, std::ref(client));
        input_thread.join();
    } catch (const mqtt::exception &exc) {
        std::cerr << exc.what() << std::endl;
        return 1;
    }

    return 0;
}