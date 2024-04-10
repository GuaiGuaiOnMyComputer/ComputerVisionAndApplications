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
std::vector<cv::Mat> loadAllImages(const std::filesystem::path &allImageFolderPath, uint32_t imageCount);

int main(int32_t, char**)
{
    // Check if all of the require images are present
    // Load them all into cv::Mat or exit the program if they are not
    assetCheck(PATH_TO_ALL_SCAN_IMAGES, {FRAME_CORNER_2D_FILE_PATH, FRAME_CORNER_3D_FILE_PATH, SCULPTURE_ROI_FILE_PATH});
    std::vector<cv::Mat> scanImages = loadAllImages(PATH_TO_ALL_SCAN_IMAGES, SCAN_IMAGE_COUNT);

    // A Binary image where the sculpture and the frames are true, and false anywhere else.
    const cv::Mat foregroundMask = midproj::get_foreground_mask(scanImages, IMG_SIZE);

    // Vector of binarized scan images in which the red pixels are true and anywhere else false.
    std::vector<cv::Mat> redPixelMaps(SCAN_IMAGE_COUNT);
    for (size_t imgIndex = 0; imgIndex < SCAN_IMAGE_COUNT; imgIndex++)
    {
        redPixelMaps.at(imgIndex) = midproj::get_red_pixel_map(scanImages.at(imgIndex), foregroundMask);
        scanImages.at(imgIndex).release(); // dispose the scanned image Mat object to save memory.
    }

    // A binary image in which everywhere the red scan line can reach is true, and anywhere else false.
    const cv::Mat scannedAreaMask = midproj::get_scanned_area_mask(redPixelMaps, foregroundMask, IMG_SIZE);

    const cv::Mat sculptureAreaMask = midproj::get_sculpture_area_mask(IMG_SIZE, SCULPTURE_ROI);

    // A binary image in which everywhere on the sculpture that the red scan line can reach is true, and anywhere else false.
    const cv::Mat scannedSculptureMask = midproj::get_scanned_sculpture_mask(scannedAreaMask, sculptureAreaMask);

    // A binary image in which everywhere on the frame that the red scan line can reach is true, and anywhere else false.
    const cv::Mat scannedFrameMask = midproj::get_scanned_frame_mask(scannedAreaMask, sculptureAreaMask);

    // Coordinates of the frame corners in the image coordinate system. Values measured using photo editing software.
    const std::vector<cv::Point2i> frameCornerCoorsImage2i = midproj::XyzIo::load_points_from_file_2i(FRAME_CORNER_2D_FILE_PATH);

    // Coordinates of the frame corners in the world coordinate system. Values given from homework instructions.
    const std::vector<cv::Point3i> frameCornerCoorsWorld3i = midproj::XyzIo::load_points_from_file_3i(FRAME_CORNER_3D_FILE_PATH);

    midproj::SliceTransform::load_frame_corners_from_vector(frameCornerCoorsImage2i, frameCornerCoorsWorld3i);
    midproj::SliceTransform::fit_frame_beam_lines();
    midproj::SliceTransform::init_front_panel_homography();

    // A vector of vectors containing the sculptures's red pixels' world coordinates.
    // The inner vector contains the red pixels' world coordinate and the outer vector contains such information derived from each scan images.
    std::vector<std::vector<cv::Point3d>> sculpturePointCloudWorldInEachSlice(SCAN_IMAGE_COUNT);

    // A Binary image where the red pixels in a scan image on the frame beams are true, and false anywhere else.
    cv::Mat onBeamRedPixelMap;
    // A Binary image where the red pixels in a scan image on the sculpture are true, and false anywhere else.
    cv::Mat sculptureRedPixelMap;
    for (size_t imgIndex = 0; imgIndex < SCAN_IMAGE_COUNT; imgIndex++)
    {
        cv::bitwise_and(scannedFrameMask, redPixelMaps.at(imgIndex), onBeamRedPixelMap);
        std::array<cv::Point2f, midproj::SliceTransform::BEAM_COUNT> avgRedPixelOnEachBeam = midproj::SliceTransform::get_red_pixels_on_frame(onBeamRedPixelMap);
        cv::bitwise_and(scannedSculptureMask, redPixelMaps.at(imgIndex), sculptureRedPixelMap);

        midproj::SliceTransform::refine_sculpture_pixel_map(sculptureRedPixelMap, 2);
        sculpturePointCloudWorldInEachSlice.at(imgIndex) = midproj::SliceTransform::get_slice_world_coordinate(avgRedPixelOnEachBeam, sculptureRedPixelMap);
    }
    const bool fileOutputStatus = midproj::XyzIo::write_xyz_point_cloud_file(OUTPUT_XYZ_FILE, sculpturePointCloudWorldInEachSlice);
    if (!fileOutputStatus)
        std::cout << "File output error." << '\n';
    else
        std::cout << "Xyz file has been written to " << std::filesystem::absolute(OUTPUT_XYZ_FILE) << '\n';
    return 0;
}

/// @brief Load all of the 3-channel scan images into a vector.
/// @param allImageFolderPath The path to folder containing all of the scan images.
/// @param imageCount Number of images to load.
/// @return A vector containing all of the 3-channel scan images.
std::vector<cv::Mat> loadAllImages(const std::filesystem::path& allImageFolderPath, uint32_t imageCount)
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
    filePath /= pathToImageAssetFolder;
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

    if ((missingImageList.size() != 0) | (missingXyzFileList.size() != 0))
        exit(-1);

    return true;
}