cmake_minimum_required(VERSION 3.19)

project(ParallecComputationsTemplate LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenMP REQUIRED)
find_package(OpenSSL REQUIRED)
add_executable(program main.cpp)
target_link_libraries(program PRIVATE OpenMP::OpenMP_CXX OpenSSL::Crypto)
