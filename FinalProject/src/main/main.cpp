#include <opencv2/opencv.hpp>
#include "scanimageio.hpp"
#include "assetconfig.hpp"

int main(int, char**)
{
    std::error_code fileSystemErrorCode;
    finprj::ScanImageIo scanImageIo(std::filesystem::path(finprj::AssetConfig::SideBySideImagePathRoot), finprj::AssetConfig::SideBySideImageFileExtension, fileSystemErrorCode);
    finprj::ImagePair pair = scanImageIo.GetNextPair();
    cv::Mat mask;
    finprj::ScanImageIo::get_blue_pixel_mask(pair.Left, mask);
    cv::imshow("a mask", mask);
    cv::waitKey(0);
    return 0;
}