cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

find_package(OpenCV REQUIRED)
add_library(finalproj_cvmatdebug STATIC)

target_sources(finalproj_cvmatdebug
    PUBLIC cvmatdebug.cpp
    PUBLIC cvmatdebug.hpp
)

target_include_directories(finalproj_cvmatdebug
    PUBLIC ${OpenCV_INCLUDE_DIRS}
)

target_link_directories(finalproj_cvmatdebug
    PUBLIC ${Opencv_LINK_DIRS}
)

target_link_libraries(finalproj_cvmatdebug
    opencv_world
)