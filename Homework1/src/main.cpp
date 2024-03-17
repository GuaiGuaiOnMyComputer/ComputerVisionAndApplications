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

std::error_code writeImageToFile(const std::filesystem::path& filePath, const cv::Mat& image);

int main()
{
  std::filesystem::path xyzPath("asset/3D_Trajectory.xyz");
  if (! std::filesystem::exists(xyzPath))
    throw std::runtime_error("File not found at asset/3D_Trajectory.xyz");
  XyzParser parser(xyzPath);
  std::vector<cv::Point3f> trajectory = parser.ParseAll();

  // load the camrea parameters from text file
  std::filesystem::path cameraParameterPath("asset/CameraParameter.txt");
  if (! std::filesystem::exists(cameraParameterPath))
    throw std::runtime_error("File not found at asset/CameraParameter.txt");
  Camera camera1("camera1"), camera2("camera2");
  camera1.GetParameterFromFile(cameraParameterPath, "Cam1_K", "Cam1_RT");
  camera2.GetParameterFromFile(cameraParameterPath, "Cam2_K", "Cam2_RT");

  // project the points in 3D world onto the image
  std::vector<cv::Point3f> trajectoryInCam1 = camera1.WorldToImageCoordinate(trajectory);
  std::vector<cv::Point3f> trajectoryInCam2 = camera2.WorldToImageCoordinate(trajectory);

  // load the image asset as cv::Mat 
  std::filesystem::path scene1Path("asset/SceneFromCamera1.jpg");
  std::filesystem::path scene2Path("asset/SceneFromCamera2.jpg");
  if (! (std::filesystem::exists(scene1Path) || std::filesystem::exists(scene2Path)))
    throw std::runtime_error("Images taken by camera1 or camera2 is missing.");
  cv::Mat scene1 = cv::imread(scene1Path.string(), cv::ImreadModes::IMREAD_COLOR);
  cv::Mat scene2 = cv::imread(scene2Path.string(), cv::ImreadModes::IMREAD_COLOR);

  // draw lines connecting each point on the trajectory onto the image
  for (size_t i = 0; i < trajectory.size() - 1; i++)
  {
    cv::Point2f currentPoint(trajectoryInCam1[i].x, trajectoryInCam1[i].y);
    cv::Point2f nextPoint(trajectoryInCam1[i + 1].x, trajectoryInCam1[i + 1].y);
    cv::line(scene1, currentPoint, nextPoint,  cv::Scalar(0, 255, 0), 5);
    currentPoint = cv::Point(trajectoryInCam2[i].x, trajectoryInCam2[i].y); 
    nextPoint = cv::Point(trajectoryInCam2[i + 1].x, trajectoryInCam2[i + 1].y); 
    cv::line(scene2, currentPoint, nextPoint, cv::Scalar(0, 255, 0), 5);
  }

  cv::imshow("TrajectoryViewFromCamera1", scene1);
  cv::imshow("TrajectoryViewFromCamera2", scene2);
  cv::waitKey(0);
  cv::destroyAllWindows();
  writeImageToFile("M11225013_1.jpg", scene1);
  writeImageToFile("M11225013_2.jpg", scene2);
  return 0;
}

std::error_code writeImageToFile(const std::filesystem::path& fileName, const cv::Mat& image)
{
  std::error_code fileOutputError;
  std::filesystem::path outputPath = std::filesystem::path();
  const std::filesystem::path currentWorkingDir = std::filesystem::current_path();
  if (fileName.is_relative())
    outputPath /= currentWorkingDir;
  outputPath /= "output";
  std::filesystem::create_directories(outputPath, fileOutputError);
  outputPath /= fileName;
  cv::imwrite(outputPath.string(), image);
  std::cout << "Image file has been written into " << outputPath << '\n';
  return fileOutputError;
}