#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

int main()
{
  cv::Mat image = cv::imread("asset\\SceneFromCamera1.jpg");
  while(cv::waitKey(0) != 69)
  {
    cv::imshow("image", image);
  }
  return 0;
}