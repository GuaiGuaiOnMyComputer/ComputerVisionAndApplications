#include <opencv2/opencv.hpp>
#include <vector>
#include "pointprojection.hpp"
#include "xyzio.hpp"

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

            puvMat.at<float>(2 * i + 1, 0) = objectPoints[i].x;
            puvMat.at<float>(2 * i + 1, 1) = objectPoints[i].y;
            puvMat.at<float>(2 * i + 1, 2) = objectPoints[i].z;

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
        cv::Vec4f pointInWorldHomo(worldPt.x, worldPt.y, worldPt.z, 1);
        cv::Point3f pointInImageHomo(0, 0, 0);
        for (size_t i = 0; i < 4; i++)
        {
            pointInImageHomo.x += projectionMatrix.at<float>(0, i) * pointInWorldHomo[0];
            pointInImageHomo.y += projectionMatrix.at<float>(1, i) * pointInWorldHomo[1];
            pointInImageHomo.z += projectionMatrix.at<float>(2, i) * pointInWorldHomo[2];
        }
        pointInImageHomo /= pointInImageHomo.z;
        return cv::Point2f(pointInImageHomo.x, pointInImageHomo.y);
    }
    
} // namespace hw3
