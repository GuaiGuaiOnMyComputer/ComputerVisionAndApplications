#include <stdint.h>
#include <array>
#include <vector>
#include <opencv2/opencv.hpp>

namespace midproj
{
    namespace fs = std::filesystem;

    /// @brief Returns binarized image where pixels whose R values are larger than B and G values are true.
    /// @param image The image to be binarized.
    cv::Mat get_red_pixel_map(cv::InputArray image, cv::InputArray foregroundMask)
    {
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

        return imgRedCh;
    }

    /// @brief Acquire a binary image where the frames and the sculpture areas are true and everywhere else is false.
    /// @param images All of the 3-channel images loaded from file and stored into a vector.
    /// @param imageSize Size of the images
    /// @return The foreground mask.
    cv::Mat get_foreground_mask(const std::vector<cv::Mat>& images, const cv::Size2i imageSize)
    {
        cv::Mat imgGray;
        cv::Mat foregroundMask = cv::Mat(imageSize, CV_8UC1, cv::Scalar(0));
        for (const cv::Mat& img : images)
        {
            cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);
            cv::threshold(imgGray, imgGray, 2, 255, cv::THRESH_BINARY);
            cv::bitwise_or(foregroundMask, imgGray, foregroundMask);
        }
        cv::Mat erosionKernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size2i(3, 3));
        cv::erode(foregroundMask, foregroundMask, erosionKernel, cv::Point2i(-1, -1), 2);
        return foregroundMask;
    }

    /// @brief Acquire a binary image where all the pixels that have been swept by the red scan lines are true, and everywhere else false
    /// @param allScanProfiles All of the scan profiles as binary images stored in a vector.
    /// @param foregroundMask Binary image where both the frame and the sculpture area are true, and everywhere else false.
    /// @return The scanning area mask.
    cv::Mat get_scanned_area_mask(const std::vector<cv::Mat>& allScanProfiles, cv::InputArray foregroundMask, const cv::Size2i& imageSize)
    {
        cv::Mat scannedAreaMask = cv::Mat(imageSize, CV_8UC1, cv::Scalar(0));
        cv::Mat tmp = cv::Mat(imageSize, CV_8UC1, cv::Scalar(0));
        for(const cv::Mat& scanProfile : allScanProfiles)
        {
            cv::bitwise_and(foregroundMask, scanProfile, tmp);
            cv::bitwise_or(tmp, scannedAreaMask, scannedAreaMask);
        }
        cv::Mat dialiateKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size2i(3, 3));
        cv::dilate(scannedAreaMask, scannedAreaMask, dialiateKernel);
        return scannedAreaMask;
    }

    /// @brief Acquire a binary image where all the pixels on the sculpture that have been swept by the red scan lines are true, and everywhere else false.
    /// @param scannedAreaMask A binary image where all the red pixels from all the scan images combined are true, and everywhere else false. Acquired from function get_scanned_area_mask.
    /// @param sculptureArea Predefined rectangular area bounding the sculpture in every scan images.
    /// @return The scanned sculpture mask.
    cv::Mat get_scanned_sculpture_mask(cv::InputArray scannedAreaMask, const cv::Rect2i& sculptureArea)
    {
        cv::Mat sculptureAreaMask = cv::Mat(scannedAreaMask.size(), CV_8UC1, cv::Scalar(0));
        sculptureAreaMask(sculptureArea).setTo(255);
        cv::bitwise_and(sculptureAreaMask, scannedAreaMask, sculptureAreaMask);
        return sculptureAreaMask;
    }

    /// @brief Acquire a binary image where all the pixels on the frame that have been swept by the red scan lines are true, and everywhere else false.
    /// @param scannedAreaMask A binary image where all the red pixels from all the scan images combined are true, and everywhere else false. Acquired from function get_scanned_area_mask.
    /// @param sculptureArea Predefined rectangular area bounding the sculpture in every scan images.
    /// @return The scanned sculpture mask.
    cv::Mat get_scanned_frame_mask(cv::InputArray scannedAreaMask, const cv::Rect2i& sculptureArea)
    {
        cv::Mat scannedFrameAreaMask = cv::Mat(scannedAreaMask.size(), CV_8UC1, cv::Scalar(255));
        scannedFrameAreaMask(sculptureArea).setTo(0);
        cv::bitwise_and(scannedFrameAreaMask, scannedAreaMask, scannedFrameAreaMask);
        return scannedFrameAreaMask;
    }
}; // namespace midproj
