cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

add_library(finproj_acquirecolor STATIC)

target_sources(finproj_acquirecolor
    PUBLIC acquirecolor.cpp
    PUBLIC acquirecolor.hpp
)

target_include_directories(finproj_acquirecolor
    PUBLIC ${OpenCV_INCLUDE_DIRS}
    PUBLIC ../xyzio
)

target_link_directories(finproj_acquirecolor
    PUBLIC ${Opencv_LINK_DIRS}
)

target_link_libraries(finproj_acquirecolor
    opencv_world
    finproj_xyzio
)