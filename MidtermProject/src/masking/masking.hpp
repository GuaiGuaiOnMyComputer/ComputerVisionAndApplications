#include <vector>
#include <opencv2/opencv.hpp>

namespace midproj
{
    void find_red_pixels(cv::InputArray image, std::vector<cv::Point> &out_redPxCoors);
}