#include <iostream>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include "coordinateparser.cpp"

#define MAIN wWinMain

#ifdef _DEBUG
template class std::vector<cv::Point2i>;
template class std::vector<cv::Point2f>;
template class std::array<cv::Point, 4>;
template class std::vector<std::vector<cv::Point>>;
#endif

namespace fs = std::filesystem;

bool checkFilesExist(const std::vector<fs::path> filePaths);
void pleaseMakeSureThePolygonBoundedByTheseVerticsIsAboslutelyFilled(cv::InputOutputArray image, std::vector<cv::Point> &vertics);
cv::Mat catToLadyAndLadyToCat(cv::Mat galleryImage, const std::vector<cv::Point2f>& catCornerPoints, const std::vector<cv::Point2f>& ladyCornerPoints);
std::vector<cv::Point> floatPointToIntPoint(const std::vector<cv::Point2f> &vtrOfFloatPoints);

int MAIN(int argc, char** agrv)
{
    const fs::path LADY_COOR_FILE_PATH = fs::path("asset/lady_paint_corners.xyz");
    const fs::path CAT_COOR_FILE_PATH = fs::path("asset/cat_paint_corners.xyz");
    const fs::path ART_GALLERY_FILE_PATH = fs::path("asset/Swap_ArtGallery.jpg");
    assert(
        checkFilesExist(
        {
            LADY_COOR_FILE_PATH,
            CAT_COOR_FILE_PATH,
            ART_GALLERY_FILE_PATH
        })
    );

    std::vector<cv::Point2f> ladyPaintCornerCoordinate = hw2::xyparser::GetCoordinateFromFile(LADY_COOR_FILE_PATH);
    std::vector<cv::Point2f> catPaintCornerCoordinate = hw2::xyparser::GetCoordinateFromFile(CAT_COOR_FILE_PATH);

    cv::Mat galleryImage = cv::imread(ART_GALLERY_FILE_PATH.string(), cv::ImreadModes::IMREAD_COLOR);
    cv::Mat catAndLadyFaceSwapped = catToLadyAndLadyToCat(galleryImage, catPaintCornerCoordinate, ladyPaintCornerCoordinate);

    cv::imshow("Lady is cat and cat is lady", catAndLadyFaceSwapped);
    cv::waitKey(0);
    return 0;
}

bool checkFilesExist(const std::vector<fs::path> filePaths) 
{
    return std::all_of(filePaths.cbegin(), filePaths.cend(), [](const fs::path &fp){ return fs::exists(fp); });
}

cv::Mat catToLadyAndLadyToCat(cv::Mat galleryImage, const std::vector<cv::Point2f>& catCornerPoints, const std::vector<cv::Point2f>& ladyCornerPoints)
{
    cv::Mat outputImageWithCatAndLadySwapped = galleryImage.clone();
    cv::Mat galleryImageFloat = cv::Mat();

    std::vector<cv::Point> catMaskVerticsInt = floatPointToIntPoint(catCornerPoints);
    std::vector<cv::Point> ladyMaskVerticsInt = floatPointToIntPoint(ladyCornerPoints);

    cv::Mat catAreaMask(galleryImage.size(), CV_8UC1);
    cv::Mat ladyAreaMask(galleryImage.size(), CV_8UC1);
    pleaseMakeSureThePolygonBoundedByTheseVerticsIsAboslutelyFilled(catAreaMask, catMaskVerticsInt);
    pleaseMakeSureThePolygonBoundedByTheseVerticsIsAboslutelyFilled(ladyAreaMask, ladyMaskVerticsInt);

    cv::Mat tmp;
    cv::Mat catToLadyHomographyMat = cv::findHomography(catCornerPoints, ladyCornerPoints, 0);
    cv::Mat catOnLady = cv::Mat::zeros(galleryImageFloat.size(), galleryImageFloat.type());
    cv::warpPerspective(galleryImage, tmp, catToLadyHomographyMat, galleryImage.size());
    cv::copyTo(tmp, outputImageWithCatAndLadySwapped, ladyAreaMask);

    cv::Mat ladyToCatHomographyMat = cv::findHomography(ladyCornerPoints, catCornerPoints, 0);
    cv::Mat ladyOnCat = cv::Mat::zeros(galleryImageFloat.size(), galleryImageFloat.type());
    cv::warpPerspective(galleryImage, tmp, ladyToCatHomographyMat, galleryImage.size());
    cv::copyTo(tmp, outputImageWithCatAndLadySwapped, catAreaMask);

    return outputImageWithCatAndLadySwapped;
}

std::vector<cv::Point> floatPointToIntPoint(const std::vector<cv::Point2f>& vtrOfFloatPoints)
{
    std::vector<cv::Point> intPoints;
    intPoints.reserve(vtrOfFloatPoints.size());
    for(const cv::Point2f& ptFloat : vtrOfFloatPoints)
        intPoints.emplace_back((int)ptFloat.x, (int)ptFloat.y);
    return intPoints;
}

void pleaseMakeSureThePolygonBoundedByTheseVerticsIsAboslutelyFilled(cv::InputOutputArray image, std::vector<cv::Point>& vertics)
{
    std::vector<cv::Point> triangleVertics(3);
    std::array<uint32_t, 4> indics{0, 1, 2, 3};
    while (std::next_permutation(indics.begin(), indics.end()))
    {
        triangleVertics.at(0) = vertics[indics[0]];
        triangleVertics.at(1) = vertics[indics[1]];
        triangleVertics.at(2) = vertics[indics[2]];
        cv::fillConvexPoly(image, triangleVertics, cv::Scalar(255));
    }
}