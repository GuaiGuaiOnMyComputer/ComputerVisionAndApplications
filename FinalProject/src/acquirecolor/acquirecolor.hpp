#include <opencv2/opencv.hpp>
#include "xyzio.hpp"

namespace finprj
{
    class AcquireColor
    {
    public:
        static XyzIo::Rgb_ui8 get_rgb_from_right_image(const cv::Mat &projectionMatrixRight, const cv::Mat &rightImage, const cv::Point3d &worldPoint);
        static std::vector<XyzIo::Rgb_ui8> get_rgb_from_right_image(const cv::Mat &projectionMatrixRight, const cv::Mat &rightImage, const std::vector<cv::Point3d> &worldPoints);
        static std::vector<std::vector<XyzIo::Rgb_ui8>> get_rgb_from_right_image(const cv::Mat &projectionMatrixRight, const cv::Mat &rightImage, const std::vector<std::vector<cv::Point3d>> &worldPoints);
    };
}