#include <array>
#include <vector>
#include <opencv2/opencv.hpp>

namespace finprj
{
    class FeatureMatching
    {
    public:
        static void find_corresponding_feature_point(const cv::Mat &leftImage, const cv::Mat &rightImage, const cv::Mat_<cv::Point> &pointLeft, cv::Mat_<cv::Point> &out_matchPointRight);
        static cv::Point find_corresponding_feature_point(const cv::Mat &leftImage, const cv::Mat &rightImage, const cv::Point &pointLeft);
        static cv::Vec3d get_epipolar_line_right_image(const cv::Point2d &pointLeft, const cv::Mat &fundementalMatrix = s_FundementalMatrix);
        static cv::Rect get_feature_matching_region(const cv::Mat &rightImage, const cv::Vec3d &epipolarLineRightImage);
        static cv::Mat draw_matching_ponits(const cv::Mat &leftImage, const cv::Mat &rightImage, const cv::Point& pointLeft, const cv::Point& pointRight);

    private:
        static void _get_template(const cv::Mat &leftImage, const cv::Point& templateCenter, const int32_t templateSize, cv::Mat &out_template);
        FeatureMatching() = delete;
        FeatureMatching(const FeatureMatching &) = delete;

    public:
        static const cv::Mat_<double> s_FundementalMatrix;
    };
}