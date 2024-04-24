#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <filesystem>
#include "assetcheck.hpp"
#include "xyzio.hpp"

int main(int , char**)
{
    using namespace hw3;
    const std::filesystem::path SANTA_XYZ_PATH("assets/Santa.xyz");
    const std::filesystem::path SANTA_JPG_PATH("assets/Santa.jpg");

    AssetCheck::check_if_asset_exist({SANTA_XYZ_PATH, SANTA_JPG_PATH});
    std::vector<XyzIo::PointCoorAndNormal> vtr = XyzIo::load_points_from_file<XyzIo::PointCoorAndNormal>(std::filesystem::path("assets//Santa.xyz"), 1);
    return 0;
}