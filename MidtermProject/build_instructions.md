# Compile Midterm Project

## Building From Source

The project is built using __w64devkit__ version 1.18 downloaded from [this website](https://github.com/skeeto/w64devkit/releases/tag/v1.18.0) with __cmake__ as the build system and __gcc__ compiler on windows operating system. Since the project has an opencv dependency, opencv must be compiled before this project is. Here are the steps to build the project and __opencv__ using cmake:

### Compiling OpenCV Dependency

Step1. Clone the source code of opencv into ```dep/opencv```
   
```powershell
cd dep
git clone https://github.com/opencv/opencv.git
```
Step2. Create the build directory for the project and generate the build files of opencv. The generator is used is```MinGW Makefiles``` due to usage of ```gcc```, replace the option with ```Visual Studio 8``` for MSVC. The directory __build__ and __build/opencv_build__ should be create automatically by cmake and should contain some build files after this process.
   
```powershell
cd .. # go back to the project root
cmake -S dep/opencv -B build/opencv_build -DCMAKE_C_COMPILER="{abs_path_to_gcc.exe}" -DCMAKE_CXX_COMPILER="{abs_path_to_g++.exe}" -G "MinGW Makefiles" 
```
Step3. Compile the opencv libraries. This process is pretty time consuming while more CPU cores can speed up the process. To use more CPU cores to compile opencv cocurrently, change the value of ```-j8``` to a larger value.
   
```powershell
cmake --build ./build/opencv_build -j8
```
Step4. Copy the opencv dynamically-linked libraries to the directory containing the projcet's executable so they can be loaded in runtime.
   
```powershell
cp build/opencv_build/bin/*.dll build/
```
   
### Compile the Project Binaries
   
Step1. Generate the build files of the project's binaries.
  
```powershell
cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Release
```
  
Step2. Compile the project's binaries. This should generate a __MidtermProject.exe__ and several __libmidproj_xxx.dll__ s  under __build__ directory.
  
```powershell
cmake --build ./build -j4
```
Step3. Run the executable. Output xyz file will be contained in the newly created __output__ folder.

```powershell
./build/MidtermProject.exe
```