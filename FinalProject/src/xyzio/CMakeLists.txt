cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

find_package( OpenCV REQUIRED )

add_library( finproj_xyzio STATIC)
target_sources(finproj_xyzio 
    PUBLIC xyzio.cpp
    PUBLIC xyzio.hpp
)
    
target_include_directories( finproj_xyzio 
    PUBLIC ${OpenCV_INCLUDE_DIRS}
)

target_link_directories( finproj_xyzio
    PUBLIC ${Opencv_LIB_DIR}
)

target_link_libraries(finproj_xyzio 
    PUBLIC opencv_world
)