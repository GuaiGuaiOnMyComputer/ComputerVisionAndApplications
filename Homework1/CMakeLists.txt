cmake_minimum_required(VERSION 3.10)
project(Homework1 VERSION 1.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(dep/CImg)
add_executable(Homework1 src/main.cpp)

target_link_libraries(Homework1 X11 pthread)