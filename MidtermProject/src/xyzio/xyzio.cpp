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

namespace midproj
{
    namespace fs = std::filesystem;

    bool XyzIo::write_xyz_point_cloud_file(const fs::path& filePath, const std::vector<std::vector<cv::Point3d>>& pointsInAllSlices)
    {
        _create_output_directory_if_not_exist(filePath.parent_path());
        std::ofstream outputFile(filePath, std::ios_base::out);
        outputFile.seekp(0, std::ios_base::beg);

        for (const std::vector<cv::Point3d>& pointsInSlice : pointsInAllSlices)
        {
            for (const cv::Point3d& point : pointsInSlice)
            {
                outputFile << std::setprecision(4) << point.x << ' ' << point.y << ' ' << point.z << '\n';
            }
        }
        outputFile.close();
        return true;
    }

    std::vector<cv::Point2i> XyzIo::load_points_from_file_2i(const fs::path& filePath)
    {
        std::ifstream fileHandle(filePath, std::ifstream::in);
        uint64_t pointCount = _getLineCount(fileHandle);
        std::vector<cv::Point2i> parsedCoordinates(pointCount);

        std::string lineContent;
        for (size_t i = 0; i < pointCount; i++)
        {
            std::getline(fileHandle, lineContent);
            parsedCoordinates.at(i) = _parseLineAsPoint_2i(lineContent);
        }
        return parsedCoordinates;
    }

    std::vector<cv::Point2f> XyzIo::load_points_from_file_2f(const fs::path& filePath)
    {
        std::ifstream fileHandle(filePath, std::ifstream::in);
        uint64_t pointCount = _getLineCount(fileHandle);
        std::vector<cv::Point2f> parsedCoordinates(pointCount);

        std::string lineContent;
        for (size_t i = 0; i < pointCount; i++)
        {
            std::getline(fileHandle, lineContent);
            parsedCoordinates.at(i) = _parseLineAsPoint_2f(lineContent);
        }
        return parsedCoordinates;
    }

    std::vector<cv::Point3f> XyzIo::load_points_from_file_3f(const fs::path& filePath)
    {
        std::ifstream fileHandle(filePath, std::ifstream::in);
        uint64_t pointCount = _getLineCount(fileHandle);
        std::vector<cv::Point3f> parsedCoordinates(pointCount);

        std::string lineContent;
        for (size_t i = 0; i < pointCount; i++)
        {
            std::getline(fileHandle, lineContent);
            parsedCoordinates.at(i) = _parseLineAsPoint_3f(lineContent);
        }
        return parsedCoordinates;
    }
    
    std::vector<cv::Point3i> XyzIo::load_points_from_file_3i(const fs::path& filePath)
    {
        std::ifstream fileHandle(filePath, std::ifstream::in);
        uint64_t pointCount = _getLineCount(fileHandle);
        std::vector<cv::Point3i> parsedCoordinates(pointCount);

        std::string lineContent;
        for (size_t i = 0; i < pointCount; i++)
        {
            std::getline(fileHandle, lineContent);
            parsedCoordinates.at(i) = _parseLineAsPoint_3i(lineContent);
        }
        return parsedCoordinates;
    }

    
    cv::Point2i XyzIo::_parseLineAsPoint_2i(const std::string& lineContent)
    {
        int32_t x, y;
        const char *stringEndPtr = lineContent.data() + lineContent.size();
        std::from_chars_result parsedResult = std::from_chars(lineContent.data(), stringEndPtr, x);
        parsedResult = std::from_chars(parsedResult.ptr + 1, stringEndPtr, y); // +1 to skip the \t delimiter
        return cv::Point2i(x, y);
    }

    cv::Point2f XyzIo::_parseLineAsPoint_2f(const std::string& lineContent)
    {
        float x, y;
        const char *stringEndPtr = lineContent.data() + lineContent.size();
        std::from_chars_result parsedResult = std::from_chars(lineContent.data(), stringEndPtr, x);
        parsedResult = std::from_chars(parsedResult.ptr + 1, stringEndPtr, y); // +1 to skip the \t delimiter
        return cv::Point2i(x, y);
    }

    cv::Point3i XyzIo::_parseLineAsPoint_3i(const std::string& lineContent)
    {
        int32_t x, y, z;
        const char *stringEndPtr = lineContent.data() + lineContent.size();
        std::from_chars_result parsedResult = std::from_chars(lineContent.data(), stringEndPtr, x);
        parsedResult = std::from_chars(parsedResult.ptr + 1, stringEndPtr, y); // +1 to skip the \t delimiter
        parsedResult = std::from_chars(parsedResult.ptr + 1, stringEndPtr, z); // +1 to skip the \t delimiter
        return cv::Point3i(x, y, z);
    }

    cv::Point3f XyzIo::_parseLineAsPoint_3f(const std::string& lineContent)
    {
        float x, y, z;
        const char *stringEndPtr = lineContent.data() + lineContent.size();
        std::from_chars_result parsedResult = std::from_chars(lineContent.data(), stringEndPtr, x);
        parsedResult = std::from_chars(parsedResult.ptr + 1, stringEndPtr, y); // +1 to skip the \t delimiter
        parsedResult = std::from_chars(parsedResult.ptr + 1, stringEndPtr, z); // +1 to skip the \t delimiter
        return cv::Point3f(x, y, z);
    }

    uint64_t XyzIo::_getLineCount(std::ifstream& fileHandle)
    {
        fileHandle.seekg(0, std::ios_base::beg);
        uint64_t lineCount = 1 + std::count(std::istreambuf_iterator<char>(fileHandle), std::istreambuf_iterator<char>(), '\n');
        fileHandle.seekg(0, std::ios_base::beg);
        return lineCount;
    }

    std::error_code XyzIo::_create_output_directory_if_not_exist(const fs::path& filePath)
    {
        std::error_code errorCode;
        fs::create_directories(filePath, errorCode);
        return errorCode;
    }

} // namespace midproj
