#include <opencv2/opencv.hpp>
#include <filesystem>
#include <string>
#include <stdint.h>
#include "scanimageio.hpp"

namespace finprj
{
    namespace fs = std::filesystem;

    ImagePair::ImagePair() noexcept : 
        FilePath{fs::path("NoImage")}, Image{cv::Mat()}, 
        LeftRoi{cv::Rect()}, RightRoi{cv::Rect()},
        Left{cv::Mat()}, Right{cv::Mat()}
    {
    }

    ImagePair::ImagePair(const cv::Mat &image, const fs::path &filepath) noexcept: 
        FilePath{filepath}, Image{image},
        LeftRoi{cv::Rect(0, 0, Image.cols / 2, Image.rows)}, RightRoi{cv::Rect(Image.cols / 2 - 1, 0, Image.cols / 2, Image.rows)},
        Left{Image(LeftRoi)}, Right{Image(RightRoi)}
    {
    }

    ScanImageIo::ScanImageIo(const std::filesystem::path &imagePathRoot, const std::string &extension, std::error_code& filesystemErrorCode) noexcept 
        : _imageCount{0}, _currentImagePair{0},
         _imageExtension{extension}, _imagePathRoot{fs::absolute(imagePathRoot)}, _pathIsValid{fs::is_directory(_imagePathRoot)}
    {
        const fs::directory_iterator imagePathRootIterator(_imagePathRoot, filesystemErrorCode);
        for(const fs::directory_entry& itemInDir : imagePathRootIterator)
        {
            if (extension == itemInDir.path().extension())
                _imageCount++;
        }
    }

    ImagePair ScanImageIo::GetNextPair()
    {
        if (_currentImagePair > _imageCount)
            return ImagePair();
        const std::string fileName = ScanImageIo::_get_image_pair_file_name(_currentImagePair, _imageExtension);
        const fs::path filePath = _imagePathRoot / fileName;
        return ImagePair(cv::imread(filePath.string()), fileName);
    }

    ImagePair ScanImageIo::GetPairByIndex(const int32_t imagePairIndex)
    {
        const std::string fileName = ScanImageIo::_get_image_pair_file_name(imagePairIndex, _imageExtension);
        const fs::path filePath = _imagePathRoot / fileName;
        return ImagePair(cv::imread(filePath.string()), fileName);
    }

    size_t ScanImageIo::GetImageCount() 
    {
        return this -> _imageCount;
    }

    /// @brief A pixel is considered on the blue scan line if: 1. its brightness is greater than 15, 2. blue value/bluePixelValueMultiple is greater than green value, 3. blue value / 1.1 is greater than red value.
    /// @param image the image to identify blue pixels in
    /// @param outputMask a binary image where blue pixels are true and everywhere else false
    /// @param bluePixelValueMultiple a predefined value to scale the blue channel before comparision
    void ScanImageIo::get_blue_pixel_mask(const cv::Mat& image, cv::Mat &outputMask, const cv::Mat &predefinedRoiMask, const bool usePredefineRoiMask, const float bluePixelValueMultiple)
    {
        cv::Mat imageBlueChannel, imageGreenChannel, imageRedChannel;

        // the pixels in image with brightness value above a predefined value will be a true pixel in brightnessMask 
        cv::Mat brightnessMask;
        cv::cvtColor(image, brightnessMask, cv::COLOR_BGR2GRAY);
        cv::compare(brightnessMask, cv::Scalar(15), brightnessMask, cv::CMP_GT);

        cv::extractChannel(image, imageBlueChannel, 0);  // extract the blue channel of image into imageBlueChannel
        cv::extractChannel(image, imageGreenChannel, 1); // extract the red channel of image into imageRedChannel
        cv::extractChannel(image, imageRedChannel, 2);   // extract the green channel of image into imageGreenChannel

        // assume pixel with b/1.1 > g && b/1.1 > a is a pixel on the blue scan line
        cv::Mat blueGreaterThanRedMask, blueGreaterThanGreenMask;
        imageBlueChannel = imageBlueChannel.mul(cv::Scalar(1 / bluePixelValueMultiple));
        cv::compare(imageBlueChannel, imageRedChannel, blueGreaterThanRedMask, cv::CMP_GT);
        cv::compare(imageBlueChannel, imageGreenChannel, blueGreaterThanGreenMask, cv::CMP_GT);

        cv::bitwise_and(brightnessMask, blueGreaterThanGreenMask, brightnessMask);
        cv::bitwise_and(brightnessMask, blueGreaterThanRedMask, outputMask);
        if (usePredefineRoiMask)
            cv::bitwise_and(outputMask, predefinedRoiMask, outputMask);
    }

    void ScanImageIo::init_roi_mask(const cv::Rect &scanObjectRoiLeft, const int64_t scanImageWidth, const int64_t scanImageHeight)
    {
        s_ModelRoi = scanObjectRoiLeft;
        s_ModelRoiMask = cv::Mat(scanImageHeight, scanImageWidth, CV_8UC1, cv::Scalar(1));
        s_ModelRoiMask(cv::Range::all(), cv::Range(0, scanImageWidth / 2)).setTo(cv::Scalar(0));
        s_ModelRoiMask(s_ModelRoi).setTo(cv::Scalar(1));
    }

    void ScanImageIo::get_blue_pixel_coors(const cv::Mat& bluePixelMask, std::vector<cv::Point2i> &out_bluePixels)
    {
        cv::findNonZero(bluePixelMask, out_bluePixels);
    }

    void ScanImageIo::get_blue_pixel_coors(const cv::Mat& bluePixelMask, cv::Mat_<cv::Point> &out_bluePixels)
    {
        cv::findNonZero(bluePixelMask, out_bluePixels);
    }

    std::string ScanImageIo::_get_image_pair_file_name(int32_t imagePairIndex, const std::string &imageExtension)
    {
        uint8_t prependingZeroCount{0};
        if (imagePairIndex > 99)
            prependingZeroCount = 0;
        else if(imagePairIndex <= 99 && imagePairIndex > 9)
            prependingZeroCount = 1;
        else
            prependingZeroCount = 2;
        return std::string(prependingZeroCount, '0') + std::to_string(imagePairIndex) + imageExtension;
    }

    cv::Mat ScanImageIo::s_ModelRoiMask;
    cv::Rect ScanImageIo::s_ModelRoi;
}