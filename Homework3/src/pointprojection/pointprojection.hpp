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
        static cv::Point2f project_to_image(const cv::Point3f &worldPt, const cv::Mat& projectionMatrix);
        static void project_to_image(const cv::Point3f &worldPt, cv::Point2f &out_imgPoint, const cv::Mat &projectionMatrix);
        static std::vector<cv::Point2f> project_to_image(const std::vector<XyzIo::CoorAndNormal3D_f> &worldPt, const cv::Mat& projectionMatrix);
        static cv::Mat show_projected_points(const cv::Mat& image, std::vector<cv::Point2f>& projectedPoints);
    };
}