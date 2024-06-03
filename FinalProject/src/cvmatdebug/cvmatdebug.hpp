#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

namespace finprj
{
    class CvMatDebug
    {
    public:

        /// @brief Prints the content of an cv::Mat to console.
        /// @tparam T Type of element contained in m.
        /// @tparam TfloatPrecision Number of digits to display for floating point elements.
        /// @tparam TelementWidth Width of an element printed to console. Must be larger than TfloatPrecision to take effect.
        /// @param m The cv::Mat object to print.
        /// @param name Name of the cv::Mat object. Also printed to the console(optional).
        /// @param message Additional message to print to the console(optional).
        template<class T, uint8_t TfloatPrecision, uint8_t TelementWidth>
        static void print_cv_mat(const cv::Mat& m, const std::string& name = "", const std::string& message = "")
        {
            std::cout << '\n';
            std::cout << std::setfill('=') << std::setw(m.cols * TelementWidth) << "";
            std::cout << '\n';
            std::cout << std::setfill(' ');
            if (!name.empty())
            {
                std::cout << "Matrix name: " << name << '\n';
            }
            if (!message.empty())
            {
                std::cout << message << '\n';
            }

            const cv::Mat_<T> &m_ = (cv::Mat_<T> &)m;
            for (int64_t i = 0; i < m.rows; i++)
            {
                for (int64_t j = 0; j < m.cols; j++)
                {
                    std::cout << std::setw(TelementWidth) << std::setprecision(TfloatPrecision) << m_(i, j) << '\t';
                }
                std::cout << '\n';
            }
            std::cout << std::setfill('=') << std::setw(m.cols * 2 * TfloatPrecision) << "";
            std::cout << '\n';
        }
    };
}