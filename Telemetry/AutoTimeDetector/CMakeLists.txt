cmake_minimum_required(VERSION 3.26)
project(AutoTimeDetector)

set(CMAKE_CXX_STANDARD 11)

find_package(PahoMqttCpp REQUIRED)

add_executable(AutoTimeDetector src/main.cpp
        src/pbPlots.cpp
        src/pbPlots.hpp
        src/supportLib.cpp
        src/supportLib.hpp
        src/MqttGpsCallback.hpp
        src/MqttGpsCallback.cpp
        src/Point.hpp
        src/mqtt_constants.hpp
        src/CoordinateConverter.cpp
        src/CoordinateConverter.hpp
        src/math.hpp
        src/CircleChecker.cpp
        src/CircleChecker.hpp
        src/Grapher.cpp
        src/Grapher.hpp
        src/math.cpp)

target_link_libraries(AutoTimeDetector PahoMqttCpp::paho-mqttpp3)