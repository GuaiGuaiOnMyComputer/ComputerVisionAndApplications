cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

set(OpenCV_DIR ../../build/opencv_build/ )
find_package( OpenCV REQUIRED )

add_library( sculptureslice SHARED
    sculptureslice.cpp
    sculptureslice.hpp
    )

target_include_directories( sculptureslice
    PUBLIC ${OPENCV_INCLUDE_DIR}
    PUBLIC ../masking
    )
target_link_directories( sculptureslice 
    PUBLIC ${OPENCV_LIB_DIR}
    )
target_link_libraries(sculptureslice
    PUBLIC ${OPENCV_LINK_LIBS}
    )