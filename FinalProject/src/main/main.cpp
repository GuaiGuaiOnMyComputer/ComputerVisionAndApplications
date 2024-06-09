#include <opencv2/opencv.hpp>
#include <forward_list>
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

    for (size_t i = 0; i < scanImageIo.GetImageCount(); i++)
    {
        finprj::ImagePair currentImagePair = scanImageIo.GetPairByIndex(i);
        finprj::ImagePair nextImagePair = scanImageIo.GetPairByIndex(i + 1);
        cv::Mat1b bluePixelMap;
        cv::Mat_<cv::Point> bluePixelCoors_left, bluePixelCoors_right;
        finprj::ScanImageIo::get_blue_pixel_mask(currentImagePair, nextImagePair, bluePixelMap, finprj::ScanImageIo::s_ModelRoiMask, true, 3, 1.55);
        finprj::ScanImageIo::get_blue_pixel_coors(bluePixelMap(currentImagePair.LeftRoi), bluePixelCoors_left);
        finprj::ScanImageIo::get_blue_pixel_coors(bluePixelMap(currentImagePair.RightRoi), bluePixelCoors_right);
        finprj::FeatureMatching::find_corresponding_feature_point(bluePixelMap(currentImagePair.LeftRoi), bluePixelMap(currentImagePair.RightRoi), bluePixelCoors_left, bluePixelCoors_right);
        std::forward_list<const cv::Point *> validBluePixelCoors_left, validBluePixelCoors_right;
        size_t validPointsCount{0};
        std::vector<cv::Point3d> projectedWorldPoints = pointProjection.LocalToWorld(bluePixelCoors_left, bluePixelCoors_right, validBluePixelCoors_left, validBluePixelCoors_right, validPointsCount);
        const cv::Mat matchedFeaturePoints = finprj::FeatureMatching::draw_matching_points(currentImagePair.Image, bluePixelCoors_left, bluePixelCoors_right);
        cv::imshow("Matched points", matchedFeaturePoints);
        cv::waitKey(0);
        i %= scanImageIo.GetImageCount() - 2;
    }

    return 0;
}