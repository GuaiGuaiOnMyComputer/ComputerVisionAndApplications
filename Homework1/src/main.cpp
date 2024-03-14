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

  std::filesystem::path scene1Path("asset/SceneFromCamera1.jpg");
  std::filesystem::path scene2Path("asset/SceneFromCamera2.jpg");
  if (! (std::filesystem::exists(scene1Path) || std::filesystem::exists(scene2Path)))
    throw std::runtime_error("Images taken by camera1 or camera2 is missing.");

  cv::Mat scene1 = cv::imread(scene1Path.string(), cv::ImreadModes::IMREAD_COLOR);
  cv::Mat scene2 = cv::imread(scene2Path.string(), cv::ImreadModes::IMREAD_COLOR);
  return 0;
}