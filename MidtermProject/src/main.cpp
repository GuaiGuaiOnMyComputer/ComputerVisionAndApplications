#include "masking.hpp"
#include "xyzio.hpp"
#include "slicetransform.hpp"
#include "predefines.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <stdint.h>
#include <filesystem>
#include <iostream>
#include <sstream>


bool assetCheck(const std::filesystem::path &pathToImageAssetFolder, const std::vector<std::filesystem::path> &pathToEachXyzAssets);
std::vector<cv::Mat> loadAllImages(const std::filesystem::path &allImageFolderPath, int32_t imageCount);

int main(int32_t argc, char** argv)
{
    assetCheck(PATH_TO_ALL_SCAN_IMAGES, {FRAME_CORNER_2D_FILE_PATH, FRAME_CORNER_3D_FILE_PATH, SCULPTURE_ROI_FILE_PATH});
    std::vector<cv::Mat> scanImages = loadAllImages(PATH_TO_ALL_SCAN_IMAGES, SCAN_IMAGE_COUNT);
    std::vector<cv::Mat> redPixelMaps(SCAN_IMAGE_COUNT);

    const cv::Mat foregroundMask = midproj::get_foreground_mask(scanImages, IMG_SIZE);
    for (size_t imgIndex = 0; imgIndex < SCAN_IMAGE_COUNT; imgIndex++)
        redPixelMaps.at(imgIndex) = (midproj::get_red_pixel_map(scanImages.at(imgIndex), foregroundMask));

    const cv::Mat scannedAreaMask = midproj::get_scanned_area_mask(redPixelMaps, foregroundMask, IMG_SIZE);
    const cv::Mat scannedSculptureMask = midproj::get_scanned_sculpture_mask(scannedAreaMask, SCULPTURE_ROI);
    const cv::Mat scannedFrameMask = midproj::get_scanned_frame_mask(scannedAreaMask, SCULPTURE_ROI);
    const std::vector<cv::Point2i> frameCorners2i = midproj::XyzIo::load_points_from_file_2i(FRAME_CORNER_2D_FILE_PATH);
    const std::vector<cv::Point2i> frameCorners3i = midproj::XyzIo::load_points_from_file_2i(FRAME_CORNER_3D_FILE_PATH);

    midproj::SliceTransform::load_frame_corners_from_vector(frameCorners2i);
    midproj::SliceTransform::fit_frame_beam_lines();
    for (size_t imgIndex = 0; imgIndex < SCAN_IMAGE_COUNT; imgIndex++)
    {
        cv::bitwise_and(scannedFrameMask, redPixelMaps.at(imgIndex), redPixelMaps.at(imgIndex));
        std::array<cv::Point2f, midproj::SliceTransform::BEAM_COUNT> avgRedPixelOnEachBeam = midproj::SliceTransform::get_red_pixels_on_frame(redPixelMaps.at(imgIndex));
    }
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