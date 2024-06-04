#include <opencv2/opencv.hpp>
#include "scanimageio.hpp"
#include "assetconfig.hpp"
#include "directtriangulation.hpp"

int main(int, char**)
{
    std::error_code fileSystemErrorCode;
    finprj::ScanImageIo scanImageIo(finprj::AssetConfig::SideBySideImagePathRoot, finprj::AssetConfig::SideBySideImageFileExtension, fileSystemErrorCode);

    const std::array<double, 12> leftCameraP{
        2.0179, 1.5967, -0.5695, 113.8802,
        0.2820, -0.7636, -2.4258, 305.7125,
        -0.0009, 0.0023, -0.0018, 1.0000};

    const std::array<double, 12> rightCameraP{
        2.8143, -1.3450, -0.5673, 347.4957,
        -0.4439, -0.4444, -3.0134, 371.1864,
        0.0023, 0.0023, -0.0018, 1.000};

    // finprj::DirectTriangulation pointProjection(finprj::AssetConfig::LeftCameraRt, finprj::AssetConfig::RightCameraRt, finprj::AssetConfig::LeftCameraK, finprj::AssetConfig::RightCameraK, finprj::AssetConfig::FundementalMatrix);
    finprj::DirectTriangulation pointProjection(leftCameraP, rightCameraP);
    cv::Point3d pointInWorld = pointProjection.LocalToWorld(cv::Point2d(259, 120), cv::Point2d(395, 89));
    return 0;
}