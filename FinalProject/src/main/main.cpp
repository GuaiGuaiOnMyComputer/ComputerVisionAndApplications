#include <opencv2/opencv.hpp>

int main(int, char**)
{
    const cv::Mat image = cv::imread("../assets/SBS images/000.jpg");
    cv::imshow("000.jpg", image);
    cv::waitKey(0);
    return 0;
}