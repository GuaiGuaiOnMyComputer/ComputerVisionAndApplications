#include <opencv2/opencv.hpp>
#include <array>
#include "xyzio.hpp"

namespace hw4
{
    class CameraCalibration
    {
    public:
        enum Panel
        {
            LEFT = 0,
            RIGHT,
            TOP,
            PANEL_COUNT
        };
        static cv::Mat find_panel_homography(const std::vector<XyzIo::Coor2D_f>& imagePoints, const std::vector<XyzIo::Coor2D_f>& objectPoints);
        static std::vector<cv::Mat> find_panel_homography(const std::array<std::vector<XyzIo::Coor2D_f>, PANEL_COUNT> &imagePoints, const std::array<std::vector<XyzIo::Coor2D_f>, PANEL_COUNT> &panelPoints);
        static cv::Mat get_k_matrix(const std::vector<cv::Mat> &panelHomographyMats);
        static std::vector<cv::Mat> get_rt_matrix(const cv::Mat &k, const std::vector<cv::Mat> &panelHomographyMat);
        static cv::Mat get_rt_matrix(const cv::Mat &k_inverse, const cv::Mat &panelHomographyMat);

    private:
        static cv::Mat _get_the_crazy_mat_that_mulitplies_with_omega_and_i_have_no_idea_how_it_works(const std::vector<cv::Mat> &panelHomographyMats);
    };
}