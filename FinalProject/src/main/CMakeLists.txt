cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

add_executable(FinalProject)

target_sources(FinalProject PUBLIC main.cpp)
target_include_directories(FinalProject 
    PUBLIC ${OpenCV_INCLUDE_DIRS}
    PUBLIC ../scanimageio
    PUBLIC ../assetconfig
    PUBLIC ../directtriangulation
    PUBLIC ../cvmatdebug
    PUBLIC ../featurematching
    PUBLIC ../xyzio
    PUBLIC ../acquirecolor
    )
target_link_directories(FinalProject
    PUBLIC ${Opencv_LINK_DIRS}
)
target_link_libraries(FinalProject
    PUBLIC opencv_world
    PUBLIC finproj_acquirecolor
    PUBLIC finproj_xyzio
    PUBLIC finalproj_scanimageio
    PUBLIC finalproj_assetconfig
    PUBLIC finalproj_directtriangulation
    PUBLIC finalproj_cvmatdebug
    PUBLIC finalproj_featurematching
)