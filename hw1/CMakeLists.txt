cmake_minimum_required(VERSION 3.17)
project(hw1)

set(CMAKE_CXX_STANDARD 14)

include_directories(RayTracer)

add_executable(hw1
        RayTracer/parser.cpp
        RayTracer/parser.h
        RayTracer/ppm.cpp
        RayTracer/ppm.h
        RayTracer/raytracer.cpp
        RayTracer/simple.xml
        RayTracer/tinyxml2.cpp
        RayTracer/tinyxml2.h
        RayTracer/vectorOperations.cpp)
