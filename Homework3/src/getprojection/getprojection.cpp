#include <opencv2/opencv.hpp>
#include <vector>
#include "getprojection.hpp"

namespace hw3
{
    cv::Mat GetProjection::get_puv_mat(const std::vector<cv::Point3f>& objectPoints, const std::vector<cv::Point2f>& imagePoints)
    {
        const size_t pointPairCount = objectPoints.size();
        assert(objectPoints.size() == imagePoints.size());

        cv::Mat puvMat(2 * pointPairCount, 12, CV_32FC1, 0);
        for (size_t i = 0; i < pointPairCount; i++)
        {
            puvMat.at<float>(3, 3) = 0;
        }
    }
} // namespace hw3
