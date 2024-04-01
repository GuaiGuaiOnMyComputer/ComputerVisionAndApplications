# Midterm Project

## Building From Source

The project is built using __w64devkit__ version 1.18 downloaded from [this website](https://github.com/skeeto/w64devkit/releases/tag/v1.18.0) with __cmake__ as the build system. Here are the steps to build the project and __opencv__ using cmake:

1. Clone the source code of opencv into ```dep/opencv```
```powershell
cd dep
git clone https://github.com/opencv/opencv.git
```
2. Make the build directory for opencv and generate the build files. Assuming the generator is ```MinGW Makefiles```:
```powershell
mkdir opencv_build
cmake -S opencv -B opencv_build -DCMAKE_C_COMPILER="{abs_path_to_gcc.exe}" -DCMAKE_CXX_COMPILER="{abs_path_to_g++.exe}" -G "MinGW Makefiles"
```
3. Compile the dynamically linked opencv libraries
```powershell
cmake ./opencv_build --build -j8
```
4. Generate the build files of this project and compile into and executable. The generated executable will be in ```build/MidtermProject.exe```
```powershell
cd ..
mkdir build
cmake -S . -B build
cmake ./build --build -j10
```
5. Copy the required runtime libraries to where the executable is so the runtime dynamic linker can find the resources.
```powershell
cp dep/opencv_build/bin/* build/
```