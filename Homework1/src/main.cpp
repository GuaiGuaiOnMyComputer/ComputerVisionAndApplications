#include <opencv2/core/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
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
  std::vector<float> trajectoryInCam1 = camera1.WorldToImageCoordinate(trajectory);
  std::vector<float> trajectoryInCam2 = camera2.WorldToImageCoordinate(trajectory);

  std::filesystem::path scene1Path("asset/SceneFromCamera1.jpg");
  std::filesystem::path scene2Path("asset/SceneFromCamera2.jpg");
  if (! (std::filesystem::exists(scene1Path) || std::filesystem::exists(scene2Path)))
    throw std::runtime_error("Images taken by camera1 or camera2 is missing.");

  cv::Mat scene1 = cv::imread(scene1Path.string(), cv::ImreadModes::IMREAD_COLOR);
  cv::Mat scene2 = cv::imread(scene2Path.string(), cv::ImreadModes::IMREAD_COLOR);

  for (size_t i = 0; i < trajectory.size() / 3; i+=3)
  {
    cv::Point currentPoint((int32_t)trajectoryInCam1.at(3 * i),(int32_t)trajectoryInCam1.at(3 * i + 1)); // trajectory.at(3 * i + 2) is the homogeneous component 1 for each point
    cv::Point nextPoint((int32_t)trajectoryInCam1.at(3 * i + 3), (int32_t)trajectoryInCam1.at(3 * i + 4)); // the next 3 components of a point in trajectoryInCam1 will be 3 * i + 3 and 3 * i + 4
    cv::line(scene1, currentPoint, nextPoint, cv::Scalar(0, 255, 0), 5);
    currentPoint = cv::Point(trajectoryInCam2.at(3 * i), trajectoryInCam2.at(3 * i + 1)); // trajectory.at(3 * i + 2) is the homogeneous component 1 for each point
    nextPoint = cv::Point(trajectoryInCam2.at(3 * i + 3), trajectoryInCam2.at(3 * i + 4)); // the next 3 components of a point in trajectoryInCam1 will be 3 * i + 3 and 3 * i + 4
    cv::line(scene2, currentPoint, nextPoint, cv::Scalar(0, 255, 0), 5);
  }

  cv::imshow("TrajectoryViewFromCamera1", scene1);
  cv::imshow("TrajectoryViewFromCamera2", scene2);
  cv::waitKey(0);
  cv::destroyAllWindows();
  cv::imwrite("TrajectoryCiewFromCamera1.jpg", scene1);
  cv::imwrite("TrajectoryCiewFromCamera2.jpg", scene2);
  return 0;
}