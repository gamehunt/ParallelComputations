cmake_minimum_required(VERSION 3.18)

project(ParallecComputationsTemplate LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(MPI REQUIRED)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

add_executable(program main.cpp)
target_link_libraries(program ${MPI_CXX_LIBRARIES})
