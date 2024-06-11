#include <vector>
#include <algorithm>
#include <execution>
#include <list>
#include <opencv2/opencv.hpp>
#include "acquirecolor.hpp"
#include "xyzio.hpp"

namespace finprj
{
    std::vector<std::vector<finprj::XyzIo::Rgb_ui8>> AcquireColor::get_rgb_from_right_image(const cv::Mat &projectionMatrixRight, const cv::Mat &rightImage, const std::vector<std::vector<cv::Point3d>>& worldPoints)
    {
        size_t worldPointCount{0};
        for(const std::vector<cv::Point3d>& sliceOfWorldPoints : worldPoints)
            worldPointCount += sliceOfWorldPoints.size();
        std::vector<std::vector<finprj::XyzIo::Rgb_ui8>> colorsOfEachWorldPoint(worldPointCount);
        std::transform(
            std::execution::par,
            worldPoints.cbegin(), worldPoints.cend(),
            colorsOfEachWorldPoint.begin(),
            [&](const std::vector<cv::Point3d> &sliceOfWorldPoints) -> std::vector<finprj::XyzIo::Rgb_ui8> {
                return AcquireColor::get_rgb_from_right_image(projectionMatrixRight, rightImage, sliceOfWorldPoints);
            }
        );
        return colorsOfEachWorldPoint;
    }

    std::vector<finprj::XyzIo::Rgb_ui8> AcquireColor::get_rgb_from_right_image(const cv::Mat &projectionMatrixRight, const cv::Mat &rightImage, const std::list<const cv::Point3d*>& worldPoint_ptrs)
    {
        // count the nomber of world points so the output vector can be pre-allocated
        size_t worldPointCount{0};
        for (auto _ : worldPoint_ptrs)
            worldPointCount++;

        std::vector<finprj::XyzIo::Rgb_ui8> colorsOfEachWorldPoint(worldPointCount);
        std::transform(
            std::execution::par,
            worldPoint_ptrs.cbegin(), worldPoint_ptrs.cend(),
            colorsOfEachWorldPoint.begin(),
            [&](const cv::Point3d* worldPt) -> finprj::XyzIo::Rgb_ui8 {
                return AcquireColor::get_rgb_from_right_image(projectionMatrixRight, rightImage, *worldPt);
            }
        );
        return colorsOfEachWorldPoint;
    }

    std::vector<XyzIo::Rgb_ui8> AcquireColor::get_rgb_from_right_image(const cv::Mat &projectionMatrixRight, const cv::Mat &rightImage, const std::vector<cv::Point3d> &worldPoints)
    {
        size_t worldPointCount = worldPoints.size();
        std::vector<finprj::XyzIo::Rgb_ui8> colorsOfEachWorldPoint(worldPointCount);
        std::transform(
            std::execution::par,
            worldPoints.cbegin(), worldPoints.cend(),
            colorsOfEachWorldPoint.begin(),
            [&](const cv::Point3d &worldPoint) -> finprj::XyzIo::Rgb_ui8 {
                return AcquireColor::get_rgb_from_right_image(projectionMatrixRight, rightImage, worldPoint);
            }
        );
        return colorsOfEachWorldPoint;
    }

    XyzIo::Rgb_ui8 AcquireColor::get_rgb_from_right_image(const cv::Mat &projectionMatrixRight, const cv::Mat &rightImage, const cv::Point3d& worldPoint)
    {
        std::array<double, 3> pointOnImageBuffer_;
        cv::Mat_<double> pointOnImage(3, 1, pointOnImageBuffer_.data());
        const cv::Vec4d worldPointHomo(worldPoint.x, worldPoint.y, worldPoint.z, 1);
        pointOnImage = projectionMatrixRight * worldPointHomo;
        pointOnImage /= pointOnImage(2, 0); // normalize the projected point with its homogeneous term

        if(pointOnImage(0, 0) < 0 || pointOnImage(1, 0) < 0)
            return XyzIo::Rgb_ui8(255, 0, 0);

        if ((abs(pointOnImage(0, 0)) > rightImage.cols) || (abs(pointOnImage(1, 0)) > rightImage.rows))
            return XyzIo::Rgb_ui8(255, 0, 0);

        const cv::Vec3b pixelColor = rightImage.at<cv::Vec3b>(pointOnImage(1), pointOnImage(0));
        return XyzIo::Rgb_ui8(pixelColor[2], pixelColor[1], pixelColor[0]);
    }
}