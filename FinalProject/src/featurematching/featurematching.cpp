#include <array>
#include <vector>
#include <cmath>
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

    cv::Point FeatureMatching::find_corresponding_feature_point(const cv::Mat &leftImage, const cv::Mat &rightImage, const cv::Point &pointLeft)
    {
        constexpr int32_t templateSize = 5;
        const cv::Vec3d epipolarLine = get_epipolar_line_right_image(pointLeft, FeatureMatching::s_FundementalMatrix);
        const cv::Rect searchRoi = get_feature_matching_region(rightImage, epipolarLine);

        cv::Mat matchingPattern;
        cv::Mat scoreMap(searchRoi.height, searchRoi.width, CV_32FC1, cv::Scalar(0));
        cv::Point matchedLocation;
        FeatureMatching::_get_template(leftImage, pointLeft, templateSize, matchingPattern);
        cv::matchTemplate(rightImage, matchingPattern, scoreMap, cv::TM_SQDIFF);
        cv::minMaxLoc(scoreMap, NULL, NULL, NULL, &matchedLocation);
        matchedLocation.y += searchRoi.y;

        return matchedLocation;
    }

    cv::Rect FeatureMatching::get_feature_matching_region(const cv::Mat &rightImage, const cv::Vec3d &epipolarLineRightImage)
    {
        // epipolarLineRightImage contains the 3 coefficients of a epipolar line equation ax + by + c = 0
        // to find the top and bottom boundary of the search region, solve y by y = (-ax - c) / b where x is the coordiante of the left boundary and the right boundary
        // searchAreaVertex1 and serachAeraVertex2 are the two vertics of the rectangular search area along the diagonal line
        // use the % operator to clip y coordinate of the vertice into range [0, rightImage.rows]
        const cv::Point searchAreaVertex1 = cv::Point(
            rightImage.cols,
            (int32_t)abs((-rightImage.cols * epipolarLineRightImage[0] - epipolarLineRightImage[2]) / epipolarLineRightImage[1]) % rightImage.rows
        );
        const cv::Point searchAreaVertex2 = cv::Point(
            0,
            (int32_t)abs((-0 * epipolarLineRightImage[0] - epipolarLineRightImage[2]) / epipolarLineRightImage[1]) % rightImage.rows
        );
        return cv::Rect(searchAreaVertex1, searchAreaVertex2);
    }

    cv::Mat FeatureMatching::draw_matching_ponits(const cv::Mat& leftImage, const cv::Mat& rightImage, const cv::Point& pointLeft, const cv::Point& pointRight)
    {
        cv::Mat outputImage;
        cv::hconcat(leftImage, rightImage, outputImage);
        const cv::Point pointRightInMergedImageCoordinate = cv::Point(pointRight.x + leftImage.cols, pointRight.y);
        cv::circle(outputImage, pointLeft, 5, cv::Scalar(255, 255, 0));
        cv::circle(outputImage, pointRightInMergedImageCoordinate, 5, cv::Scalar(255, 255, 0));
        cv::line(outputImage, pointLeft, pointRightInMergedImageCoordinate, cv::Scalar(0, 255, 255), 3);
    }

    void FeatureMatching::_get_template(const cv::Mat& leftImage, const cv::Point& templateCenter, const int32_t templateSize, cv::Mat &out_template)
    {
        const cv::Point templateTopLeftCorner(
            std::max(templateCenter.x - (templateSize - 1) / 2, 0),
            std::max(templateCenter.y - (templateSize - 1) / 2, 0)
        );

        const cv::Point templateBottomRightCorner(
            std::min(templateCenter.x + (templateSize - 1) / 2, leftImage.cols),
            std::min(templateCenter.y + (templateSize - 1) / 2, leftImage.rows)
        );

        out_template = leftImage(cv::Rect(templateTopLeftCorner, templateBottomRightCorner));
    }

    const cv::Mat_<double> FeatureMatching::s_FundementalMatrix = cv::Mat_<double>(3, 3, (double*)AssetConfig::FundementalMatrix.data());
}