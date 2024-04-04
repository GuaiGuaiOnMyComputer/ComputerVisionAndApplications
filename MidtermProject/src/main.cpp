#include "redpixelfinder.hpp"
#include "virtualcamera.hpp"
#include "xyzio.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <stdint.h>
#include <filesystem>
#include <iostream>
#include <sstream>


bool assetCheck(); 

int main(int32_t argc, char** argv)
{
    // Step1: determine the camera matrix from the corners of the wire frame
    // Step2: identify the red pixels
    // Step3: project the red
    assetCheck();

    std::vector<cv::Point> redPoints;
    return 0;
}

/// @brief Check if all 54 images are present. Quit the program if not.
/// @return true if all the images are found.
bool assetCheck()
{
    namespace fs = std::filesystem;
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
    return true;
}