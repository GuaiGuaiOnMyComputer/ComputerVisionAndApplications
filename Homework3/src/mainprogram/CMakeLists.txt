cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(OpenCV_DIR ../../build/opencv_build)

find_package( OpenCV REQUIRED )

add_executable(Homework3)
target_sources(Homework3 
    PUBLIC main.cpp
)
    
target_include_directories( Homework3 PUBLIC ${OPENCV_INCLUDE_DIR})
target_link_directories( Homework3  PUBLIC ${OPENCV_LIB_DIR})
target_link_libraries(Homework3 PUBLIC ${OPENCV_LINK_LIBS})