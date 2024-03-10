# Please run cmake --build .. in the build directory of Homework1

cmake_minimum_required(VERSION 3.10)
project(Homework1 VERSION 1.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/dep/libjpeg-turbo)
include_directories(${CMAKE_SOURCE_DIR}/dep/CImg)
include_directories(${CMAKE_SOURCE_DIR}/src/)
add_executable(Homework1 src/main.cpp)

include(ExternalProject)
ExternalProject_Add(libjpeg-turbo  
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/dep/libjpeg-turbo
    CMAKE_ARGS 
        -DCMAKE_INSTALL_PREFIX=.
)
target_link_libraries(Homework1 
    ${CMAKE_SOURCE_DIR}/build/libjpeg-turbo-prefix/src/libjpeg-turbo-build/Debug/turbojpeg-static.lib
    ${CMAKE_SOURCE_DIR}/build/libjpeg-turbo-prefix/src/libjpeg-turbo-build/Debug/jpeg-static.lib
    ${CMAKE_SOURCE_DIR}/build/libjpeg-turbo-prefix/src/libjpeg-turbo-build/Debug/jpeg.lib
)


# target_link_libraries(Homework1 X11 pthread)
# target_link_libraries(Homework1 libjpeg)