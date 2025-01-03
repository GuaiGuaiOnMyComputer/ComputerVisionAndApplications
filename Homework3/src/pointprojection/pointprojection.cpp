#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <execution>
#include <stdio.h>
#include "pointprojection.hpp"
#include "xyzio.hpp"

template class cv::Mat_<float>;
template class cv::Mat_<double>;

namespace hw3
{
    cv::Mat PointProjection::get_puv_mat(const std::vector<hw3::XyzIo::Coor3D_f>& objectPoints, const std::vector<hw3::XyzIo::Coor2D_f>& imagePoints)
    {
        const size_t pointPairCount = objectPoints.size();
        assert(objectPoints.size() == imagePoints.size());

        cv::Mat puvMat(2 * pointPairCount, 12, CV_32FC1, cv::Scalar(0));
        for (size_t i = 0; i < pointPairCount; i++)
        {
            const cv::Vec4f objectPointsHomo(objectPoints[i].x, objectPoints[i].y, objectPoints[i].z, 1);

            puvMat.at<float>(2 * i, 0) = objectPointsHomo[0];
            puvMat.at<float>(2 * i, 1) = objectPointsHomo[1];
            puvMat.at<float>(2 * i, 2) = objectPointsHomo[2];
            puvMat.at<float>(2 * i, 3) = objectPointsHomo[3];

            memset(&puvMat.at<float>(2 * i, 4), 0, 4 * sizeof(float));

            puvMat.at<float>(2 * i, 8) =  -imagePoints[i].x * objectPointsHomo[0];
            puvMat.at<float>(2 * i, 9) = -imagePoints[i].x * objectPointsHomo[1];
            puvMat.at<float>(2 * i, 10) = -imagePoints[i].x * objectPointsHomo[2];
            puvMat.at<float>(2 * i, 11) = -imagePoints[i].x * objectPointsHomo[3];

            memset(&puvMat.at<float>(2 * i + 1, 0), 0, 4 * sizeof(float));

            puvMat.at<float>(2 * i + 1, 4) = objectPointsHomo[0];
            puvMat.at<float>(2 * i + 1, 5) = objectPointsHomo[1];
            puvMat.at<float>(2 * i + 1, 6) = objectPointsHomo[2];
            puvMat.at<float>(2 * i + 1, 7) = objectPointsHomo[3];

            puvMat.at<float>(2 * i + 1, 8) =  -imagePoints[i].y * objectPointsHomo[0];
            puvMat.at<float>(2 * i + 1, 9) =  -imagePoints[i].y * objectPointsHomo[1];
            puvMat.at<float>(2 * i + 1, 10) = -imagePoints[i].y * objectPointsHomo[2];
            puvMat.at<float>(2 * i + 1, 11) = -imagePoints[i].y * objectPointsHomo[3];
        }
        return puvMat;
    }

    cv::Mat PointProjection::get_projection_mat(cv::Mat& puvMat)
    {
        cv::Mat w, u, vt;
        cv::SVD::compute(puvMat, w, u, vt);

        cv::Mat projectionMat = cv::Mat(vt.t(), cv::Range::all(), cv::Range(11, 12)).clone().reshape(1, 3);
        projectionMat /= projectionMat.at<float>(2, 3); // rescale the projection matrix so the bottom-right corner becomes 1
        return projectionMat;
    }

    cv::Mat PointProjection::get_projection_mat(const std::vector<hw3::XyzIo::Coor3D_f>& objectPoints, const std::vector<hw3::XyzIo::Coor2D_f>& imagePoints)
    {
        cv::Mat puvMat = get_puv_mat(objectPoints, imagePoints);
        return get_projection_mat(puvMat);
    }

    cv::Point2f PointProjection::project_to_image(const cv::Point3f &worldPt, const cv::Mat& projectionMatrix)
    {
        cv::Point2f out_imgPointHomo;
        project_to_image(worldPt, out_imgPointHomo, projectionMatrix);
        return out_imgPointHomo;
    }

    void PointProjection::project_to_image(const cv::Point3f &worldPt, cv::Point2f &out_imgPoint, const cv::Mat& projectionMatrix)
    {
        cv::Vec4f pointInWorldHomo(worldPt.x, worldPt.y, worldPt.z, 1);
        cv::Vec3f pointInImageHomo(0, 0, 0);
        for (size_t i = 0; i < 4; i++)
        {
            pointInImageHomo[0] += projectionMatrix.at<float>(0, i) * pointInWorldHomo[i];
            pointInImageHomo[1] += projectionMatrix.at<float>(1, i) * pointInWorldHomo[i];
            pointInImageHomo[2] += projectionMatrix.at<float>(2, i) * pointInWorldHomo[i];
        }
        // normalize the projected point by rescaling their z coordinates to 1
        pointInImageHomo /= pointInImageHomo[2];
        out_imgPoint = cv::Point2f(pointInImageHomo[0], pointInImageHomo[1]);
    }

