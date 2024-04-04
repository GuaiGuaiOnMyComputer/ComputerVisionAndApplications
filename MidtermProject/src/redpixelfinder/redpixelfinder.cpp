#include <stdint.h>
#include <array>
#include <vector>
#include <opencv2/opencv.hpp>

namespace midproj
{
    /// @brief Finds the coordinate of red pixels in image. If a pixel's red value is greater than both its blue and green values, its coordinate will be stored into out_redPxCoors.
    /// @param image The image to be search.
    /// @param out_redPxCoors The coordinates of red pixels stored in a vector<Point>
    void find_red_pixels(cv::InputArray image, std::vector<cv::Point>& out_redPxCoors)
    {
        out_redPxCoors.reserve(1000);

        // subtract the image with its green and blue channel to get its red channel
        cv::Mat imgGray, foreGroundMask, imageForegroundOnly;
        cv::cvtColor(image, imgGray, cv::COLOR_BGR2GRAY);
        cv::threshold(imgGray, foreGroundMask, 2, 255, cv::THRESH_BINARY);
        cv::copyTo(image, imageForegroundOnly, foreGroundMask);

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
    }
}; // namespace midproj
