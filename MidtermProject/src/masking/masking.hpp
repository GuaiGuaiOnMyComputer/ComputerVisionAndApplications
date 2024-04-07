#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

namespace midproj
{
    cv::Mat find_red_pixels(cv::InputArray image, cv::InputArray foregroundMask, std::vector<cv::Point>& out_redPxCoors);
    cv::Mat get_foreground_mask(const std::vector<cv::Mat> &images, const cv::Size2i imageSize);
    cv::Mat get_scanned_area_mask(const std::vector<cv::Mat> &allScanProfiles, cv::InputArray foregroundMask, const cv::Size2i &imageSize);
    cv::Mat get_scanned_sculpture_mask(cv::InputArray scanAreaMask, const cv::Rect2i &sculptureArea);
    cv::Mat get_scanned_frame_mask(cv::InputArray scanAreaMask, const cv::Rect2i &sculptureArea);
}