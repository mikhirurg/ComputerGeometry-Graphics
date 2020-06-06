cmake_minimum_required(VERSION 3.15)
project(ComputerGeometry-Graphics)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")

set(SOURCE_FILES LAB2_FULL/main.cpp)
set(L2_lib LAB2_renovate/CImageFileOpenException.cpp LAB2_renovate/CImage.cpp LAB2_renovate/CImageMemAllocException.cpp LAB2_renovate/CImageException.cpp LAB2_renovate/CImageFileDeleteException.cpp LAB2_renovate/CImageParamsException.cpp LAB2_renovate/CImageFileReadException.cpp LAB2_renovate/CImageFileFormatException.cpp)
set(L3_lib LAB3/CImageException.cpp LAB3/CImageFileDeleteException.cpp LAB3/CImageFileFormatException.cpp LAB3/CImageFileOpenException.cpp LAB3/CImageFileReadException.cpp LAB3/CImageMemAllocException.cpp LAB3/CImageParamsException.cpp)
set(L4_lib LAB4/CImageParamsException.cpp LAB4/CImageMemAllocException.cpp LAB4/CImageFileReadException.cpp LAB4/CImageFileOpenException.cpp LAB4/CImageFileFormatException.cpp LAB4/CImageFileDeleteException.cpp LAB4/CImageException.cpp LAB4/CPixel.cpp LAB4/CSpace.cpp)

#LAB 2

add_executable(ComputerGeometry-Graphics ${SOURCE_FILES})
add_executable(CircleSample LAB2_renovate/CircleSample.cpp ${L2_lib})
add_executable(Lab2Full LAB2_renovate/Lab2_Full.cpp ${L2_lib})
add_executable(GammaLinesSample LAB2_renovate/GammaLinesSample.cpp ${L2_lib})

#LAB 3

add_executable(LAB3_test LAB3/test.cpp ${L3_lib})
add_executable(LAB3_final LAB3/Lab3_full.cpp ${L3_lib})
add_executable(LAB3_final_color LAB3/Lab3_full_color.cpp ${L3_lib})

#LAB 4

add_executable(Lab4 LAB4/Lab4.cpp ${L4_lib})
