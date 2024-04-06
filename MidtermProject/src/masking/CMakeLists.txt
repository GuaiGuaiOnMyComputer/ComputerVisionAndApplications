cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(OpenCV_DIR ../../build/opencv_build)

find_package( OpenCV REQUIRED )

add_library( masking SHARED
    masking.cpp
    masking.hpp
    )

target_include_directories( masking
    PUBLIC ${OPENCV_INCLUDE_DIR}
)

target_link_directories( masking 
    PUBLIC ${OPENCV_LIB_DIR}
    )
target_link_libraries(masking
    PUBLIC ${OPENCV_LINK_LIBS}
    )