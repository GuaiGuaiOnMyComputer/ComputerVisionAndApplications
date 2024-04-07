#include <opencv2/opencv.hpp>
#include "slicetransform.hpp"

namespace midproj
{
    std::vector<cv::Point2f> SliceTransform::get_red_pixels_on_frame(cv::InputArray redPixelMap)
    {
        std::vector<cv::Point2i> redPixelCoors;
        redPixelCoors.reserve(500);

        cv::findNonZero(redPixelMap, redPixelCoors);
    }
} // namespace midproj
