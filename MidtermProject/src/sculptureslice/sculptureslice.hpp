#pragma once

#include <opencv2/opencv.hpp>
#include <filesystem>
#include <string.h>

namespace midproj
{
    namespace fs = std::filesystem;

    class SculptureSlice
    {
    public:

        SculptureSlice(const cv::Mat& scanImage, const int32_t imgIndex);
        SculptureSlice();
        void get_red_scan_line(cv::InputArray foregroundMask);
        virtual ~SculptureSlice();
    private:
        std::vector<cv::Point2i> _redPixelCoordinates;
        cv::Mat _redPixelMap;
        const int32_t _imageIndex;
        const cv::Mat _scanImage;
    };
} // namespace midproj
