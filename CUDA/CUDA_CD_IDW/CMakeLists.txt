cmake_minimum_required(VERSION 3.18)
project(CUDA_CD_IDW CUDA)

# Find GDAL
find_package(GDAL REQUIRED)

# Include GDAL directories
include_directories(${GDAL_INCLUDE_DIRS})


set(CMAKE_CUDA_STANDARD 17)

add_executable(CUDA_CD_IDW main.cu Raster.h Raster.cpp)

target_link_libraries(CUDA_CD_IDW ${GDAL_LIBRARIES})

set_target_properties(CUDA_CD_IDW PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON)