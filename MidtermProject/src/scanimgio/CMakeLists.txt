cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(OpenCV_DIR ../../build/opencv_build)

find_package( OpenCV REQUIRED )

add_library( midproj_scanimgio SHARED)
target_sources(midproj_scanimgio 
    PUBLIC scanimgio.cpp
    PUBLIC scanimgio.hpp
)
    
target_include_directories( midproj_scanimgio PUBLIC ${OPENCV_INCLUDE_DIR})
target_link_directories( midproj_scanimgio  PUBLIC ${OPENCV_LIB_DIR})
target_link_libraries(midproj_scanimgio PUBLIC ${OPENCV_LINK_LIBS})