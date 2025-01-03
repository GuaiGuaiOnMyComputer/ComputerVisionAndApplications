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
        static std::vector<cv::Point2f> project_to_image(const std::vector<XyzIo::CoorAndNormal3D_f> &worldPt, const cv::Mat &projectionMatrix, bool includeOccluded, const cv::Vec3f& cameraPointingVtr, std::vector<XyzIo::CoorAndNormal3D_f>& out_nonOccludedPoints);
        static std::vector<XyzIo::Rgb_ui8> get_rgb_from_image(const std::vector<XyzIo::CoorAndNormal3D_f> &worldPt, const cv::Mat& projectionMat, const cv::Mat& image);
        static cv::Mat show_projected_points(const cv::Mat& image, const std::vector<cv::Point2f>& projectedPoints);
        static inline bool check_if_point_occluded(const XyzIo::CoorAndNormal3D_f &worldPt, const cv::Vec3f &cameraPointingVtr);
        static void save_image_with_selected_points(const fs::path& outputPath, const cv::Mat &refImage, const std::vector<XyzIo::Coor2D_f> &selectedPoints);
        static XyzIo::Rgb_ui8 get_rgb_from_image(const XyzIo::CoorAndNormal3D_f &worldPt, const cv::Mat &projectionMat, const cv::Mat& image);
        static std::vector<XyzIo::Rgb_ui8> get_rgb_from_2d_coordinate(const std::vector<cv::Point2f> &imagePoints, const cv::Mat& referenceImaeg);
        static XyzIo::Rgb_ui8 get_rgb_from_2d_coordinate(const cv::Point2f &imagePoint, const cv::Mat &referenceImage);
    };
}