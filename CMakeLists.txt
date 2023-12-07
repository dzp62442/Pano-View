cmake_minimum_required(VERSION 3.10)
project(pano-view LANGUAGES CXX CUDA)

add_compile_options(-std=c++14)
set(CMAKE_CXX_STANDARD 14)
if (NOT CMAKE_BUILD_TYPE)
    # Options: Debug, Release, MinSizeRel, RelWithDebInfo
    set(CMAKE_BUILD_TYPE "Release")
    message(STATUS "No build type selected, default to ${CMAKE_BUILD_TYPE}")
endif()

# CUDA
find_package(CUDA REQUIRED)
enable_language(CUDA)
set(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)
set(CMAKE_NVCC_FLAGS
    ${CMAKE_NVCC_FLAGS};
    -Xcompiler
    -fopenmp
    -O2
    -use_fast_math
    -gencode arch=compute_87,code=sm_87
)
set(CMAKE_CUDA_FLAGS "-arch=sm_87")
include_directories(${CUDA_INCLUDE_DIRS})

# OpenMP
find_package(OpenMP REQUIRED)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS})

# OpenGL
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${GLM_INCLUDE_DIR})

# Assimp
find_package(ASSIMP REQUIRED)
include_directories(${ASSIMP_INCLUDE_DIRS})

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Eigen3
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIRS})

# Yaml-cpp
set(YAML_CPP_LIBRARIES /usr/lib/aarch64-linux-gnu/libyaml-cpp.so)

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB_RECURSE My_Source_Files ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${My_Source_Files})
target_link_libraries(${PROJECT_NAME}
    ${CUDA_LIBRARIES}    
    ${OPENGL_LIBRARIES} glfw ${GLM_LIBRARIES}
    ${ASSIMP_LIBRARIES}
    ${OpenCV_LIBS}
    ${YAML_CPP_LIBRARIES}
)

set_target_properties(${PROJECT_NAME} 
            PROPERTIES OUTPUT_NAME ${PROJECT_NAME} 
			CUDA_SEPARABLE_COMPILATION ON
)
set_property(TARGET ${PROJECT_NAME} PROPERTY CUDA_STANDARD 14)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:${CMAKE_NVCC_FLAGS}>)
