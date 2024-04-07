#include "masking.hpp"
#include "xyzio.hpp"
#include "sculptureslice.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <stdint.h>
#include <filesystem>
#include <iostream>
#include <sstream>

#if true
template class std::vector<cv::Point2i>;
template class std::vector<cv::Point2f>;
template class std::vector<cv::Mat>;
// template class std::vector<midproj::SculptureSlice>;
#endif

bool assetCheck();
std::vector<cv::Mat> loadAllImages(const std::filesystem::path &allImageFolderPath, int32_t imageCount);

int main(int32_t argc, char** argv)
{
    const std::filesystem::path frameCorner2dFilePath("assets//FrameCornerCoordinates2d.xyz");
    const std::filesystem::path frameCorner3dFilePath("assets//FrameCornerCoordinates3d.xyz");
    const std::filesystem::path pathToAllScanImages("assets//ShadowStrip");
    const cv::Size2i IMG_SIZE(1080, 1080);
    constexpr int32_t SCAN_IMAGE_COUNT = 55;

    assetCheck();
    std::vector<cv::Mat> scanImages = loadAllImages(pathToAllScanImages, SCAN_IMAGE_COUNT);
    const cv::Mat fgMask = midproj::get_foreground_mask(scanImages, IMG_SIZE);

    // std::vector<midproj::SculptureSlice> scanSlices;
    // scanSlices.reserve(SCAN_IMAGE_COUNT);

    for (size_t imgIndex = 0; imgIndex < SCAN_IMAGE_COUNT; imgIndex++)
    {
        // midproj::SculptureSlice aSliceOfSculpture(scanImages.at(imgIndex), imgIndex);
        // scanSlices.push_back(aSliceOfSculpture);
        // scanSlices.at(imgIndex).get_red_scan_line(fgMask);
    }
    cv::Mat foregroundMask = midproj::get_foreground_mask(scanImages, IMG_SIZE);
    cv::imshow("foregroundMask", foregroundMask);
    std::vector<cv::Point2i> frameCorners2i = midproj::XyzIo::load_points_from_file_2i(frameCorner2dFilePath);
    std::vector<cv::Point2i> frameCorners3i = midproj::XyzIo::load_points_from_file_2i(frameCorner3dFilePath);

    cv::waitKey(0);
    return 0;
}

std::vector<cv::Mat> loadAllImages(const std::filesystem::path& allImageFolderPath, int32_t imageCount)
{
    std::vector<cv::Mat> images;
    images.reserve(imageCount);
    std::filesystem::path filePath;
    std::stringstream fileName;

    for (size_t i = 0; i < imageCount; i++)
    {
        fileName << std::setfill('0') << std::setw(4) << i << ".jpg";
        filePath = allImageFolderPath / fileName.str();
        images.push_back(cv::imread(filePath.string(), cv::IMREAD_COLOR));
        filePath.remove_filename();
        fileName.str(std::string());
    }
    return images;
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