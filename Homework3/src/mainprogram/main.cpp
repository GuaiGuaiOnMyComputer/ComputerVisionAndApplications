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
    const std::filesystem::path SANTA_FEATURE_POINTS_2D_XYZ_PATH("assets/SantaFeaturePoints2D.xyz");
    const std::filesystem::path SANTA_FEATURE_POINTS_3D_XYZ_PATH("assets/SantaFeaturePoints3D.xyz");

    AssetCheck::check_if_asset_exist(
        {SANTA_XYZ_PATH, SANTA_JPG_PATH, SANTA_FEATURE_POINTS_2D_XYZ_PATH, SANTA_FEATURE_POINTS_3D_XYZ_PATH});
    std::vector<XyzIo::CoorAndNormal3D<float>> santaPointsAndNor3D = XyzIo::load_points_from_file<XyzIo::CoorAndNormal3D<float>, float>(SANTA_XYZ_PATH, 1);
    std::vector<XyzIo::Coor2D_f> santaPoints2d = XyzIo::load_points_from_file<XyzIo::Coor2D_f, float>(SANTA_FEATURE_POINTS_2D_XYZ_PATH, 0);
    std::vector<XyzIo::Coor3D_f> santaPoints3d = XyzIo::load_points_from_file<XyzIo::Coor3D_f, float>(SANTA_FEATURE_POINTS_3D_XYZ_PATH, 0);

    cv::Mat cameraMatrix, distortionCoeff, rotationVector, translationVector;
    const cv::Size IMG_SIZE(2747, 1835);
    cv::calibrateCamera(santaPoints3d, santaPoints2d, IMG_SIZE, cameraMatrix, distortionCoeff, rotationVector, translationVector);

    return 0;
}