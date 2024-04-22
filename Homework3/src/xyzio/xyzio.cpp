#include <fstream>
#include <iomanip>
#include <vector>
#include <array>
#include <algorithm>
#include <filesystem>
#include <stdint.h>
#include <charconv>
#include <opencv2/opencv.hpp>
#include "xyzio.hpp"

namespace hw3
{
    namespace fs = std::filesystem;

    XyzIo::PointCoorAndNormal::PointCoorAndNormal(std::array<double, Dimensions> xyzCoordinates, std::array<double, Dimensions> xyzNormals) noexcept
    {
        Coor = cv::Point3d(xyzCoordinates[0], xyzCoordinates[1], xyzCoordinates[2]);
        Normal = cv::Vec3d(xyzNormals[0], xyzNormals[1], xyzNormals[2]);
    }

    XyzIo::PointCoorAndNormal::PointCoorAndNormal(const std::array<double, Dimensions * Components>& data) noexcept: Coor{cv::Point3d(data[0], data[1], data[2])}, Normal{cv::Vec3d(data[3], data[4], data[5])}
    {
    }

    XyzIo::PointCoorAndNormal::PointCoorAndNormal() = default;
    XyzIo::PointCoorAndNormal::~PointCoorAndNormal(){};

    uint64_t XyzIo::_getLineCount(std::ifstream& fileHandle, const uint32_t emptyLineCount)
    {
        fileHandle.seekg(0, std::ios_base::beg);
        uint64_t lineCount = 1 + std::count(std::istreambuf_iterator<char>(fileHandle), std::istreambuf_iterator<char>(), '\n') - emptyLineCount;
        fileHandle.seekg(0, std::ios_base::beg);
        return lineCount;
    }

    std::error_code XyzIo::_create_output_directory_if_not_exist(const fs::path& filePath)
    {
        std::error_code errorCode;
        fs::create_directories(filePath, errorCode);
        return errorCode;
    }

} // namespace hw3
