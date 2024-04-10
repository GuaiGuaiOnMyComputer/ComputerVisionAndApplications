#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

namespace midproj
{
    cv::Mat get_red_pixel_map(cv::InputArray image, cv::InputArray foregroundMask);
    cv::Mat get_foreground_mask(const std::vector<cv::Mat> &images, const cv::Size2i imageSize);
    cv::Mat get_scanned_area_mask(const std::vector<cv::Mat> &allScanProfiles, cv::InputArray foregroundMask, const cv::Size2i &imageSize);
    cv::Mat get_scanned_sculpture_mask(cv::InputArray scannedAreaMask, cv::InputArray sculptureAreaMask);
    cv::Mat get_scanned_frame_mask(cv::InputArray scannedAreaMask, cv::InputArray sculptureAreaMask);
    cv::Mat get_sculpture_area_mask(const cv::Size2i& imageSize, const cv::Rect2i& sculptureRoi);
}