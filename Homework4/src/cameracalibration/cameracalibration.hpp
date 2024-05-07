#include <opencv2/opencv.hpp>
#include <array>
#include "xyzio.hpp"

namespace hw4
{
    class CameraCalibration
    {
    public:
        static cv::Mat find_panel_homography(const std::vector<XyzIo::Coor2D_f>& imagePoints, const std::vector<XyzIo::Coor2D_f>& objectPoints);
        static std::array<cv::Mat, 3> find_panel_homography(const std::array<std::vector<XyzIo::Coor2D_f>, 3> &imagePoints, const std::array<std::vector<XyzIo::Coor2D_f>, 3> &objectPoints);
    };
}