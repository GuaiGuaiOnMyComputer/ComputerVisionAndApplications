#include <opencv2/opencv.hpp>
#include "scanimageio.hpp"
#include "assetconfig.hpp"
#include "directtriangulation.hpp"

int main(int, char**)
{
    std::error_code fileSystemErrorCode;
    finprj::ScanImageIo scanImageIo(finprj::AssetConfig::SideBySideImagePathRoot, finprj::AssetConfig::SideBySideImageFileExtension, fileSystemErrorCode);
    finprj::DirectTriangulation(finprj::AssetConfig::LeftCameraRt, finprj::AssetConfig::RightCameraRt, finprj::AssetConfig::LeftCameraMatrix, finprj::AssetConfig::RightCameraMatrix, finprj::AssetConfig::FundementalMatrix);
    return 0;
}