cmake_minimum_required(VERSION 3.26)
project(HyTech_C_To_Py)

set(CMAKE_CXX_STANDARD 20)

add_executable(HyTech_C_To_Py main.cpp
        Field.cpp
        Field.hpp
        Message.cpp
        Message.hpp
        Field.cpp
        Field.hpp
        Signal.cpp
        Signal.hpp
        StringManip.cpp
        StringManip.hpp)
