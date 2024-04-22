#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <filesystem>
#include "xyzio.hpp"

int main(int , char**)
{
    std::vector<hw3::XyzIo::PointCoorAndNormal> vtr = hw3::XyzIo::load_points_from_file<hw3::XyzIo::PointCoorAndNormal>(std::filesystem::path("assets//Santa.xyz"), 1);
    return 0;
}