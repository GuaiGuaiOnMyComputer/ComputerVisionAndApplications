cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

add_library(hw4_assetcheck STATIC)

target_sources(hw4_assetcheck 
    PUBLIC assetcheck.cpp
    PUBLIC assetcheck.hpp
    )
target_link_libraries(hw4_assetcheck
    PUBLIC opencv_world
)