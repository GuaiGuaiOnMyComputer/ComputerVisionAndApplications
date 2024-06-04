#include <array>
#include <vector>
#include <opencv2/opencv.hpp>

namespace finprj
{
    class FeatureMatching
    {
    public:
        static cv::Vec3d get_epipolar_line_right_image(const cv::Point2d &pointLeft, const cv::Mat &fundementalMatrix = s_FundementalMatrix);

    public:
        static const cv::Mat_<double> s_FundementalMatrix;
    };
}