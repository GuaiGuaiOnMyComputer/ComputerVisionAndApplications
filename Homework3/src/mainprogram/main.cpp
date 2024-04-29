#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <filesystem>
#include "assetcheck.hpp"
#include "pointprojection.hpp"
#include "xyzio.hpp"

template class cv::Mat_<float>;
template class cv::Mat_<uint8_t>;
template class std::vector<cv::Point2f>;
template class std::vector<cv::Point3f>;
template class std::vector<hw3::XyzIo::Coor2D_f>;
template class std::vector<hw3::XyzIo::Coor3D_f>;

int main(int , char**)
{
    const std::filesystem::path SANTA_XYZ_PATH("assets/Santa.xyz");
    const std::filesystem::path SANTA_JPG_PATH("assets/Santa.jpg");
    const std::filesystem::path SANTA_FEATURE_POINTS_2D_XYZ_PATH("assets/SantaFeaturePoints2D.xyz");
    const std::filesystem::path SANTA_FEATURE_POINTS_3D_XYZ_PATH("assets/SantaFeaturePoints3D.xyz");

    hw3::AssetCheck::check_if_asset_exist(
        {SANTA_XYZ_PATH, SANTA_JPG_PATH, SANTA_FEATURE_POINTS_2D_XYZ_PATH, SANTA_FEATURE_POINTS_3D_XYZ_PATH});
    std::vector<hw3::XyzIo::CoorAndNormal3D<float>> santaPointsAndNor3D = hw3::XyzIo::load_points_from_file<hw3::XyzIo::CoorAndNormal3D<float>, float>(SANTA_XYZ_PATH, 1);
    std::vector<hw3::XyzIo::Coor2D_f> santaPoints2d = hw3::XyzIo::load_points_from_file<hw3::XyzIo::Coor2D_f, float>(SANTA_FEATURE_POINTS_2D_XYZ_PATH, 0);
    std::vector<hw3::XyzIo::Coor3D_f> santaPoints3d = hw3::XyzIo::load_points_from_file<hw3::XyzIo::Coor3D_f, float>(SANTA_FEATURE_POINTS_3D_XYZ_PATH, 0);

    const cv::Mat projectionMat = hw3::PointProjection::get_projection_mat(santaPoints3d, santaPoints2d);
    std::vector<cv::Point2f> reprojectedSantaPointsOnImage = hw3::PointProjection::project_to_image(santaPointsAndNor3D, projectionMat);
    const cv::Mat image = cv::imread(SANTA_JPG_PATH.string());
    cv::Mat debugImage = hw3::PointProjection::show_projected_points(image, reprojectedSantaPointsOnImage);
    cv::namedWindow("DebugImage", cv::WindowFlags::WINDOW_FREERATIO);
    cv::imshow("DebugImage", debugImage);
    cv::waitKey(0);
    return 0;
}