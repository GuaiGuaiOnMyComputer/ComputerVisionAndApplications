#include <opencv2/opencv.hpp>
#include "scanimageio.hpp"
#include "assetconfig.hpp"
#include "featurematching.hpp"
#include "featurematching.hpp"
#include "directtriangulation.hpp"

int main(int, char**)
{
    std::error_code fileSystemErrorCode;

    finprj::ScanImageIo scanImageIo = finprj::ScanImageIo(
        finprj::AssetConfig::SideBySideImagePathRoot,
        finprj::AssetConfig::SideBySideImageFileExtension,
        fileSystemErrorCode
    );

    //TODO: remove debug code
    size_t i = 0;
    while (i < 120)
    {
        finprj::ImagePair imagePair = scanImageIo.GetPairByIndex(i);
        cv::Mat mask;
        scanImageIo.get_blue_pixel_mask(imagePair.Image, mask);
        cv::imshow("ImagePair", mask);
        i++;
        i %= 120;
        if (cv::waitKey(0) == 69)
            break;
    }

    finprj::DirectTriangulation pointProjection = finprj::DirectTriangulation(
        finprj::AssetConfig::LeftCameraRt,
        finprj::AssetConfig::RightCameraRt,
        finprj::AssetConfig::LeftCameraK,
        finprj::AssetConfig::RightCameraK
    );

    for (size_t i = 0; i < scanImageIo.GetImageCount(); i++)
    {
        finprj::ImagePair imagePair = scanImageIo.GetPairByIndex(i);
        // TODO: implement testing code
    }

    return 0;
}