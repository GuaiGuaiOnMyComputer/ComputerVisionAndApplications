#include <array>
#include <vector>
#include <forward_list>
#include <opencv2/opencv.hpp>

namespace finprj
{
    class FeatureMatching
    {
    public:
        static void find_corresponding_feature_point(const cv::Mat &leftImage, const cv::Mat &rightImage, const cv::Mat_<cv::Point> &pointLeft, cv::Mat_<cv::Point> &out_matchPointRight);
        static cv::Point find_corresponding_feature_point(const cv::Mat &leftImage, const cv::Mat &rightImage, const cv::Point &pointLeft);
        static cv::Vec3d get_epipolar_line_right_image(const cv::Point2d &pointLeft, const cv::Mat &fundementalMatrix = s_FundementalMatrix);
        static cv::Rect get_feature_matching_region(const cv::Mat &rightImage, const cv::Vec3d &epipolarLineRightImage, const int64_t minRoiHeight);
        static cv::Rect get_feature_matching_region(const cv::Size &rightImageSize, const cv::Vec3d &epipolarLineRightImage, const int64_t minRoiHeight);
        static cv::Mat draw_matching_points(const cv::Mat &leftImage, const cv::Mat &rightImage, const cv::Point& pointLeft, const cv::Point& pointRight);
        static cv::Mat draw_matching_points(const cv::Mat &image, const cv::Mat_<cv::Point> &pointsLeft, const cv::Mat_<cv::Point> &pointsRight);
        static cv::Mat draw_matching_points(const cv::Mat &image, const std::forward_list<const cv::Point*>& pointsLeft_ptrs, const std::forward_list<const cv::Point*>& pointsRight_ptrs);
        static bool check_if_on_epipolar_line(const cv::Point &pointRight, const cv::Vec3d &epipolarLineCoeff, const double threshold);
        static void reject_mismatched_point(cv::Point &in_out_projectedPoint, const cv::Vec3d &epipolarLineCoeff, const double threshold);
        static void reject_mismatched_point(std::vector<cv::Point> &in_out_projectedPoints, const cv::Vec3d &epipolarLineCoeff, const double threshold);
        static void remove_mismatched_point(const cv::Mat_<cv::Point> &pointLeft, const cv::Mat_<cv::Point> &pointRight, std::forward_list<const cv::Point *> &out_validPointLeft, std::forward_list<const cv::Point *> &out_validPointRight, size_t& out_validPointCount);

    private:
        static void _get_template(const cv::Mat &leftImage, const cv::Point& templateCenter, const int32_t templateSize, cv::Mat &out_template);
        static cv::Rect _get_roi_from_center_coor(const cv::Point &roiCenter, const int32_t roiSize, const int32_t maxX, const int32_t maxY);
        FeatureMatching() = delete;
        FeatureMatching(const FeatureMatching &) = delete;

    public:
        static const cv::Mat_<double> s_FundementalMatrix;
    };
}