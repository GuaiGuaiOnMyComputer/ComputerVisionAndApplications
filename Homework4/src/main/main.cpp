#include <opencv2/opencv.hpp>
#include <filesystem>
#include <array>
#include "assetcheck.hpp"
#include "cameracalibration.hpp"
#include "debugcode.hpp"

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
    std::array<fs::path, Panel::PANEL_COUNT> panelCornersPathsImage;
    panelCornersPathsImage[Panel::LEFT] = fs::path("../assets/leftPanelCorners_Image.xyz");
    panelCornersPathsImage[Panel::RIGHT] = fs::path("../assets/rightPanelCorners_Image.xyz");
    panelCornersPathsImage[Panel::TOP] = fs::path("../assets/topPanelCorners_Image.xyz");

    std::array<fs::path, Panel::PANEL_COUNT> panelCornerPathsPanelLocal;
    panelCornerPathsPanelLocal[Panel::LEFT] = fs::path("../assets/leftPanelCorners_PanelLocal.xyz");
    panelCornerPathsPanelLocal[Panel::RIGHT] = fs::path("../assets/rightPanelCorners_PanelLocal.xyz");
    panelCornerPathsPanelLocal[Panel::TOP] = fs::path("../assets/topPanelCorners_PanelLocal.xyz");

    hw4::AssetCheck::check_if_asset_exist(panelCornersPathsImage);
    hw4::AssetCheck::check_if_asset_exist(panelCornerPathsPanelLocal);

    std::array<std::vector<hw4::XyzIo::Coor2D_f>, Panel::PANEL_COUNT> panelCornerCoorsImage;
    std::array<std::vector<hw4::XyzIo::Coor2D_f>, Panel::PANEL_COUNT> panelCornerCoorsPanelLocal;
    for (size_t i = 0; i < Panel::PANEL_COUNT; i ++)
    {
        panelCornerCoorsImage[i] = hw4::XyzIo::load_points_from_file<hw4::XyzIo::Coor2D_f, float>(panelCornersPathsImage[i], 0);
        panelCornerCoorsPanelLocal[i] = hw4::XyzIo::load_points_from_file<hw4::XyzIo::Coor2D_f, float>(panelCornerPathsPanelLocal[i], 0);
    }

    const std::vector<cv::Mat> panelHomographyMats = hw4::CameraCalibration::find_panel_homography(panelCornerCoorsImage, panelCornerCoorsPanelLocal);
    const cv::Mat k = hw4::CameraCalibration::get_k_matrix(panelHomographyMats);

    return 0;
}