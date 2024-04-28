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
            puvMat.at<float>(2 * i, 0) = objectPoints[i].x;
            puvMat.at<float>(2 * i, 1) = objectPoints[i].y;
            puvMat.at<float>(2 * i, 2) = objectPoints[i].z;

            memset(&puvMat.at<float>(2 * i, 3), 0, 6 * sizeof(float));

            puvMat.at<float>(2 * i, 9) =  -imagePoints[i].x * objectPoints[i].x;
            puvMat.at<float>(2 * i, 10) = -imagePoints[i].x * objectPoints[i].y;
            puvMat.at<float>(2 * i, 11) = -imagePoints[i].x * objectPoints[i].z;

            memset(&puvMat.at<float>(2 * i + 1, 0), 0, 6 * sizeof(float));

            puvMat.at<float>(2 * i + 1, 6) = objectPoints[i].x;
            puvMat.at<float>(2 * i + 1, 7) = objectPoints[i].y;
            puvMat.at<float>(2 * i + 1, 8) = objectPoints[i].z;

            puvMat.at<float>(2 * i + 1, 9) =  -imagePoints[i].y * objectPoints[i].x;
            puvMat.at<float>(2 * i + 1, 10) = -imagePoints[i].y * objectPoints[i].y;
            puvMat.at<float>(2 * i + 1, 11) = -imagePoints[i].y * objectPoints[i].z;
        }
        return puvMat;
    }

    cv::Mat PointProjection::get_projection_mat(cv::Mat& puvMat)
    {
        cv::Mat w, u, vt;
        cv::SVD::compute(puvMat, w, u, vt, cv::SVD::FULL_UV);

        cv::Mat projectionMat = cv::Mat(u, cv::Range::all(), cv::Range(u.cols - 1, u.cols)).clone().reshape(1, 3);
        projectionMat /= projectionMat.at<float>(2, 3); // normalize the bottom-left element to 1
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
            pointInImageHomo[0] += projectionMatrix.at<float>(0, i) * pointInWorldHomo[0];
            pointInImageHomo[1] += projectionMatrix.at<float>(1, i) * pointInWorldHomo[1];
            pointInImageHomo[2] += projectionMatrix.at<float>(2, i) * pointInWorldHomo[2];
        }
        // normalize the projected point by rescaling their z coordinates to 1
        pointInImageHomo[0] /= pointInImageHomo[2];
        pointInImageHomo[1] /= pointInImageHomo[2];
        out_imgPoint = cv::Point2f(pointInImageHomo[0], pointInImageHomo[1]);
    }

    std::vector<cv::Point2f> PointProjection::project_to_image(const std::vector<XyzIo::CoorAndNormal3D_f> &worldPt, const cv::Mat& projectionMatrix)
    {
        std::vector<cv::Point2f> pointsInImageNonHomo(worldPt.size());

        // create a wrapper for the overloaded functoin project_to_image that calls the function with projection matrix predefined
        const auto singlePointProjectionActionWrapper = [&](const cv::Point3f &worldPt) constexpr -> cv::Point2f
            { return PointProjection::project_to_image(worldPt, projectionMatrix); };
        std::transform(std::execution::par_unseq, worldPt.cbegin(), worldPt.cend(), pointsInImageNonHomo.begin(), singlePointProjectionActionWrapper);
        return pointsInImageNonHomo;
    }

    cv::Mat PointProjection::show_projected_points(const cv::Mat& image, std::vector<cv::Point2f>& projectedPoints)
    {
        uint32_t validPointCount{0};
        cv::Mat annotatedImage = image.clone();
        for (const cv::Point2f& pt : projectedPoints)
        {
            cv::circle(annotatedImage, pt, 2, cv::Scalar(255, 255, 0));
            validPointCount++;
        }
        char textOnImage[80]{0};
        std::snprintf(textOnImage, 80, "Valid point Count: %i", validPointCount);
        cv::putText(annotatedImage, textOnImage, cv::Point(0, 0), cv::FONT_HERSHEY_SIMPLEX, 20, cv::Scalar(255, 255, 0));
        return annotatedImage;
    }
} // namespace hw3
