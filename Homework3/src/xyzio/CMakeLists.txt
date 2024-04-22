cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(OpenCV_DIR ../../build/opencv_build)

find_package( OpenCV REQUIRED )

add_library( hw3_xyzio SHARED)
target_sources(hw3_xyzio 
    PUBLIC xyzio.cpp
    PUBLIC xyzio.hpp
)
    
target_include_directories( hw3_xyzio PUBLIC ${OPENCV_INCLUDE_DIR})
target_link_directories( hw3_xyzio  PUBLIC ${OPENCV_LIB_DIR})
target_link_libraries(hw3_xyzio PUBLIC ${OPENCV_LINK_LIBS})