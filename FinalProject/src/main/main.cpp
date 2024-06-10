#include <opencv2/opencv.hpp>
#include <forward_list>
#include "xyzio.hpp"
#include "scanimageio.hpp"
#include "assetconfig.hpp"
#include "featurematching.hpp"
#include "directtriangulation.hpp"

template class std::forward_list<const cv::Point *>;

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

    finprj::DirectTriangulation pointProjection = finprj::DirectTriangulation(
        finprj::AssetConfig::LeftCameraRt,
        finprj::AssetConfig::RightCameraRt,
        finprj::AssetConfig::LeftCameraK,
        finprj::AssetConfig::RightCameraK
    );

    std::vector<std::vector<cv::Point3d>> allPredictedWorlPoints(scanImageIo.GetImageCount());

    for (size_t i = 0; i < scanImageIo.GetImageCount(); i++)
    {
        finprj::ImagePair currentImagePair = scanImageIo.GetPairByIndex(i);
        finprj::ImagePair nextImagePair = scanImageIo.GetPairByIndex((i + 1) % scanImageIo.GetImageCount());
        cv::Mat1b bluePixelMap;
        cv::Mat_<cv::Point> bluePixelCoors_left, bluePixelCoors_right;
        finprj::ScanImageIo::get_blue_pixel_mask(currentImagePair, nextImagePair, bluePixelMap, finprj::ScanImageIo::s_ModelRoiMask, true, 3, 1.55);
        finprj::ScanImageIo::get_blue_pixel_coors(bluePixelMap(currentImagePair.LeftRoi), bluePixelCoors_left);
        finprj::ScanImageIo::get_blue_pixel_coors(bluePixelMap(currentImagePair.RightRoi), bluePixelCoors_right);

        std::forward_list<const cv::Point *> validBluePixelCoors_left, validBluePixelCoors_right;
        finprj::FeatureMatching::find_corresponding_feature_point(bluePixelMap(currentImagePair.LeftRoi), bluePixelMap(currentImagePair.RightRoi), bluePixelCoors_left, bluePixelCoors_right);
        size_t validPointCount{0};
        finprj::FeatureMatching::remove_mismatched_point(bluePixelCoors_left, bluePixelCoors_right, validBluePixelCoors_left, validBluePixelCoors_right, validPointCount);

        allPredictedWorlPoints[i] = pointProjection.LocalToWorld(validBluePixelCoors_left, validBluePixelCoors_right, validPointCount);
        pointProjection.RemoveOutliners(allPredictedWorlPoints[i], validBluePixelCoors_left, validBluePixelCoors_right);
    }

    finprj::XyzIo::write_xyz(finprj::AssetConfig::PredictedXyzOutputPath, allPredictedWorlPoints);

    return 0;
}