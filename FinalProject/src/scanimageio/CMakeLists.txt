cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

find_package(OpenCV REQUIRED)
add_library(finalproj_scanimageio STATIC)

target_sources(finalproj_scanimageio
    PUBLIC scanimageio.cpp
    PUBLIC scanimageio.hpp
)

target_include_directories(finalproj_scanimageio
    PUBLIC ${OpenCV_INCLUDE_DIRS}
)

target_link_directories(finalproj_scanimageio
    PUBLIC ${Opencv_LINK_DIRS}
)

target_link_libraries(finalproj_scanimageio
    opencv_world
)