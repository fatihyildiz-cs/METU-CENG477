cmake_minimum_required(VERSION 3.17)
project(RayTracer)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(RayTracer
        parser.cpp
        parser.h
        ppm.cpp
        ppm.h
        raytracer.cpp
        simple.xml
        tinyxml2.cpp
        tinyxml2.h
        vectorOperations.cpp)
