#include <opencv2/opencv.hpp>
#include <execution>
#include <algorithm>
#include "cameracalibration.hpp"
#include "xyzio.hpp"

namespace hw4
{
    cv::Mat CameraCalibration::find_panel_homography(const std::vector<XyzIo::Coor2D_f>& imagePoints, const std::vector<XyzIo::Coor2D_f>& objectPoints)
    {
        return cv::findHomography(objectPoints, imagePoints, 0);
    }

    std::array<cv::Mat, 3> CameraCalibration::find_panel_homography(const std::array<std::vector<XyzIo::Coor2D_f>, 3> &imagePoints, const std::array<std::vector<XyzIo::Coor2D_f>, 3> &objectPoints)
    {
        std::array<cv::Mat, 3> panelHomographyMats;
        const auto findPanelHomographyWrapper = [](const std::vector<XyzIo::Coor2D_f> &_imagePoints, const std::vector<XyzIo::Coor2D_f> &_objectPoints) -> cv::Mat {
            return CameraCalibration::find_panel_homography(_imagePoints, _objectPoints); 
        };
        std::transform(std::execution::par, imagePoints.cbegin(), imagePoints.cend(), objectPoints.cbegin(), panelHomographyMats.begin(), findPanelHomographyWrapper);
        return panelHomographyMats;
    }

}