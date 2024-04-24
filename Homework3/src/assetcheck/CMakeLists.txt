cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

add_library(hw3_assetcheck)
target_sources(hw3_assetcheck 
    PUBLIC assetcheck.cpp
    PUBLIC assetcheck.hpp
)