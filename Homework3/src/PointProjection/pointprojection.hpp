#include <opencv2/opencv.hpp>
#include <vector>
#include "xyzio.hpp"

namespace hw3
{
    class PointProjection
    {
    public:
        static cv::Mat get_puv_mat(const std::vector<hw3::XyzIo::Coor3D_f>& objectPoints, const std::vector<hw3::XyzIo::Coor2D_f>& imagePoints);
        static cv::Mat get_projection_mat(const std::vector<hw3::XyzIo::Coor3D_f> &objectPoints, const std::vector<hw3::XyzIo::Coor2D_f> &imagePoints);
        static cv::Mat get_projection_mat(cv::Mat& puvMat);
        static cv::Point3f project_to_world(const cv::Point2f &pt);
    };
}