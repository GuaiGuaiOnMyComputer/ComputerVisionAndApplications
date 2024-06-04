cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

find_package(OpenCV REQUIRED)
add_library(finalproj_directtriangulation STATIC)

target_sources(finalproj_directtriangulation
    PUBLIC directtriangulation.cpp
    PUBLIC directtriangulation.hpp
)

target_include_directories(finalproj_directtriangulation
    PUBLIC ${OpenCV_INCLUDE_DIRS}
    PUBLIC ../cvmatdebug
)

target_link_directories(finalproj_directtriangulation
    PUBLIC ${Opencv_LINK_DIRS}
)

target_link_libraries(finalproj_directtriangulation
    PRIVATE opencv_world
    PRIVATE finalproj_cvmatdebug
)