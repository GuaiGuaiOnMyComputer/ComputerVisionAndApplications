#include <opencv2/opencv.hpp>
#include "scanimageio.hpp"
#include "assetconfig.hpp"

int main(int, char**)
{
    std::error_code fileSystemErrorCode;
    finprj::ScanImageIo scanImageIo(std::filesystem::path(finprj::AssetConfig::SideBySideImagePathRoot), finprj::AssetConfig::SideBySideImageFileExtension, fileSystemErrorCode);
    for (size_t i = 0; i < 150; i++)
    {
        finprj::ImagePair imgPair = scanImageIo.GetNextPair();
    }
    return 0;
}