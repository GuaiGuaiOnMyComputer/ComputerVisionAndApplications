#include <opencv2/opencv.hpp>
#include <filesystem>
#include <array>
#include "assetcheck.hpp"
#include "cameracalibration.hpp"

#if true
template class std::vector<hw4::XyzIo::Coor2D_f>;
template class std::vector<hw4::XyzIo::Coor3D_f>;
template class std::vector<cv::Mat>;
template class std::array<cv::Mat, hw4::CameraCalibration::PANEL_COUNT>;
template class std::array < std::vector<hw4::XyzIo::Coor2D_f>, hw4::CameraCalibration::PANEL_COUNT>;
template class cv::Mat_<float>;
template class cv::Mat_<double>;
#endif

int main(int, char**)
{
    namespace fs = std::filesystem;
    using Panel = hw4::CameraCalibration::Panel;
    const std::array<fs::path, Panel::PANEL_COUNT> PANEL_CORNERS_PATHS_IMAGE{
        fs::path("../assets/leftPanelCorners_Image.xyz"),
        fs::path("../assets/rightPanelCorners_Image.xyz"),
        fs::path("../assets/topPanelCorners_Image.xyz"),
    };
    const std::array<fs::path, Panel::PANEL_COUNT> PANEL_CORNERS_PATHS_PANEL_LOCAL{
        fs::path("../assets/leftPanelCorners_PanelLocal.xyz"),
        fs::path("../assets/rightPanelCorners_PanelLocal.xyz"),
        fs::path("../assets/topPanelCorners_PanelLocal.xyz"),
    };
    hw4::AssetCheck::check_if_asset_exist(PANEL_CORNERS_PATHS_IMAGE);
    hw4::AssetCheck::check_if_asset_exist(PANEL_CORNERS_PATHS_PANEL_LOCAL);

    const std::array<std::vector<hw4::XyzIo::Coor2D_f>, Panel::PANEL_COUNT> panelCornerCoorsImage{
        hw4::XyzIo::load_points_from_file<hw4::XyzIo::Coor2D_f, float>(PANEL_CORNERS_PATHS_IMAGE[Panel::LEFT], 0),
        hw4::XyzIo::load_points_from_file<hw4::XyzIo::Coor2D_f, float>(PANEL_CORNERS_PATHS_IMAGE[Panel::RIGHT], 0),
        hw4::XyzIo::load_points_from_file<hw4::XyzIo::Coor2D_f, float>(PANEL_CORNERS_PATHS_IMAGE[Panel::TOP], 0)
    };
    const std::array<std::vector<hw4::XyzIo::Coor2D_f>, Panel::PANEL_COUNT> panelCornerCoorsPanelLocal{
        hw4::XyzIo::load_points_from_file<hw4::XyzIo::Coor2D_f, float>(PANEL_CORNERS_PATHS_PANEL_LOCAL[Panel::LEFT], 0),
        hw4::XyzIo::load_points_from_file<hw4::XyzIo::Coor2D_f, float>(PANEL_CORNERS_PATHS_PANEL_LOCAL[Panel::RIGHT], 0),
        hw4::XyzIo::load_points_from_file<hw4::XyzIo::Coor2D_f, float>(PANEL_CORNERS_PATHS_PANEL_LOCAL[Panel::TOP], 0)
    };

    const std::vector<cv::Mat> panelHomographyMats = hw4::CameraCalibration::find_panel_homography(panelCornerCoorsImage, panelCornerCoorsPanelLocal);
    const cv::Mat k = hw4::CameraCalibration::get_k_matrix(panelHomographyMats);
    return 0;
}