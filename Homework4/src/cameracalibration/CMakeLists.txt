cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

find_package( OpenCV REQUIRED )

add_library( hw4_cameracalibration STATIC)
target_sources(hw4_cameracalibration 
    PUBLIC cameracalibration.cpp
    PUBLIC cameracalibration.hpp
)
    
target_include_directories( hw4_cameracalibration
    PUBLIC ${OpenCV_INCLUDE_DIRS}
    PUBLIC "../xyzio"
    PUBLIC "../cvmatio"
    )
target_link_directories( hw4_cameracalibration
    PUBLIC ${Opencv_LINK_DIRS}
    PUBLIC hw4_xyzio
    PUBLIC hw4_debugcode
    )
target_link_libraries(hw4_cameracalibration 
    opencv_world
    )