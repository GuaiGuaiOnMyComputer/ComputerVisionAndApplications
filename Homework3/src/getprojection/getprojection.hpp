#include <opencv2/opencv.hpp>
#include <vector>

namespace hw3
{
    class GetProjection
    {
    public:
        static cv::Mat get_puv_mat(const std::vector<cv::Point3f>& objectPoints, const std::vector<cv::Point2f>& imagePoints);
    };
}