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
    std::vector<XyzIo::Coor2D<float>> santaPoints2d = XyzIo::load_points_from_file<XyzIo::Coor2D<float>, float>(SANTA_FEATURE_POINTS_2D_XYZ_PATH, 0);
    std::vector<XyzIo::Coor3D<float>> santaPoints3d = XyzIo::load_points_from_file<XyzIo::Coor3D<float>, float>(SANTA_FEATURE_POINTS_3D_XYZ_PATH, 0);

    cv::Mat cameraMatrix, distortionCoeff, rotationVector, translationVector;
    const cv::Size IMG_SIZE(2747, 1835);
    std::vector<std::vector<cv::Point3f>> tmpA(1);
    for (XyzIo::Coor3D pt : santaPoints3d)
    {
        tmpA[0].emplace_back(pt.x, pt.y, pt.z);
    }
    std::vector<std::vector<cv::Point2f>> tmpB(1);
    for (XyzIo::Coor2D pt : santaPoints2d)
    {
        tmpB[0].emplace_back(pt.x, pt.y);
    }
    cv::calibrateCamera(tmpA, tmpB, IMG_SIZE, cameraMatrix, distortionCoeff, rotationVector, translationVector);

    return 0;
}