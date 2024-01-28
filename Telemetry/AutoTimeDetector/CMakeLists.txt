cmake_minimum_required(VERSION 3.26)
project(AutoTimeDetector)

set(CMAKE_CXX_STANDARD 11)

find_package(PahoMqttCpp REQUIRED)

add_executable(AutoTimeDetector main.cpp
        pbPlots.cpp
        pbPlots.hpp
        supportLib.cpp
        supportLib.hpp)

target_link_libraries(AutoTimeDetector PahoMqttCpp::paho-mqttpp3)