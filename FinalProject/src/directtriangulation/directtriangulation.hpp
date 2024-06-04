#include <opencv2/opencv.hpp>
#include <array>
#include <vector>

namespace finprj
{
    class DirectTriangulation
    {
    public:
        DirectTriangulation(
            const std::array<double, 3 * 4> &leftRt, const std::array<double, 3 * 4> &rightRt, 
            const std::array<double, 3 * 3> &leftK, const std::array<double, 3 * 3>& rightK, 
            const std::array<double, 3 * 3>& fundementalMatrix) noexcept;
        DirectTriangulation(const std::array<double, 4 * 3> &leftP, const std::array<double, 4 * 3> &rightP) noexcept;

        DirectTriangulation() = delete;
        DirectTriangulation(DirectTriangulation &&) = delete;

        cv::Point3d LocalToWorld(const cv::Point2d& pointLeft, const cv::Point2d& pointRight);
        std::vector<cv::Point3d> LocalToWorld(const std::vector<cv::Point2d> &pointsLeft, const std::vector<cv::Point2d> &pointsRight);

    private:
        const cv::Mat _leftRt;
        const cv::Mat _rightRt;
        const cv::Mat _leftK;
        const cv::Mat _rightK;
        const cv::Mat _leftP;
        const cv::Mat _rightP;
        const cv::Mat _fundementalMatrix;

    private:
        inline static void _get_uvp_mat(const cv::Point2d& pointLeft, const cv::Point2d& pointRight, const cv::Mat &pLeft, const cv::Mat &pRight, cv::Mat& out_uvpMat);
    };
}