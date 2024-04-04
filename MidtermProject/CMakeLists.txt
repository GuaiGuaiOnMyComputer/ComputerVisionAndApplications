cmake_minimum_required(VERSION 3.25)
project( MidtermProject )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

set(OpenCV_DIR dep/opencv_build/ )
find_package( OpenCV REQUIRED )
include_directories( 
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
    dep/opencv_build
    src/redpixelfinder
    src/virtualcamera
    src/xyzio
    )

add_library( redpixelfinder SHARED
    src/redpixelfinder/redpixelfinder.cpp
    src/redpixelfinder/redpixelfinder.hpp
    )
add_library( virtualcamera SHARED
    src/virtualcamera/virtualcamera.cpp
    src/virtualcamera/virtualcamera.hpp
    )
add_library( xyzio SHARED
    src/xyzio/xyzio.cpp
    src/xyzio/xyzio.hpp
    )

add_executable( MidtermProject 
    src/main.cpp
    )


link_directories( MidtermProject 
    dep/opencv_build/lib/
    build/redpixelfinder
    )
link_directories( redpixelfinder 
    dep/opencv_build/lib/
    )
link_directories( virtualcamera 
    dep/opencv_build/lib/
    )
link_directories( xyzio 
    dep/opencv_build/lib/
    )
target_sources( MidtermProject
    PUBLIC src/main.cpp
    )

target_link_libraries(redpixelfinder
    opencv_calib3d
    opencv_imgcodecs
    opencv_highgui
    opencv_imgproc
    opencv_features2d
    opencv_flann
    opencv_core
    )
target_link_libraries(virtualcamera
    opencv_calib3d
    opencv_imgcodecs
    opencv_highgui
    opencv_imgproc
    opencv_features2d
    opencv_flann
    opencv_core
    )
target_link_libraries(xyzio
    opencv_calib3d
    opencv_imgcodecs
    opencv_highgui
    opencv_imgproc
    opencv_features2d
    opencv_flann
    opencv_core
    )
target_link_libraries(MidtermProject 
    opencv_calib3d
    opencv_imgcodecs
    opencv_highgui
    opencv_imgproc
    opencv_features2d
    opencv_flann
    opencv_core
    redpixelfinder
    virtualcamera
    xyzio
    )