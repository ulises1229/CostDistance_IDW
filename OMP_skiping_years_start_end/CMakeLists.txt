cmake_minimum_required(VERSION 3.18)
project(OMP_skiping_years)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fopenmp")

include_directories(.)

find_package(GDAL REQUIRED)

add_executable(OMP_skiping_years
        common.h
        main.cpp
        Methods_IDW_CD.cpp
        Methods_IDW_CD.h
        Raster.cpp
        Raster.h)
target_link_libraries(OMP_skiping_years gdal)