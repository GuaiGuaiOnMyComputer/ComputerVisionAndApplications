#include <stdint.h>
#include <array>
#include <vector>
#include <opencv2/opencv.hpp>

namespace midproj
{
    namespace fs = std::filesystem;

    /// @brief Finds the coordinate of red pixels in image. If a pixel's red value is greater than both its blue and green values, its coordinate will be stored into out_redPxCoors.
    /// @param image The image to be search.
    /// @param out_redPxCoors The coordinates of red pixels stored in a vector<Point>
    cv::OutputArray find_red_pixels(cv::InputArray image, cv::InputArray foregroundMask, std::vector<cv::Point>& out_redPxCoors)
    {
        out_redPxCoors.reserve(1000);
        cv::Mat imageForegroundOnly;
        cv::copyTo(image, imageForegroundOnly, foregroundMask);

        cv::Mat imgRedOnly; // mask where only the red pixels are true
        cv::Mat imgRedCh, imgBlueCh, imgGreenCh;
        cv::extractChannel(imageForegroundOnly, imgBlueCh, 0); // extract the blue channel
        cv::extractChannel(imageForegroundOnly, imgGreenCh, 1); // extract the green channel
        cv::extractChannel(imageForegroundOnly, imgRedCh, 2); // extract the green channel

        cv::compare(imgRedCh, imgBlueCh, imgBlueCh, cv::CMP_GT);
        cv::compare(imgBlueCh, imgGreenCh, imgRedCh, cv::CMP_GT);

        // remove noise and rough dots from imgRedCh using erode
        // and reconnect broken lines using dilate
        cv::Mat morphKernel = cv::getStructuringElement(cv::MorphShapes::MORPH_CROSS, cv::Size2i(3, 3));
        cv::erode(imgRedCh, imgRedCh, morphKernel, cv::Point(-1, -1), 2);
        morphKernel = cv::getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size2i(3, 3));
        cv::dilate(imgRedCh, imgRedCh, morphKernel, cv::Point2i(-1, -1), 1);
        morphKernel = cv::getStructuringElement(cv::MorphShapes::MORPH_CROSS, cv::Size2i(3, 3));
        cv::erode(imgRedCh, imgRedCh, morphKernel, cv::Point2i(-1, -1), 1);

        cv::findNonZero(imgRedCh, out_redPxCoors);
        return imgRedCh;
    }

    /// @brief Acquire a binary image where the frames and the sculpture areas are white and everywhere else is black.
    /// @param images All of the 3-channel images loaded from file and stored into a vector.
    /// @param imageSize Size of the images
    /// @return The foreground mask.
    cv::OutputArray get_foreground_mask(const std::vector<cv::Mat> images, const cv::Size2i imageSize)
    {
        cv::Mat imgGray;
        cv::Mat foregroundMask = cv::Mat(imageSize, CV_8UC1, cv::Scalar(0));
        for (cv::InputArray img : images)
        {
            cv::cvtColor(images, imgGray, cv::COLOR_BGR2GRAY);
            cv::threshold(imgGray, imgGray, 2, 255, cv::THRESH_BINARY);
            cv::bitwise_or(foregroundMask, imgGray, foregroundMask);
        }
        return foregroundMask;
    }

    /// @brief Acquire a binary image where all the pixels that have been swept by the red scan lines are white, and everywhere else black
    /// @param allScanProfiles All of the scan profiles as binary images stored in a vector.
    /// @return The scanning area mask.
    cv::OutputArray get_scan_area_mask(const std::vector<cv::Mat>& allScanProfiles, const cv::Size2i& imageSize)
    {
        cv::Mat scanAreaMask = cv::Mat(imageSize, CV_8UC1, cv::Scalar(0));
        for(const cv::Mat& scanProfile : allScanProfiles)
        {
            if (scanProfile.size() != scanAreaMask.size())
                throw std::runtime_error("The image size of scanProfile does not match scanAreaMask");
            cv::bitwise_or(scanAreaMask, scanProfile, scanAreaMask);
        }
        return scanAreaMask;
    }

    void get_frame_mask()
    {

    }

    void get_sculpture_mask()
    {

    }
}; // namespace midproj
