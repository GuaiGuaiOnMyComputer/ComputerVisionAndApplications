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

// Compile the entire template class during debug and only compile the used functions when compiling in release mode.
#ifdef _DEBUG
template class std::vector<cv::Point2i>;
template class std::vector<cv::Point2f>;
template class std::array<cv::Point, 4>;
template class std::vector<std::vector<cv::Point>>;
#endif

namespace fs = std::filesystem;

bool checkFilesExist(const std::vector<fs::path>& filePaths);
void pleaseMakeSureThePolygonBoundedByTheseVerticsIsAboslutelyFilled(cv::InputOutputArray image, std::vector<cv::Point> &vertics);
cv::Mat catToLadyAndLadyToCat(cv::InputArray galleryImage, const std::vector<cv::Point2f>& catCornerPoints, const std::vector<cv::Point2f>& ladyCornerPoints);
std::vector<cv::Point> floatPointToIntPoint(const std::vector<cv::Point2f> &vtrOfFloatPoints);

int MAIN(int argc, char** agrv)
{
    // check if all the assets are present and load them all
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

    // parse the coordinates of the 4 vertices of the two paintings in the original image
    // the coordinates are written text files asset/cat_paint_corners.xyz and lady_paint_corners.xyz
    std::vector<cv::Point2f> ladyPaintCornerCoordinate = hw2::xyparser::GetCoordinateFromFile(LADY_COOR_FILE_PATH);
    std::vector<cv::Point2f> catPaintCornerCoordinate = hw2::xyparser::GetCoordinateFromFile(CAT_COOR_FILE_PATH);

    cv::Mat galleryImage = cv::imread(ART_GALLERY_FILE_PATH.string(), cv::ImreadModes::IMREAD_COLOR);

    // swap the two paintings in the image
    cv::Mat catAndLadyFaceSwapped = catToLadyAndLadyToCat(galleryImage, catPaintCornerCoordinate, ladyPaintCornerCoordinate);

    const fs::path OUTPUT_IMAGE_PATH = fs::path("M11225013.jpg");
    cv::imshow("Lady is cat and cat is lady", catAndLadyFaceSwapped);
    cv::imwrite(OUTPUT_IMAGE_PATH.string(), catAndLadyFaceSwapped);
    std::cout << "The output image is saved to: " << fs::absolute(OUTPUT_IMAGE_PATH) << std::endl;
    cv::waitKey(0);
    return 0;
}

/// @brief Checks if all the files at the specified paths are present
/// @param filePaths A vector of file paths to be examined.
/// @return True if all the specified files are present
bool checkFilesExist(const std::vector<fs::path>& filePaths) 
{
    return std::all_of(filePaths.cbegin(), filePaths.cend(), [](const fs::path &fp){ return fs::exists(fp); });
}

/// @brief Swaps the two paintings using homography transform
/// @param galleryImage The original image with the cat and the lady at where they are.
/// @param catCornerPoints Coordinates of the corner points of the cat painting in the original image.
/// @param ladyCornerPoints Cooridnates of the corner points of the lady painting in the original image.
/// @return The image with the lady and cat painting swapped.
cv::Mat catToLadyAndLadyToCat(cv::InputArray galleryImage, const std::vector<cv::Point2f>& catCornerPoints, const std::vector<cv::Point2f>& ladyCornerPoints)
{
    cv::Mat outputImageWithCatAndLadySwapped;
    cv::Mat galleryImageFloat = cv::Mat();
    cv::copyTo(galleryImage, outputImageWithCatAndLadySwapped, cv::noArray());

    // convert the cv::Point2f elements into cv::Point
    std::vector<cv::Point> catMaskVerticsInt = floatPointToIntPoint(catCornerPoints);
    std::vector<cv::Point> ladyMaskVerticsInt = floatPointToIntPoint(ladyCornerPoints);

    // create two masks each covering one of the two paintings
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

/// @brief Convert each cv::Point2f elements in vtrOfFloatPoints into cv::Point
/// @param vtrOfFloatPoints The vector of points to be converted 
/// @return The converted vtrOfFloatPoints
std::vector<cv::Point> floatPointToIntPoint(const std::vector<cv::Point2f>& vtrOfFloatPoints)
{
    std::vector<cv::Point> intPoints;
    intPoints.reserve(vtrOfFloatPoints.size());
    for(const cv::Point2f& ptFloat : vtrOfFloatPoints)
        intPoints.emplace_back((int)ptFloat.x, (int)ptFloat.y);
    return intPoints;
}

/// @brief Fills the polygon bounded by the 4 vertics in vertics.
/// @param image The image where the polygon will be filled.
/// @param vertics Vertices of the polygon.
void pleaseMakeSureThePolygonBoundedByTheseVerticsIsAboslutelyFilled(cv::InputOutputArray image, std::vector<cv::Point>& vertices)
{
    assert(vertices.size() == 4);

    // randomly select 3 out of the 4 points in the vertices vector, forming a triangle and fill it with a scalar
    // this makes sure the the area is filled as a solid polygon bounded by the 4 vertices, instead of a twisted polygon.
    std::vector<cv::Point> triangleVertics(3);
    std::array<uint32_t, 4> indics{0, 1, 2, 3};
    while (std::next_permutation(indics.begin(), indics.end()))
    {
        triangleVertics.at(0) = vertices[indics[0]];
        triangleVertics.at(1) = vertices[indics[1]];
        triangleVertics.at(2) = vertices[indics[2]];
        cv::fillConvexPoly(image, triangleVertics, cv::Scalar(255));
    }
}