cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

set(OpenCV_DIR ../../dep/opencv_build/ )
find_package( OpenCV REQUIRED )

add_library( sculptureslice SHARED
    sculptureslice.cpp
    sculptureslice.hpp
    )

include_directories( 
    ../../dep/opencv/include
    ../../dep/opencv/modules/core/include
    ../../dep/opencv/modules/highgui/include
    ../../dep/opencv/modules/imgcodecs/include
    ../../dep/opencv/modules/dnn/include
    ../../dep/opencv/modules/ml/include
    ../../dep/opencv/modules/stitching/include
    ../../dep/opencv/modules/objdetect/include
    ../../dep/opencv/modules/videoio/include
    ../../dep/opencv/modules/video/include
    ../../dep/opencv/modules/photo/include
    ../../dep/opencv/modules/imgproc/include
    ../../dep/opencv/modules/gapi/include
    ../../dep/opencv/modules/flann/include
    ../../dep/opencv/modules/features2d/include
    ../../dep/opencv/modules/calib3d/include
    ../../dep/opencv_build
    ../masking
    )
link_directories( sculptureslice 
    ../../build/opencv_build/lib/
    )
target_link_libraries(sculptureslice
    opencv_calib3d
    opencv_imgcodecs
    opencv_highgui
    opencv_imgproc
    opencv_features2d
    opencv_flann
    opencv_core
    )