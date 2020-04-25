cmake_minimum_required(VERSION 3.15)
project(ComputerGeometry-Graphics)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")

set(SOURCE_FILES LAB2_FULL/main.cpp)
set(L2_lib LAB2_renovate/CImageFileOpenException.cpp LAB2_renovate/CImage.cpp LAB2_renovate/CImageMemAllocException.cpp LAB2_renovate/CImageException.cpp LAB2_renovate/CImageFileDeleteException.cpp LAB2_renovate/CImageParamsException.cpp LAB2_renovate/CImageFileReadException.cpp LAB2_renovate/CImageFileFormatException.cpp)

add_executable(ComputerGeometry-Graphics ${SOURCE_FILES})
add_executable(CircleSample LAB2_renovate/CircleSample.cpp ${L2_lib})
add_executable(Lab2Full LAB2_renovate/Lab2_Full.cpp ${L2_lib})
add_executable(GammaLinesSample LAB2_renovate/GammaLinesSample.cpp ${L2_lib})