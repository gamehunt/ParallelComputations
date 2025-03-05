cmake_minimum_required(VERSION 3.18)

project(ParallecComputationsTemplate LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# find_package(Curses REQUIRED)
find_package(OpenCV REQUIRED PATHS /hdd/Builds/opencv_build NO_DEFAULT_PATH)
find_package(OpenMP REQUIRED)
# include_directories(${CURSES_INCLUDE_DIR})
add_executable(program main.cpp)
target_include_directories(program PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(program ${OpenCV_LIBS} OpenMP::OpenMP_CXX)
