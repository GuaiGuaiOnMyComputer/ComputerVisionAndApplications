cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

add_executable(Homework4)

target_sources(Homework4 PUBLIC main.cpp)
target_include_directories(Homework4 
    PUBLIC ${OpenCV_INCLUDE_DIRS}
    PUBLIC ../assetcheck
    PUBLIC ../xyzio
    PUBLIC ../cameracalibration
    PUBLIC ../debugcode
    )
target_link_directories(Homework4
    PUBLIC ${Opencv_LINK_DIRS}
)
target_link_libraries(Homework4
    PUBLIC opencv_world
    PUBLIC hw4_assetcheck
    PUBLIC hw4_xyzio
    PUBLIC hw4_cameracalibration
    PUBLIC hw4_debugcode
)