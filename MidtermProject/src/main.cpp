#include "redpixelfinder.hpp"
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
    cv::Mat img = cv::imread("assets/ShadowStrip/0035.jpg");
    midproj::find_red_pixels(img, redPoints);
    cv::imshow("Test image", img);
    cv::waitKey();
    return 0;
}

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