#include <opencv2/opencv.hpp>
#include <forward_list>
#include <array>
#include <vector>

namespace finprj
{
    class DirectTriangulation
    {
    public:
        DirectTriangulation(
            const std::array<double, 3 * 4> &leftRt, const std::array<double, 3 * 4> &rightRt, 
            const std::array<double, 3 * 3> &leftK, const std::array<double, 3 * 3>& rightK) noexcept;
        explicit DirectTriangulation(const std::array<double, 4 * 3> &leftP, const std::array<double, 4 * 3> &rightP) noexcept;

        DirectTriangulation() = delete;
        DirectTriangulation(DirectTriangulation &&) = delete;

        cv::Point2d WorldToLocal(const cv::Mat &cameraP, const cv::Point3d &worldPoint);
        cv::Point3d LocalToWorld(const cv::Point2d& pointLeft, const cv::Point2d& pointRight);
        std::vector<cv::Point3d> LocalToWorld(const std::vector<cv::Point2d> &pointsLeft, const std::vector<cv::Point2d> &pointsRight);
        std::vector<cv::Point3d> LocalToWorld(const cv::Mat_<cv::Point> &pointsLeft, const cv::Mat_<cv::Point> &pointsRight);
        std::vector<cv::Point3d> LocalToWorld(const std::forward_list<const cv::Point *>& pointsLeft, const std::forward_list<const cv::Point *>& pointsRight, const size_t pointCount);

        void FilterOutliners(const std::vector<cv::Point3d> &projectedWorldPoints, const cv::Range &xRange, const cv::Range &yRange, const cv::Range &zRange, std::forward_list<const cv::Point3d *>& in_out_validWorldPoints);

        const cv::Mat& GetRightP() const;
        const cv::Mat& GetLeftP() const;

    private:
        const cv::Mat _leftRt;
        const cv::Mat _rightRt;
        const cv::Mat _leftK;
        const cv::Mat _rightK;
        cv::Mat _leftP;
        cv::Mat _rightP;

    private:
        inline static void _get_uvp_mat(const cv::Point2d& pointLeft, const cv::Point2d& pointRight, const cv::Mat &pLeft, const cv::Mat &pRight, cv::Mat& out_uvpMat);
    };
}