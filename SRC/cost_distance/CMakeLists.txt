cmake_minimum_required(VERSION 3.15)
project(test)

set(CMAKE_CXX_STANDARD 14)

#add_executable(MyExecutable main.cpp)
#target_link_libraries(MyExecutable PRIVATE gdal)


# find system installed GDAL package with predefined CMake variable for finding GDAL
find_package(GDAL REQUIRED)

# Specify location of GDAL header files
include_directories( include /usr/include/gdal)
add_executable(test main.cpp raster.cpp raster.h raster_cost_dist.cpp raster_cost_dist.h)
# Specify GDAL libraries to link your cpp executable target against
target_link_libraries(test gdal)

