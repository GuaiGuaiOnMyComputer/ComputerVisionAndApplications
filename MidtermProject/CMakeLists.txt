cmake_minimum_required(VERSION 3.25)
project( MidtermProject )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

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
add_subdirectory(src/masking)
add_subdirectory(src/slicetrans)
set_target_properties(midproj_xyzio PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../../../build)
set_target_properties(midproj_masking PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../../../build)
set_target_properties(midproj_slicetrans PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../../../build)

add_executable(MidtermProject)

target_include_directories( MidtermProject
    PUBLIC src/masking
    PUBLIC src/slicetrans
    PUBLIC src/xyzio
    PUBLIC ${OPENCV_INCLUDE_DIR}
    )


target_sources( MidtermProject PUBLIC src/main.cpp)

target_link_libraries(MidtermProject 
    midproj_masking
    midproj_xyzio
    midproj_slicetrans
    ${OPENCV_LINK_LIBS}
    )