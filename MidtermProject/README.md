# Midterm Project

## Building From Source

The project is built using __w64devkit__ version 1.18 downloaded from [this website](https://github.com/skeeto/w64devkit/releases/tag/v1.18.0) with __cmake__ as the build system. Since the project has an opencv dependency, opencv must be compiled before this project is. Here are the steps to build the project and __opencv__ using cmake:

1. Clone the source code of opencv into ```dep/opencv```
```powershell
cd dep
git clone https://github.com/opencv/opencv.git
```
2. Make the build directory for the project and generate the build files of opencv. Assuming the generator is ```Unix Makefiles```. The directory __build__ and __build/opencv_build__ should be create automatically by cmake and should contain some build files after this process.
```powershell
cd .. # go back to the project root
cmake -S opencv -B build/opencv_build -DCMAKE_C_COMPILER="{abs_path_to_gcc.exe}" -DCMAKE_CXX_COMPILER="{abs_path_to_g++.exe}" -DCMAKE_INSTALL_PREFIX=./build/opencv_build -G "Unix Makefiles"
```
3. Compile the opencv libraries. This process is pretty time consuming while more CPU cores can speed up the process. To use more CPU cores to compile opencv cocurrently, change the value of ```-j8``` to a larger value.
```powershell
cmake ./build/opencv_build --build -j8
```
4. Generate the build files of this project and compile into and executable. The generated executable will be in ```build/MidtermProject.exe``` and all the required runtime libraries should be all be in the proper locations.
```powershell
cmake -S . -B build
cmake ./build --build -j10
```
5. Copy the opencv dynamically-linked libraries to the directory containing the executable so they can be loaded in runtime.
```powershell
cp build/opencv_build/bin build/
```
6. Run the project executable
```powershell
./build/MidtermProject.exe
```