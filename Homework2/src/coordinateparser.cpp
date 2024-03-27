#include <filesystem>
#include <vector>
#include <fstream>
#include <stdint.h>
#include <string>
#include <iostream>
#include <charconv>
#include <opencv2/core.hpp>

namespace hw2
{
    namespace fs = std::filesystem;

    class xyparser
    {
    public: 
        static std::vector<cv::Point2f> GetCoordinateFromFile(const fs::path& filePath)
        {
            std::ifstream fileHandle(filePath, std::ifstream::in);
            uint32_t pointCount = _getLineCount(fileHandle);
            std::vector<cv::Point2f> parsedCoordinates;
            parsedCoordinates.reserve(pointCount);

            std::string lineContent;
            for (size_t i = 0; i < pointCount; i++)
            {
                std::getline(fileHandle, lineContent);
                parsedCoordinates.push_back(_parseLineAsPoint(lineContent));
            }
            return parsedCoordinates;
        }

    private:

        static uint32_t _getLineCount(std::ifstream& fileHandle)
        {
            fileHandle.seekg(0, std::ios_base::beg);
            uint32_t lineCount = 1 + std::count(std::istreambuf_iterator<char>(fileHandle), std::istreambuf_iterator<char>(), '\n');
            fileHandle.seekg(0, std::ios_base::beg);
            return lineCount;
        }

        static cv::Point2f _parseLineAsPoint(const std::string& lineContent)
        {
            size_t intStart, intEnd;
            int32_t x, y;
            const char *stringEndPtr = lineContent.data() + lineContent.size();
            std::from_chars_result parsedResult = std::from_chars(lineContent.data(), stringEndPtr, x);
            std::from_chars(parsedResult.ptr + 1, stringEndPtr, y); // +1 to skip the \t delimiter
            return cv::Point2f(x, y);
        }

        xyparser() = delete;
    };
}