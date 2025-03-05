cmake_minimum_required(VERSION 3.18)

project(ParallecComputationsTemplate LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Curses REQUIRED)
find_package(OpenMP REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
add_executable(program main.cpp)
target_link_libraries(program ${CURSES_LIBRARIES} OpenMP::OpenMP_CXX)
