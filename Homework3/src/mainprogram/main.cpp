#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <filesystem>
#include "assetcheck.hpp"
#include "pointprojection.hpp"
#include "xyzio.hpp"

int main(int , char**)
{
    // set the input file paths of required assets
    const std::filesystem::path SANTA_XYZ_PATH("../assets/Santa.xyz");
    const std::filesystem::path SANTA_JPG_PATH("../assets/Santa.jpg");
    const std::filesystem::path SANTA_FEATURE_POINTS_2D_XYZ_PATH("../assets/SantaFeaturePoints2D.xyz");
    const std::filesystem::path SANTA_FEATURE_POINTS_3D_XYZ_PATH("../assets/SantaFeaturePoints3D.xyz");

    hw3::AssetCheck::check_if_asset_exist(
        {SANTA_XYZ_PATH, SANTA_JPG_PATH, SANTA_FEATURE_POINTS_2D_XYZ_PATH, SANTA_FEATURE_POINTS_3D_XYZ_PATH});

    // parse the xyz coordinates and normal vectors of all the points of the entire sculpture as floats and store them into a vector of CoorAndNormal3D<float>
    const std::vector<hw3::XyzIo::CoorAndNormal3D<float>> santaPointsAndNor3D = hw3::XyzIo::load_points_from_file<hw3::XyzIo::CoorAndNormal3D<float>, float>(SANTA_XYZ_PATH, 1);

    // parse the xyz coordinates of the 8 selected 2D and 3D feature points, and store them into a vector of Coor2D_f
    const std::vector<hw3::XyzIo::Coor2D_f> santaPoints2d = hw3::XyzIo::load_points_from_file<hw3::XyzIo::Coor2D_f, float>(SANTA_FEATURE_POINTS_2D_XYZ_PATH, 0);
    const std::vector<hw3::XyzIo::Coor3D_f> santaPoints3d = hw3::XyzIo::load_points_from_file<hw3::XyzIo::Coor3D_f, float>(SANTA_FEATURE_POINTS_3D_XYZ_PATH, 0);

    // read the image of the santa sculpture as a 3-channel cv::Mat 
    const cv::Mat santaPhoto = cv::imread(SANTA_JPG_PATH.string());

    // export an image of the santa photo marked with all the selected 2D feature points in cyan dots
    hw3::PointProjection::save_image_with_selected_points("../output/selected_points.jpg", santaPhoto, santaPoints2d);

    // calculate the camera projection matrix that maps 3D world points into 2D images
    const cv::Mat projectionMat = hw3::PointProjection::get_projection_mat(santaPoints3d, santaPoints2d);

    // create a vector of CoorAndNormal3D_f that stores the coordinates and the normal vectors of 3D points that is not occluded 
    std::vector<hw3::XyzIo::CoorAndNormal3D_f> nonOccludedPoints;

    // camera is pointing into the world (0, -1, 0) direction
    const cv::Vec3f CAMERA_POINTING_VTR_WOLRD(0, -1, 0); 

    // find the 2D coordinates of the non-occluded 3D points in image
    const std::vector<cv::Point2f> santaPointsInImage = hw3::PointProjection::project_to_image(santaPointsAndNor3D, projectionMat, false, CAMERA_POINTING_VTR_WOLRD, nonOccludedPoints);

    // read the RGB pixel values of the image at the specified image coordinate.
    const std::vector<hw3::XyzIo::Rgb_ui8> rgbValues = hw3::PointProjection::get_rgb_from_2d_coordinate(santaPointsInImage, santaPhoto);
    hw3::XyzIo::write_xyz_normal_and_rgb("../output/m11225013.txt", nonOccludedPoints, rgbValues);
    return 0;
}