    std::vector<cv::Point2f> PointProjection::project_to_image(const std::vector<XyzIo::CoorAndNormal3D_f> &worldPt, const cv::Mat& projectionMatrix)
    {
        std::vector<cv::Point2f> pointsInImageNonHomo(worldPt.size());

        // create a wrapper for the overloaded functoin project_to_image that calls the function with projection matrix predefined
        const auto singlePointProjectionActionWrapper = [&](const cv::Point3f &worldPt) constexpr -> cv::Point2f
            { return PointProjection::project_to_image(worldPt, projectionMatrix); };
        std::transform(std::execution::par, worldPt.cbegin(), worldPt.cend(), pointsInImageNonHomo.begin(), singlePointProjectionActionWrapper);
        return pointsInImageNonHomo;
    }

    std::vector<cv::Point2f> PointProjection::project_to_image(const std::vector<XyzIo::CoorAndNormal3D_f> &worldPt, const cv::Mat& projectionMatrix, bool includeOccluded, const cv::Vec3f& cameraPointingVtr, std::vector<XyzIo::CoorAndNormal3D_f>& out_nonOccludedPoints)
    {
        out_nonOccludedPoints.reserve(worldPt.size());
        for (size_t i = 0; i < worldPt.size(); i++)
        {
            if (!check_if_point_occluded(worldPt[i], cameraPointingVtr) | includeOccluded)
                out_nonOccludedPoints.push_back(worldPt[i]);
        }
        std::vector<cv::Point2f> pointsInImageNonHomo(out_nonOccludedPoints.size());
        const auto singlePointProjectionActionWrapper =
            [&](const XyzIo::CoorAndNormal3D_f &worldPtAndNor) constexpr -> cv::Point2f
            {
                return PointProjection::project_to_image(worldPtAndNor, projectionMatrix);
            };
        std::transform(std::execution::par, out_nonOccludedPoints.cbegin(), out_nonOccludedPoints.cend(), pointsInImageNonHomo.begin(), singlePointProjectionActionWrapper);
        return pointsInImageNonHomo;
    }

    XyzIo::Rgb_ui8 PointProjection::get_rgb_from_image(const XyzIo::CoorAndNormal3D_f& worldPt, const cv::Mat& projectionMat, const cv::Mat& image)
    {
        cv::Point2f imagePoint = project_to_image(worldPt, projectionMat);
        return get_rgb_from_2d_coordinate(imagePoint, image);
    }

    std::vector<XyzIo::Rgb_ui8> PointProjection::get_rgb_from_image(const std::vector<XyzIo::CoorAndNormal3D_f>& worldPt, const cv::Mat& projectionMat, const cv::Mat& image)
    {
        const std::vector<cv::Point2f> imagePoints = project_to_image(worldPt, projectionMat);
        const std::vector<XyzIo::Rgb_ui8> rgbValues = get_rgb_from_2d_coordinate(imagePoints, image);
        return rgbValues;
    }

    bool PointProjection::check_if_point_occluded(const XyzIo::CoorAndNormal3D_f& worldPt, const cv::Vec3f &cameraPointingVtr)
    {
        return cameraPointingVtr.dot(worldPt) > 0;
    }

    void PointProjection::save_image_with_selected_points(const fs::path& outputPath, const cv::Mat& refImage, const std::vector<XyzIo::Coor2D_f>& selectedPoints)
    {
        cv::Mat annotatedImage = refImage.clone();
        for (const cv::Point2f& pt : selectedPoints)
            cv::circle(annotatedImage, pt, 5, cv::Scalar(255, 255, 0), 5);
        cv::imwrite(outputPath.string(), annotatedImage);
    }

    cv::Mat PointProjection::show_projected_points(const cv::Mat& image, const std::vector<cv::Point2f>& projectedPoints)
    {
        uint32_t validPointCount{0};
        cv::Mat annotatedImage = image.clone();
        for (const cv::Point2f& pt : projectedPoints)
        {
            cv::circle(annotatedImage, pt, 2, cv::Scalar(255, 255, 0));
            validPointCount++;
        }
        return annotatedImage;
    }

    XyzIo::Rgb_ui8 PointProjection::get_rgb_from_2d_coordinate(const cv::Point2f& imagePoint, const cv::Mat& referenceImage)
    {
        const cv::Vec<uint8_t, 3> bgr = referenceImage.at<cv::Vec<uint8_t, 3>>((int32_t)(imagePoint.y), (int32_t)(imagePoint.x));
        return XyzIo::Rgb_ui8(bgr[2], bgr[1], bgr[0]); // reorder the pixel values from BGR format used by opencv to RGB used by meshlab.
    }

    std::vector<XyzIo::Rgb_ui8> PointProjection::get_rgb_from_2d_coordinate(const std::vector<cv::Point2f> &imagePoints, const cv::Mat& referenceImage)
    {
        std::vector<XyzIo::Rgb_ui8> pointRgbVals(imagePoints.size());
        const auto singlePointGetRgbActionWrapper = [&](const cv::Point2f &imgPt) constexpr -> XyzIo::Rgb_ui8
            { return PointProjection::get_rgb_from_2d_coordinate(imgPt, referenceImage); };
        std::transform(std::execution::par, imagePoints.cbegin(), imagePoints.cend(), pointRgbVals.begin(), singlePointGetRgbActionWrapper);
        return pointRgbVals;
    }

} // namespace hw3
