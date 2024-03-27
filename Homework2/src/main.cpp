#include <iostream>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <opencv2/core.hpp>
#include "coordinateparser.cpp"

#define MAIN wWinMain

#ifdef _DEBUG
template class std::vector<cv::Point2i>;
#endif

namespace fs = std::filesystem;

bool checkFilesExist(const std::vector<fs::path> filePaths) 
{
    return std::all_of(filePaths.cbegin(), filePaths.cend(), [](const fs::path &fp){ return fs::exists(fp); });
}

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

    std::vector<cv::Point2i> ladyPaintCornerCoordinate = hw2::xyparser::GetCoordinateFromFile(LADY_COOR_FILE_PATH);
    std::vector<cv::Point2i> catPaintCornerCoordinate = hw2::xyparser::GetCoordinateFromFile(CAT_COOR_FILE_PATH);
    return 0;
}

