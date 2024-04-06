#include "masking.hpp"
#include "xyzio.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <stdint.h>
#include <filesystem>
#include <iostream>
#include <sstream>

#if false
template class std::vector<cv::Point2i>;
template class std::vector<cv::Point2f>;
#endif

bool assetCheck(); 

int main(int32_t argc, char** argv)
{
    std::filesystem::path frameCorner2dFilePath("assets//FrameCornerCoordinates2d.xyz");
    std::filesystem::path frameCorner3dFilePath("assets//FrameCornerCoordinates3d.xyz");
    // Step1: determine the camera matrix from the corners of the wire frame
    // Step2: identify the red pixels
    // Step3: project the red
    assetCheck();
    std::vector<cv::Point2i> frameCorners2i = midproj::XyzIo::load_points_from_file_2i(frameCorner2dFilePath);
    std::vector<cv::Point2i> frameCorners3i = midproj::XyzIo::load_points_from_file_2i(frameCorner3dFilePath);

    return 0;
}

/// @brief Check if all 54 images are present. Quit the program if not.
/// @return true if all the images are found.
bool assetCheck()
{
    namespace fs = std::filesystem;

    // check for all 54 images
    std::stringstream fileName;
    fs::path filePath;
    filePath /= "assets//ShadowStrip";
    for (size_t i = 0; i < 54; i++)
    {
        fileName << std::setfill('0') << std::setw(4) << i << ".jpg";
        filePath /= fileName.str();
        if (!fs::exists(filePath))
        {
            std::cout << "The required file: " << fs::absolute(filePath) << " does not exist";
            exit(-1);
        }
        filePath.remove_filename();
        fileName.str(std::string());
    }

    // check for the two xyz files
    if (! fs::exists("assets//FrameCornerCoordinates2d.xyz") || !fs::exists("assets//FrameCornerCoordinates3d.xyz"))
    {
        std::cout << "At least one of the required file containing the coordinates of the frame corner is missing";
        exit(0);
    }
    return true;
}