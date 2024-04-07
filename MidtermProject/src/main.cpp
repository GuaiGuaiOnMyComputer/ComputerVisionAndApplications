#include "masking.hpp"
#include "xyzio.hpp"
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
#endif

bool assetCheck(const std::filesystem::path &pathToImageAssetFolder, const std::vector<std::filesystem::path> &pathToEachXyzAssets);
std::vector<cv::Mat> loadAllImages(const std::filesystem::path &allImageFolderPath, int32_t imageCount);

int main(int32_t argc, char** argv)
{
    const std::filesystem::path frameCorner2dFilePath("assets//FrameCornerCoordinates2d.xyz");
    const std::filesystem::path frameCorner3dFilePath("assets//FrameCornerCoordinates3d.xyz");
    const std::filesystem::path sculptureCornerRoiFilePath("assets//SculptureRoi.xyz");
    const std::filesystem::path pathToAllScanImages("assets//ShadowStrip");
    const cv::Size2i IMG_SIZE(1080, 1080);
    const cv::Rect2i SCULPTURE_ROI(379, 263, 220, 417);
    constexpr int32_t SCAN_IMAGE_COUNT = 55;

    assetCheck(pathToAllScanImages, {frameCorner2dFilePath, frameCorner3dFilePath, sculptureCornerRoiFilePath});
    std::vector<cv::Mat> scanImages = loadAllImages(pathToAllScanImages, SCAN_IMAGE_COUNT);
    std::vector<cv::Mat> redPixelMaps;
    std::vector<std::vector<cv::Point2i>> allRedPointCoors;
    redPixelMaps.reserve(SCAN_IMAGE_COUNT);
    allRedPointCoors.reserve(SCAN_IMAGE_COUNT);

    const cv::Mat foregroundMask = midproj::get_foreground_mask(scanImages, IMG_SIZE);
    for (size_t imgIndex = 0; imgIndex < SCAN_IMAGE_COUNT; imgIndex++)
    {
        allRedPointCoors.emplace_back();
        allRedPointCoors.at(imgIndex).reserve(800);
        redPixelMaps.push_back(midproj::find_red_pixels(scanImages.at(imgIndex), foregroundMask, allRedPointCoors.at(imgIndex)));
    }

    const cv::Mat scannedAreaMask = midproj::get_scanned_area_mask(redPixelMaps, foregroundMask, IMG_SIZE);
    const cv::Mat scannedSculptureMask = midproj::get_scanned_sculpture_mask(scannedAreaMask, SCULPTURE_ROI);
    const cv::Mat scannedFrameMask = midproj::get_scanned_frame_mask(scannedAreaMask, SCULPTURE_ROI);
    std::vector<cv::Point2i> frameCorners2i = midproj::XyzIo::load_points_from_file_2i(frameCorner2dFilePath);
    std::vector<cv::Point2i> frameCorners3i = midproj::XyzIo::load_points_from_file_2i(frameCorner3dFilePath);

    cv::imshow("Foreground Mask", foregroundMask);
    cv::imshow("Scanned Area Mask", scannedAreaMask);
    cv::imshow("scanned Sculpture Mask", scannedSculptureMask);
    cv::imshow("scanned frame mask", scannedFrameMask);
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
bool assetCheck(const std::filesystem::path& pathToImageAssetFolder, const std::vector<std::filesystem::path>& pathToEachXyzAssets)
{
    namespace fs = std::filesystem;

    // check for all 54 images
    std::vector<fs::path> missingImageList;
    std::stringstream fileName;
    fs::path filePath;
    filePath /= "assets//ShadowStrip";
    for (size_t i = 0; i < 54; i++)
    {
        fileName << std::setfill('0') << std::setw(4) << i << ".jpg";
        filePath /= fileName.str();
        if (!fs::exists(filePath))
            missingImageList.push_back(fs::absolute(filePath));
        filePath.remove_filename();
        fileName.str(std::string());
    }
    for (const fs::path& missingFilePath : missingImageList)
        std::cout << "The image file at " << fs::absolute(missingFilePath) << " is missing.";

    std::vector<fs::path> missingXyzFileList;
    for (const fs::path& xyzAssetPath : pathToEachXyzAssets)
    {
        if (!fs::exists(xyzAssetPath))
            missingXyzFileList.push_back(xyzAssetPath);
    }
    for (const fs::path& missingFilePath : missingXyzFileList)
        std::cout << "The necessary xyz file " << fs::absolute(missingFilePath) << " is missing.";

    if (missingImageList.size() != 0 | missingXyzFileList.size() != 0)
        exit(-1);

    return true;
}