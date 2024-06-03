cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

add_library(finalproj_assetconfig STATIC)

target_sources(finalproj_assetconfig
    PUBLIC assetconfig.cpp
    PUBLIC assetconfig.hpp
)