cmake_minimum_required(VERSION 3.10)
project(Homework1 VERSION 1.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(dep/libjpeg-turbo)
include_directories(dep/CImg)
add_executable(Homework1 src/main.cpp)

# add_library(libjpeg-turbo STATIC dep/libjpeg-turbo/)
include(ExternalProject)
ExternalProject_Add(libjpeg-turbo  
    SOURCE_DIR ../dep/libjpeg-turbo
    CMAKE_ARGS 
        -DCMAKE_INSTALL_PREFIX=.
)
set_target_properties(libjpeg-turbo PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(Homework1 libjpeg-turbo-prefix/src/libjpeg-turbo-build/Debug/jpeg)


# target_link_libraries(Homework1 X11 pthread)
# target_link_libraries(Homework1 libjpeg)