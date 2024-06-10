#include <array>
#include <vector>
#include <algorithm>
#include <functional>
#include <execution>
#include <ranges>
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

    void FeatureMatching::find_corresponding_feature_point(const cv::Mat &leftImage, const cv::Mat &rightImage, const cv::Mat_<cv::Point> &pointLeft, cv::Mat_<cv::Point> &out_matchedPointRight)
    {
        const int32_t leftPointCount = pointLeft.rows * pointLeft.cols;
        out_matchedPointRight = cv::Mat_<cv::Point>(1, { leftPointCount });

        std::transform(
            std::execution::par,
            pointLeft.begin(), pointLeft.end(),
            out_matchedPointRight.begin(),
            [&](const cv::Point &pointToMatch) -> cv::Point {
                return FeatureMatching::find_corresponding_feature_point(leftImage, rightImage, pointToMatch);
            }
        );
    }


    cv::Point FeatureMatching::find_corresponding_feature_point(const cv::Mat &leftImage, const cv::Mat &rightImage, const cv::Point &pointLeft)
    {
        constexpr int32_t templateSize = 5;
        cv::Point matchedLocation(-1, -1);
        const cv::Vec3d epipolarLine = get_epipolar_line_right_image(pointLeft, FeatureMatching::s_FundementalMatrix);
        const cv::Rect searchRoiRight = get_feature_matching_region(rightImage, epipolarLine, templateSize);

        cv::Mat_<cv::Point> nonZeroPixelCoorsWithinSearchRoi;
        cv::findNonZero(rightImage(searchRoiRight), nonZeroPixelCoorsWithinSearchRoi);
        std::forward_list<const cv::Point*> possibleMatchingPointsRight;
        size_t possibleMatchingPointCount{0};
        for (size_t i = 0; i < nonZeroPixelCoorsWithinSearchRoi.total(); i++)
        {
            nonZeroPixelCoorsWithinSearchRoi(i).y += searchRoiRight.y;
            if (check_if_on_epipolar_line(nonZeroPixelCoorsWithinSearchRoi(i), epipolarLine, 0.5))
            {
                possibleMatchingPointsRight.push_front(&nonZeroPixelCoorsWithinSearchRoi(i));
                possibleMatchingPointCount++;
            }
        }

        if (possibleMatchingPointCount == 0)
            return matchedLocation;

        cv::Mat matchingPatternLeft;
        cv::Mat toBeMatchedRight;
        _get_template(leftImage, pointLeft, templateSize, matchingPatternLeft);
        size_t matchingPointIndex{0};
        int32_t highestScore{-1};
        for (auto matchingPointRight_iter = possibleMatchingPointsRight.cbegin(); matchingPointRight_iter != possibleMatchingPointsRight.cend(); matchingPointRight_iter++)
        {
            const cv::Rect toBeMatchedRoi = _get_roi_from_center_coor(**matchingPointRight_iter, templateSize, rightImage.cols, rightImage.rows);
            toBeMatchedRight = rightImage(toBeMatchedRoi);
            const cv::Scalar score = cv::sum(matchingPatternLeft.mul(toBeMatchedRight));
            if (score[0] > highestScore)
            {
                highestScore = score[0];
                matchedLocation = cv::Point((*matchingPointRight_iter)->x, (*matchingPointRight_iter)->y);
            }
            matchingPointIndex++;
        }

        return matchedLocation;
    }

    cv::Rect FeatureMatching::get_feature_matching_region(const cv::Size &rightImageSize, const cv::Vec3d &epipolarLineRightImage, const int64_t minRoiHeight)
    {
        // epipolarLineRightImage contains the 3 coefficients of a epipolar line equation ax + by + c = 0
        // to find the top and bottom boundary of the search region, solve y by y = (-ax - c) / b where x is the coordiante of the left boundary and the right boundary
        // searchAreaVertex1 and serachAeraVertex2 are the two vertics of the rectangular search area along the diagonal line
        // use the % operator to clip y coordinate of the vertice into range [0, rightImage.rows]
        std::array<cv::Point, 2> roiVerticsYCoor {
            /*Left vertex y coor */cv::Point(0, ((-0 * epipolarLineRightImage[0] - epipolarLineRightImage[2]) / epipolarLineRightImage[1])),
            /*Right vertex y coor*/cv::Point(rightImageSize.width, (-rightImageSize.width * epipolarLineRightImage[0] - epipolarLineRightImage[2]) / epipolarLineRightImage[1])
        };
        std::sort(roiVerticsYCoor.begin(), roiVerticsYCoor.end(), 
            [](const cv::Point &p1, const cv::Point &p2){
                return p1.y < p2.y;});

        // clip the y coordiantes of the vertics into range [0, rightImageSize.height]
        // for top bound y coordinate, if y < 0, y = y * 0 else y = y * 1
        // for bottom bound y coordinate, if y > rightImageSize.height, y = y - (y - rightImageSize.height - 1) else y = y - 0
        roiVerticsYCoor[0].y *= !(roiVerticsYCoor[0].y < 0); 
        roiVerticsYCoor[1].y -= (roiVerticsYCoor[1].y > rightImageSize.height) * (roiVerticsYCoor[1].y - rightImageSize.height - 1); // if y coordinate > rightImageSize.height, y = rightImageSize.height * 1

        // adjust the height of the ROI if not tall enough
        // if it's too short and the top boundary is already at top, shift the bottom boundary down
        // if it's too short and the bottom boundary is already at bottom, shift the top boundary up
        const int64_t roiHeight = roiVerticsYCoor[1].y - roiVerticsYCoor[0].y;
        const bool roiTooShortFlag = roiHeight < minRoiHeight;
        roiVerticsYCoor[1].y += minRoiHeight * roiTooShortFlag;
        const bool roiTopBoundaryExtremeFlag = roiVerticsYCoor[0].y - minRoiHeight < 0;
        const bool roiBottomBoundaryExtremeFlag = roiVerticsYCoor[1].y + minRoiHeight > rightImageSize.height;
        roiVerticsYCoor[0].y -= minRoiHeight * (roiTooShortFlag && roiBottomBoundaryExtremeFlag);
        roiVerticsYCoor[1].y += minRoiHeight * (roiTooShortFlag && roiTopBoundaryExtremeFlag);
        const cv::Point searchAreaLeftVertex = cv::Point(roiVerticsYCoor[0].x, roiVerticsYCoor[0].y);
        const cv::Point searchAreaRightVertex = cv::Point(roiVerticsYCoor[1].x, roiVerticsYCoor[1].y);

        return cv::Rect(searchAreaRightVertex, searchAreaLeftVertex);
    }

    cv::Rect FeatureMatching::get_feature_matching_region(const cv::Mat &rightImage, const cv::Vec3d &epipolarLineRightImage, const int64_t minRoiHeight)
    {
        // epipolarLineRightImage contains the 3 coefficients of a epipolar line equation ax + by + c = 0
        // to find the top and bottom boundary of the search region, solve y by y = (-ax - c) / b where x is the coordiante of the left boundary and the right boundary
        // searchAreaVertex1 and serachAeraVertex2 are the two vertics of the rectangular search area along the diagonal line
        // use the % operator to clip y coordinate of the vertice into range [0, rightImage.rows]
        return FeatureMatching::get_feature_matching_region(rightImage.size(), epipolarLineRightImage, minRoiHeight);
    }

    cv::Mat FeatureMatching::draw_matching_points(const cv::Mat& leftImage, const cv::Mat& rightImage, const cv::Point& pointLeft, const cv::Point& pointRight)
    {
        cv::Mat outputImage;
        cv::hconcat(leftImage, rightImage, outputImage);
        const cv::Point pointRightInMergedImageCoordinate = cv::Point(pointRight.x + leftImage.cols, pointRight.y);
        cv::circle(outputImage, pointLeft, 2, cv::Scalar(255, 255, 0));
        cv::circle(outputImage, pointRightInMergedImageCoordinate, 2, cv::Scalar(255, 255, 0));
        cv::line(outputImage, pointLeft, pointRightInMergedImageCoordinate, cv::Scalar(0, 255, 255), 2);
        return outputImage;
    }

    cv::Mat FeatureMatching::draw_matching_points(const cv::Mat &image, const cv::Mat_<cv::Point> &pointsLeft, const cv::Mat_<cv::Point> &pointsRight)
    {
        cv::Mat outputImage = image.clone();
        for (int32_t i = 0; i < pointsLeft.rows * pointsLeft.cols; i++)
        {
            const cv::Point pointRightInMergedImageCoordinate = cv::Point(pointsRight(i).x + image.cols / 2, pointsRight(i).y);
            cv::circle(outputImage, pointsLeft(i), 5, cv::Scalar(255, 255, 0));
            cv::circle(outputImage, pointRightInMergedImageCoordinate, 5, cv::Scalar(255, 255, 0));
            cv::line(outputImage, pointsLeft(i), pointRightInMergedImageCoordinate, cv::Scalar(0, 255, 255), 3);
        }
        return outputImage;
    }

    cv::Mat FeatureMatching::draw_matching_points(const cv::Mat &image, const std::forward_list<const cv::Point*>& pointsLeft_ptrs, const std::forward_list<const cv::Point*>& pointsRight_ptrs)
    {
        cv::Mat outputImage = image.clone();
        auto pointsLeftPtrs_itr = pointsLeft_ptrs.cbegin();
        auto pointsRightPtrs_itr = pointsRight_ptrs.cbegin();
        while (pointsLeftPtrs_itr != pointsLeft_ptrs.cend() && pointsRightPtrs_itr != pointsRight_ptrs.cend())
        {
            const cv::Point pointRightInMergedImageCoordinate = cv::Point((*pointsRightPtrs_itr)->x + image.cols / 2, (*pointsRightPtrs_itr)->y);
            cv::circle(outputImage, *(*pointsLeftPtrs_itr), 5, cv::Scalar(255, 255, 0));
            cv::circle(outputImage, pointRightInMergedImageCoordinate, 5, cv::Scalar(255, 255, 0));
            cv::line(outputImage, *(*pointsLeftPtrs_itr), pointRightInMergedImageCoordinate, cv::Scalar(0, 255, 255), 3);
            pointsLeftPtrs_itr++;
            pointsRightPtrs_itr++;
        }
        return outputImage;

    }

    void FeatureMatching::remove_mismatched_point(const cv::Mat_<cv::Point>& pointLeft, const cv::Mat_<cv::Point>& pointRight, std::forward_list<const cv::Point *>& out_validPointLeft, std::forward_list<const cv::Point*>& out_validPointRight, size_t &out_validPointCount)
    {
        out_validPointCount = 0;
        for (size_t i = 0; i < pointLeft.total(); i++)
        {
            if (pointRight(i).x > 0)
            {
                out_validPointLeft.push_front(&pointLeft(i));
                out_validPointRight.push_front(&pointRight(i));
                out_validPointCount++;
            }
        }
    }

    inline bool FeatureMatching::check_if_on_epipolar_line(const cv::Point &pointRight, const cv::Vec3d &epipolarLineCoeff, const double threshold)
    {
        return abs(pointRight.x * epipolarLineCoeff[0] +  pointRight.y * epipolarLineCoeff[1] + 1 * epipolarLineCoeff[2]) < threshold;
    }

    void FeatureMatching::reject_mismatched_point(cv::Point &in_out_projectedPoint, const cv::Vec3d &epipolarLineCoeff, const double threshold)
    {
        if (!check_if_on_epipolar_line(in_out_projectedPoint, epipolarLineCoeff, threshold))
        {
            in_out_projectedPoint.x = -1;
            in_out_projectedPoint.y = -1;
        }
    }

    void FeatureMatching::reject_mismatched_point(std::vector<cv::Point> &in_out_projectedPoint, const cv::Vec3d &epipolarLineCoeff, const double threshold)
    {
        std::for_each(
            std::execution::par,
            in_out_projectedPoint.begin(),
            in_out_projectedPoint.end(),
            [&](cv::Point &projected){
                reject_mismatched_point(projected, epipolarLineCoeff, threshold);
            }
        );
    }


    inline void FeatureMatching::_get_template(const cv::Mat& leftImage, const cv::Point& templateCenter, const int32_t templateSize, cv::Mat &out_template)
    {
        out_template = leftImage(_get_roi_from_center_coor(templateCenter, templateSize, leftImage.cols, leftImage.rows));
    }

    inline cv::Rect FeatureMatching::_get_roi_from_center_coor(const cv::Point& roiCenter, const int32_t roiSize, const int32_t maxX, const int32_t maxY)
    {
        cv::Point roiTopLeftCorner(
            std::max(roiCenter.x - (roiSize - 1) / 2, 0),
            std::max(roiCenter.y - (roiSize - 1) / 2, 0)
        );

        cv::Point roiBottomRightCorner(
            std::min(roiCenter.x + (roiSize - 1) / 2, maxX),
            std::min(roiCenter.y + (roiSize - 1) / 2, maxY)
        );

        const int32_t roiWidth = roiBottomRightCorner.x - roiTopLeftCorner.x;
        const bool roiTooNarrowFlag = roiWidth < roiSize;
        roiBottomRightCorner.x += roiTooNarrowFlag * (roiSize - roiWidth);

        const int32_t roiHeight = roiBottomRightCorner.y - roiTopLeftCorner.y;
        const bool roiTooShortFlag = roiHeight < roiSize;
        roiBottomRightCorner.y += roiTooShortFlag * (roiSize - roiHeight);
        
        return cv::Rect(roiTopLeftCorner, roiBottomRightCorner);
    }

    const cv::Mat_<double> FeatureMatching::s_FundementalMatrix = cv::Mat_<double>(3, 3, (double*)AssetConfig::FundementalMatrix.data());
}