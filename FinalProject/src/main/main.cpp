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
    const cv::Rect scanObjectRoiLeft = cv::Rect(finprj::AssetConfig::ScanObjectLeftRoi[0], finprj::AssetConfig::ScanObjectLeftRoi[1], finprj::AssetConfig::ScanObjectLeftRoi[2], finprj::AssetConfig::ScanObjectLeftRoi[3]);
    finprj::ScanImageIo::init_roi_mask(
        scanObjectRoiLeft, 
        finprj::AssetConfig::SideBySideImageWidth, 
        finprj::AssetConfig::SideBySideImageHeight);

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

    for (size_t i = 14; i < scanImageIo.GetImageCount(); i++)
    {
        finprj::ImagePair imagePair = scanImageIo.GetPairByIndex(i);
        cv::Mat bluePixelMask;
        cv::Mat_<cv::Point> bluePixelCoors_left, bluePixelCoors_right;
        finprj::ScanImageIo::get_blue_pixel_mask(imagePair.Image, bluePixelMask, finprj::ScanImageIo::s_ModelRoiMask, true);
        finprj::ScanImageIo::get_blue_pixel_coors(bluePixelMask(imagePair.LeftRoi), bluePixelCoors_left);
        finprj::ScanImageIo::get_blue_pixel_coors(bluePixelMask(imagePair.RightRoi), bluePixelCoors_right);
        finprj::FeatureMatching::find_corresponding_feature_point(bluePixelMask(imagePair.LeftRoi), bluePixelMask(imagePair.RightRoi), bluePixelCoors_left, bluePixelCoors_right);
        const cv::Mat matchedFeaturePoints = finprj::FeatureMatching::draw_matching_points(imagePair.Image, bluePixelCoors_left, bluePixelCoors_right);
        cv::imshow("Matched features", matchedFeaturePoints);
        cv::waitKey(0);
    }

    return 0;
}