cmake_minimum_required(VERSION 3.15)
project(ComputerGeometry-Graphics)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")

set(SOURCE_FILES LAB2/main.cpp)
add_executable(ComputerGeometry-Graphics ${SOURCE_FILES})