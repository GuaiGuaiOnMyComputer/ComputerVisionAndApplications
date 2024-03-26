# Computer Vision and Applications

## To Run the Program

The executable is located in the folder ```dist/Homework2.exe``` along with all the required runtime libraries. Double clicking on the executable runs the program.

## Submitted Items
* Output image __M11225013.jpg__
* Source code of the homework
* Image assets
* Opencv dlls required by the homework program at runtime

## Dependencies

### Opencv 4.9
The source code is in folder ```dep/opencv``` and the build directory set at ```dep/opencv_build```. Library built by __cmake__ 3.29 with __gcc__ 13.1 along with __win64devsdk__ 1.19.0.

Build the library by:
```bash
mkdir opencv_build
cd dep

cmake -S .\opencv\ -B .\opencv_build\  -DCMAKE_INSTALL_PREFIX=opencv_build  -DBUILD_WITH_DEGUG_INFO=OFF  -DWITH_DSHOW=OFF  -DWITH_FFMPEG=OFF  -DBUILD_opencv_calib3d=OFF  -DBUILD_opencv_features2d=OFF -DBUILD_opencvflann=OFF  -DBUILD_opencv_ml=OFF  -DBUILD_opencv_dnn=OFF -DBUILD_opencv_ts=OFF  -DBUILD_opencv_video=OFF  -DBUILD_opencv_videoio=OFF  -G "MinGW Makefiles"

cmake --build .\opencv_build -j6
```
Opencv modules which are not essential for this homework are skipped to save build time and debug info is stripped from the compiled libraries to reduce file sizes.

As of 26.March.2024, windows defender might interfere with cmake generating build files by identifying the test compile files as malware and removing them if the version of __GCC__ or __win64devsdk__ is too new. Using __win64devsdk__ version 1.19.0 avoids this issue.