cmake_minimum_required(VERSION 3.18)

project(ParallecComputationsTemplate LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCV REQUIRED PATHS /hdd/Builds/opencv_build NO_DEFAULT_PATH)
find_package(MPI REQUIRED)
add_executable(program main.cpp)
target_include_directories(program PRIVATE ${OpenCV_INCLUDE_DIRS} ${MPI_INCLUDE_PATH})
target_link_libraries(program ${OpenCV_LIBS} ${MPI_CXX_LIBRARIES})
