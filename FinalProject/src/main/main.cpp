#include <opencv2/opencv.hpp>
#include "scanimageio.hpp"
#include "assetconfig.hpp"

int main(int, char**)
{
    std::error_code fileSystemErrorCode;
    finprj::ScanImageIo scanImageIo(finprj::AssetConfig::SideBySideImagePathRoot, finprj::AssetConfig::SideBySideImageFileExtension, fileSystemErrorCode);
    finprj::ImagePair imagePair = scanImageIo.GetPairByIndex(64);
    cv::Mat mask;
    while (true)
    {
        finprj::ScanImageIo::get_blue_pixel_mask(imagePair.Image, mask);
        cv::imshow("a mask", mask);
        if (cv::waitKey(0) == 69)
            break;
    }
    return 0;
}