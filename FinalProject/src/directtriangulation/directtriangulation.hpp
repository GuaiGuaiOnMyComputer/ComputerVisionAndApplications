#include <opencv2/opencv.hpp>
#include <array>

namespace finprj
{
    class DirectTriangulation
    {
    public:
        DirectTriangulation(
            const std::array<double, 4 * 4> &leftRt, const std::array<double, 4 * 4> &rightRt, 
            const std::array<double, 3 * 3> &leftK, const std::array<double, 3 * 3>& rightK, 
            const std::array<double, 4 * 4>& fundementalMatrix) noexcept;

        DirectTriangulation() = delete;
        DirectTriangulation(DirectTriangulation &&) = delete;

    private:
        const cv::Mat _leftRt;
        const cv::Mat _rightRt;
        const cv::Mat _leftK;
        const cv::Mat _rightK;
        const cv::Mat _fundementalMatrix;
    };
}