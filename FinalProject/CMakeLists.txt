cmake_minimum_required(VERSION 3.25)
project( FinalProject )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# set the default build type as release
# build with debug sybmols if build types other than Release is given
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Os")

if (UNIX)
    find_package(Iconv REQUIRED)
    set(OpenCV_DIR "/opt/opencv/build")
    find_package(Iconv)
elseif(WIN32)
    set(OpenCV_DIR "C:/opencv/build/MinGW/Release")
endif()
if (NOT OpenCV_DIR)
    message(FATAL_ERROR "Please set variable OpenCV_DIR to the directory containing file OpenCVConfig.cmake. You can do so by running command ```cmake -S {sourceDir} -B {buildDir} -DOpenCV_DIR='{PATH_TO_OpenCVConfig.cmake}```")
endif()
find_package(OpenCV REQUIRED)

add_subdirectory(src/main)
add_subdirectory(src/scanimageio)
add_subdirectory(src/assetconfig)
add_subdirectory(src/directtriangulation)
add_subdirectory(src/cvmatdebug)
set_target_properties(FinalProject PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ../../../bin
    ARCHIVE_OUTPUT_DIRECTORY ../../../bin/lib
)