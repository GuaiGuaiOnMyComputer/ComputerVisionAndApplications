#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <filesystem>
#include <fstream>
#include <vector>
#include <iostream>
#include "XyzParser.cpp" // TODO: include header file instead
#include "Camera.cpp"

int main()
{
  std::filesystem::path xyzPath("asset/3D_Trajectory.xyz");
  if (! std::filesystem::exists(xyzPath))
    throw std::runtime_error("File not found at asset/3D_Trajectory.xyz");
  XyzParser parser(xyzPath);
  std::vector<float> trajectory = parser.ParseAll();


  std::filesystem::path cameraParameterPath("asset/CameraParameter.txt");
  if (! std::filesystem::exists(cameraParameterPath))
    throw std::runtime_error("File not found at asset/CameraParameter.txt");
  Camera camera1("camera1"), camera2("camera2");
  camera1.GetParameterFromFile(cameraParameterPath, "Cam1_K", "Cam1_RT");
  camera2.GetParameterFromFile(cameraParameterPath, "Cam2_K", "Cam2_RT");

  return 0;
}