cmake_minimum_required(VERSION 3.17)
project(477_hw1)

set(CMAKE_CXX_STANDARD 14)

include_directories("Code Template")

add_executable(477_hw1
        "Code Template/parser.cpp"
        "Code Template/parser.h"
        "Code Template/ppm.cpp"
        "Code Template/ppm.h"
        "Code Template/raytracer.cpp"
        "Code Template/simple.xml"
        "Code Template/test.ppm"
        "Code Template/tinyxml2.cpp"
        "Code Template/tinyxml2.h"
        "Sample Scenes/Input Files/bunny.xml"
        "Sample Scenes/Input Files/cornellbox.xml"
        "Sample Scenes/Input Files/horse_and_mug.xml"
        "Sample Scenes/Input Files/killeroo.xml"
        "Sample Scenes/Input Files/mirror_spheres.xml"
        "Sample Scenes/Input Files/simple.xml"
        "Sample Scenes/Input Files/simple_reflectance.xml"
        "Sample Scenes/Input Files/simple_shading.xml"
        "Sample Scenes/Outputs/bunny.ppm"
        "Sample Scenes/Outputs/cornellbox_front.ppm"
        "Sample Scenes/Outputs/cornellbox_inverse.ppm"
        "Sample Scenes/Outputs/cornellbox_top.ppm"
        "Sample Scenes/Outputs/horse_and_mug.ppm"
        "Sample Scenes/Outputs/killeroo.ppm"
        "Sample Scenes/Outputs/mirror_spheres.ppm"
        "Sample Scenes/Outputs/simple.ppm"
        "Sample Scenes/Outputs/simple_reflectance.ppm"
        "Sample Scenes/Outputs/simple_shading.ppm")
