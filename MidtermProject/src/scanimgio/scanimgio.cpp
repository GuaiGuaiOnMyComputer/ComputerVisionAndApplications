#include <opencv2/opencv.hpp>
#include <filesystem>
#include <iostream>
#include <vector>
#include "scanimgio.hpp"

namespace midproj
{
    
    /// @brief Load all of the 3-channel scan images into a vector.
    /// @param allImageFolderPath The path to folder containing all of the scan images.
    /// @param imageCount Number of images to load.
    /// @return A vector containing all of the 3-channel scan images.
    std::vector<cv::Mat> ScanImageIo::load_all_images(const std::filesystem::path& allImageFolderPath, uint32_t imageCount)
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
    bool ScanImageIo::asset_check(const std::filesystem::path& pathToImageAssetFolder, const std::vector<std::filesystem::path>& pathToEachXyzAssets)
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
} //namespace midproj