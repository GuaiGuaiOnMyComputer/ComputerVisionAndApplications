#include <opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>

namespace hw4
{
    class DebugCode
    {
    public:

        template<class T, uint8_t TfloatPrecision, uint8_t TelementWidth>
        static void print_cv_mat(const cv::Mat& m)
        {
            const cv::Mat_<T> &m_ = (cv::Mat_<T> &)m;
            for (size_t i = 0; i < m.rows; i++)
            {
                for (size_t j = 0; j < m.cols; j++)
                {
                    std::cout << std::setw(TelementWidth) << std::setprecision(TfloatPrecision) << m_(i, j) << '\t';
                }
                std::cout << '\n';
            }
        }
    };
}