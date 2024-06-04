#include <opencv2/opencv.hpp>
#include "scanimageio.hpp"
#include "assetconfig.hpp"
#include "directtriangulation.hpp"

int main(int, char**)
{
    std::error_code fileSystemErrorCode;

    finprj::ScanImageIo scanImageIo = finprj::ScanImageIo(
        finprj::AssetConfig::SideBySideImagePathRoot,
        finprj::AssetConfig::SideBySideImageFileExtension,
        fileSystemErrorCode
    );

    finprj::DirectTriangulation pointProjection = finprj::DirectTriangulation(
        finprj::AssetConfig::LeftCameraRt,
        finprj::AssetConfig::RightCameraRt,
        finprj::AssetConfig::LeftCameraK,
        finprj::AssetConfig::RightCameraK,
        finprj::AssetConfig::FundementalMatrix
    );

    return 0;
}