#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <filesystem>
#include <vector>
#include "XyzParser.cpp" // TODO: include header file instead

int main()
{
  XyzParser parser(std::filesystem::path("asset/3D_Trajectory.xyz"));
  std::vector<float> trajectory;
  parser.ParseAll(trajectory);
  return 0;
}