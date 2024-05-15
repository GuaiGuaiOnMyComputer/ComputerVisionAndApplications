#include <opencv2/opencv.hpp>
#include <filesystem>
#include <array>
#include "assetcheck.hpp"
#include "cameracalibration.hpp"


template class std::vector<hw4::XyzIo::Coor2D_f>;
template class std::vector<hw4::XyzIo::Coor3D_f>;

int main(int, char**)
{
    namespace fs = std::filesystem;
    const std::array<fs::path, hw4::CameraCalibration::Panel::PANEL_COUNT> PANEL_CORNERS_PATHS_2D{
        fs::path("../assets/leftPanelCorners2D.xyz"),
        fs::path("../assets/rightPanelCorners2D.xyz"),
        fs::path("../assets/topPanelCorners2D.xyz"),
    };
    const std::array<fs::path, hw4::CameraCalibration::Panel::PANEL_COUNT> PANEL_CORNERS_PATHS_3D{
        fs::path("../assets/leftPanelCorners3D.xyz"),
        fs::path("../assets/rightPanelCorners3D.xyz"),
        fs::path("../assets/topPanelCorners3D.xyz"),
    };
    hw4::AssetCheck::check_if_asset_exist(PANEL_CORNERS_PATHS_2D);
    hw4::AssetCheck::check_if_asset_exist(PANEL_CORNERS_PATHS_3D);

    std::array<std::vector<hw4::XyzIo::Coor2D_f>, hw4::CameraCalibration::Panel::PANEL_COUNT> panelCornerCoors2d;
    std::array<std::vector<hw4::XyzIo::Coor3D_f>, hw4::CameraCalibration::Panel::PANEL_COUNT> panelCornerCoors3d;
    for (size_t i = 0; i < hw4::CameraCalibration::Panel::PANEL_COUNT;i ++)
    {
        panelCornerCoors2d[i] = hw4::XyzIo::load_points_from_file<hw4::XyzIo::Coor2D_f, float>(PANEL_CORNERS_PATHS_2D[i], 0);
        panelCornerCoors3d[i] = hw4::XyzIo::load_points_from_file<hw4::XyzIo::Coor3D_f, float>(PANEL_CORNERS_PATHS_3D[i], 0);
    }

    return 0;
}