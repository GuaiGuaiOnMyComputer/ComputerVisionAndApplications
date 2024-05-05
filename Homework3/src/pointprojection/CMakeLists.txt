cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(OpenCV_DIR ../../build/opencv_build)

find_package( OpenCV REQUIRED )

add_library( hw3_pointprojection SHARED)
target_sources(hw3_pointprojection 
    PUBLIC pointprojection.cpp
    PUBLIC pointprojection.hpp
)
    
target_include_directories( hw3_pointprojection
    PUBLIC ${OPENCV_INCLUDE_DIR}
    ../xyzio
    )
target_link_directories( hw3_pointprojection  PUBLIC ${OPENCV_LIB_DIR})
target_link_libraries(hw3_pointprojection PUBLIC
    ${OPENCV_LINK_LIBS}
    hw3_xyzio
    )