cmake_minimum_required(VERSION 3.18)
project(CUDA_CD_IDW CUDA)

set(CMAKE_CUDA_STANDARD 17)

add_executable(CUDA_CD_IDW main.cu)

set_target_properties(CUDA_CD_IDW PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON)
