cmake_minimum_required(VERSION 3.15)
project(Version_2)

set(CMAKE_CXX_STANDARD 14)

# find system installed GDAL package with predefined CMake variable for finding GDAL
find_package(GDAL REQUIRED)

# Specify location of GDAL header files
include_directories(include /usr/include/gdal)
add_executable(Version_2
    common.h
    main.cpp
    raster.cpp
    raster.h
    Tratamiento_datos.cpp
    Tratamiento_datos.h)
# Specify GDAL libraries to link your cpp executable target against
target_link_libraries(Version_2 gdal)