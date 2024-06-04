cmake_minimum_required(VERSION 3.25)

set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED On)

find_package(OpenCV REQUIRED)
add_library(finalproj_featurematching STATIC)

target_sources(finalproj_featurematching
    PUBLIC featurematching.cpp
    PUBLIC featurematching.hpp
)

target_include_directories(finalproj_featurematching
    PUBLIC ${OpenCV_INCLUDE_DIRS}
    PUBLIC "../assetconfig"
)

target_link_directories(finalproj_featurematching
    PUBLIC ${Opencv_LINK_DIRS}
)

target_link_libraries(finalproj_featurematching
    PUBLIC opencv_world
)