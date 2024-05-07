cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

find_package( OpenCV REQUIRED )

add_library( hw4_xyzio STATIC)
target_sources(hw4_xyzio 
    PUBLIC xyzio.cpp
    PUBLIC xyzio.hpp
)
    
target_include_directories( hw4_xyzio
    PUBLIC ${OpenCV_INCLUDE_DIRS}
    )
target_link_directories( hw4_xyzio
    PUBLIC ${Opencv_LINK_DIRS}
    )
target_link_libraries(hw4_xyzio 
    opencv_world
    )