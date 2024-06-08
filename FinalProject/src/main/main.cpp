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
        cv::Mat bluePixelMask;
        cv::Mat_<cv::Point> bluePixelCoors_left, bluePixelCoors_right;
        finprj::ScanImageIo::get_blue_pixel_mask(imagePair.Image, bluePixelMask);
        finprj::ScanImageIo::get_blue_pixel_coors(bluePixelMask(imagePair.LeftRoi), bluePixelCoors_left);
        finprj::ScanImageIo::get_blue_pixel_coors(bluePixelMask(imagePair.RightRoi), bluePixelCoors_right);
        finprj::FeatureMatching::find_corresponding_feature_point(imagePair.Left, imagePair.Right, bluePixelCoors_left, bluePixelCoors_right);
    }

    return 0;
}