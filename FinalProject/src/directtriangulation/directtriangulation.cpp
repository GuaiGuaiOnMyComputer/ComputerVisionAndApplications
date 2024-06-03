#include <opencv2/opencv.hpp>
#include "directtriangulation.hpp"

namespace finprj
{
    DirectTriangulation::DirectTriangulation(
        const std::array<double, 4 * 4> &leftRt, const std::array<double, 4 * 4> &rightRt, 
        const std::array<double, 3 * 3> &leftK, const std::array<double, 3 * 3>& rightK, 
        const std::array<double, 4 * 4>& fundementalMatrix) noexcept : 
            _leftRt{cv::Mat(3, 4, CV_64FC1, (void*)leftRt.data())}, _rightRt{cv::Mat(3, 4, CV_64FC1, (void*)rightRt.data())},
            _leftK{cv::Mat(3, 3, CV_64FC1, (void*)leftK.data())}, _rightK{cv::Mat(3, 3, CV_64FC1, (void*)rightK.data())},
            _fundementalMatrix{cv::Mat(3, 3, CV_64FC1, (void*)fundementalMatrix.data())}
    {
        
    }
}
