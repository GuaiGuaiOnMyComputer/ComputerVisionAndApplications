#include <fstream>
#include <vector>
#include <array>
#include <algorithm>
#include <filesystem>
#include <stdint.h>
#include <opencv2/opencv.hpp>
#include "xyzio.hpp"

namespace midproj
{
    namespace fs = std::filesystem;

    std::vector<cv::Point2i> XyzIo::load_points_from_file_2i(const fs::path& filePath)
    {
        std::ifstream fileHandle(filePath, std::ifstream::in);
        uint64_t pointCount = _getLineCount(fileHandle);
        std::vector<cv::Point2i> parsedCoordinates;
        parsedCoordinates.reserve(pointCount);

        std::string lineContent;
        for (size_t i = 0; i < pointCount; i++)
        {
            std::getline(fileHandle, lineContent);
            parsedCoordinates.push_back(_parseLineAsPoint_2i(lineContent));
        }
        return parsedCoordinates;
    }

    std::vector<cv::Point2f> XyzIo::load_points_from_file_2f(const fs::path& filePath)
    {
        std::ifstream fileHandle(filePath, std::ifstream::in);
        uint64_t pointCount = _getLineCount(fileHandle);
        std::vector<cv::Point2f> parsedCoordinates;
        parsedCoordinates.reserve(pointCount);

        std::string lineContent;
        for (size_t i = 0; i < pointCount; i++)
        {
            std::getline(fileHandle, lineContent);
            parsedCoordinates.push_back(_parseLineAsPoint_2f(lineContent));
        }
        return parsedCoordinates;
    }

    cv::Point2i XyzIo::_parseLineAsPoint_2i(const std::string& lineContent)
    {
        size_t intStart, intEnd;
        int32_t x, y;
        const char *stringEndPtr = lineContent.data() + lineContent.size();
        std::from_chars_result parsedResult = std::from_chars(lineContent.data(), stringEndPtr, x);
        std::from_chars(parsedResult.ptr + 1, stringEndPtr, y); // +1 to skip the \t delimiter
        return cv::Point2i(x, y);
    }

    cv::Point2f XyzIo::_parseLineAsPoint_2f(const std::string& lineContent)
    {
        size_t intStart, intEnd;
        int32_t x, y;
        const char *stringEndPtr = lineContent.data() + lineContent.size();
        std::from_chars_result parsedResult = std::from_chars(lineContent.data(), stringEndPtr, x);
        std::from_chars(parsedResult.ptr + 1, stringEndPtr, y); // +1 to skip the \t delimiter
        return cv::Point2f(x, y);
    }

    uint64_t XyzIo::_getLineCount(std::ifstream& fileHandle)
    {
        fileHandle.seekg(0, std::ios_base::beg);
        uint64_t lineCount = 1 + std::count(std::istreambuf_iterator<char>(fileHandle), std::istreambuf_iterator<char>(), '\n');
        fileHandle.seekg(0, std::ios_base::beg);
        return lineCount;
    }

} // namespace midproj
