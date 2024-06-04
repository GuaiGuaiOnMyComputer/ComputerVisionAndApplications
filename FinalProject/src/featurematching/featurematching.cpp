#include <array>
#include <vector>
#include <opencv2/opencv.hpp>
#include "featurematching.hpp"
#include "assetconfig.hpp"

namespace finprj
{
    cv::Vec3d FeatureMatching::get_epipolar_line_right_image(const cv::Point2d& pointLeft, const cv::Mat& fundementalMatrix)
    {
        const cv::Vec3d pointLeftHomo(pointLeft.x, pointLeft.y, 1);
        std::array<double, 3> epipolarLineResultBuffer;
        cv::Mat_<double> epipolarLineResult(3, 1, epipolarLineResultBuffer.data());
        epipolarLineResult = fundementalMatrix * pointLeftHomo;
        return cv::Vec3d((double*)epipolarLineResult.data);
    }

    const cv::Mat_<double> FeatureMatching::s_FundementalMatrix = cv::Mat_<double>(3, 3, (double*)AssetConfig::FundementalMatrix.data());
}