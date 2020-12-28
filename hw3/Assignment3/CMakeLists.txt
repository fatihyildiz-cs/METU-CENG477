cmake_minimum_required(VERSION 3.17)
project(Assignment3)

set(CMAKE_CXX_STANDARD 14)

include_directories(Source)

add_executable(Assignment3
        Samples/Outputs/horse.png
        Samples/Outputs/horse_and_mug.png
        Samples/Outputs/horse_and_mug_backface_culled.png
        Samples/Outputs/horse_and_mug_frontface_culled.png
        Samples/Outputs/horse_and_mug_reversed.png
        Samples/Outputs/horse_and_mug_reversed_frontface_culled.png
        Samples/horse.xml
        Samples/horse_and_mug.xml
        Samples/horse_and_mug_backface_culled.xml
        Samples/horse_and_mug_frontface_culled.xml
        Samples/horse_and_mug_reversed.xml
        Samples/horse_and_mug_reversed_frontface_culled.xml
        Source/main.cpp
        Source/parser.cpp
        Source/parser.h
        Source/tinyxml2.cpp
        Source/tinyxml2.h
        Makefile)
