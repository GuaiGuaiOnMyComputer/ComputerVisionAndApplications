cmake_minimum_required(VERSION 3.25)
project( MidtermProject )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# set the default build type as release
# build with debug sybmols if build types other than Release is given
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(OpenCV_DIR build/opencv_build )
find_package(OpenCV REQUIRED)

set(OPENCV_INCLUDE_DIR CACHE
    dep/opencv/include
    dep/opencv/modules/core/include
    dep/opencv/modules/highgui/include
    dep/opencv/modules/imgcodecs/include
    dep/opencv/modules/dnn/include
    dep/opencv/modules/ml/include
    dep/opencv/modules/stitching/include
    dep/opencv/modules/objdetect/include
    dep/opencv/modules/videoio/include
    dep/opencv/modules/video/include
    dep/opencv/modules/photo/include
    dep/opencv/modules/imgproc/include
    dep/opencv/modules/gapi/include
    dep/opencv/modules/flann/include
    dep/opencv/modules/features2d/include
    dep/opencv/modules/calib3d/include
    build/opencv_build
    )
set(OPENCV_LIB_DIR
    build/opencv_build/lib
)
set(OPENCV_LINK_LIBS
    opencv_calib3d
    opencv_imgcodecs
    opencv_highgui
    opencv_imgproc
    opencv_features2d
    opencv_flann
    opencv_core
)

add_subdirectory(src/xyzio)
add_subdirectory(src/mainprogram)
add_subdirectory(src/assetcheck)
add_subdirectory(src/pointprojection)
set_target_properties(Homework3 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../../../bin)
set_target_properties(hw3_xyzio PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../../../bin)
set_target_properties(hw3_assetcheck PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../../../bin)
set_target_properties(hw3_pointprojection PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../../../bin)