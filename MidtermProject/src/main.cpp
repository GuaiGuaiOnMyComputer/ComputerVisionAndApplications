#include <opencv2/opencv.hpp>
#include <stdint.h>

int main(int32_t argc, char** argv)
{
    cv::Mat img = cv::imread("assets/ShadowStrip/0000.jpg");
    cv::imshow("Test image", img);
    cv::waitKey();
    return 0;
}