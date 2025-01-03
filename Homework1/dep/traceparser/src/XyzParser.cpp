#include <filesystem>
#include <string>
#include <array>
#include <vector>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <opencv2/core.hpp>
// #include "XyzParser.hpp"
// TODO: include header file instead


/// @brief A parser for xyz files that encodes each point's x, y, z component as 3 floats in each line.
class XyzParser
{
public:
    XyzParser(const std::filesystem::path xyzFilePath) : _xyzFilePath{xyzFilePath}, _fileExist{false}, _dataStart{0}
    {
        _fileBasename = _xyzFilePath.filename();
        _fileExist = std::filesystem::exists(_xyzFilePath);
        _fileHandle = std::ifstream();
    }

    /// @brief Read the entire file and returns the parsed floats as xyz coordinates. The resultant vector will be have each element as a loaded point stored as cv::Point3f.
    /// @param out_trackPoints An output variable storing all the parsed floats.
    /// @param headerLen Number of characters in the file header. Skip the file header without parsing it as floats.
    void ParseAll(std::vector<cv::Point3f>& out_trackPoints, int64_t headerLen = -1)
    {
        if (!_fileExist)
            throw std::runtime_error("File not found at location specified by member _xyzFilePath");
        out_trackPoints = ParseAll(headerLen);
    }

    /// @brief Read the entire file and returns the parsed floats as xyz coordinates. The returned vector will be have each element as a loaded point stored as cv::Point3f.
    /// @param headerLen Number of characters in the file header. Skip the file header without parsing it as floats.
    /// @return A 1D vector of cv::Point3f
    /// @throw std::runtime_error occurrs when the xyz file is not found.
    std::vector<cv::Point3f> ParseAll(int64_t headerLen = -1)
    {
        if (!_fileExist)
            throw std::runtime_error("File not found at location specified by member _xyzFilePath");

        std::vector<cv::Point3f> out_trackPoints;
        const uint64_t _dataStart = headerLen > -1 ? headerLen : _getHeaderLen();
        _fileHandle.open(_xyzFilePath, std::ios_base::in);
        _fileHandle.seekg(_dataStart); // skip the header if there is any

        try
        {
            // count the number of lines in the file.
            // +1 line for the last line that might not end with \n
            // each line contains 3 floats, representing x, y and z coordinate
            uint64_t lineCount = 1 + std::count(std::istreambuf_iterator<char>(_fileHandle), std::istreambuf_iterator<char>(), '\n');
            out_trackPoints.reserve(3 * lineCount);
            _fileHandle.seekg(_dataStart);

            for (size_t i = 0; i < lineCount; i++)
            {
                std::string lineContent;
                std::getline(_fileHandle, lineContent);
                out_trackPoints.push_back(_parseLine(lineContent));
            }
        }
        catch(const std::exception& e)
        {
            _fileHandle.close();
            std::cerr << e.what() << '\n';
        }
        _fileHandle.close();
        return out_trackPoints;
    }

    virtual ~XyzParser()
    {
        if (_fileHandle.is_open())
            _fileHandle.close();
    };

public: 

private:

    uint64_t _getHeaderLen()
    {
        return 0; // TODO: determine the header length automatically
    }

    /// @brief Parse a line of string containing 3 floats 
    /// @param lineContent The string content of a line
    /// @return cv::Point3f containing xyz components
    cv::Point3f _parseLine(std::string& lineContent)
    {
        char* floatStart = &lineContent[0];
        char* floatEnd{NULL};
        std::array<float, 3> parsedFloats;
        for (size_t parsedFloatCount = 0; parsedFloatCount < 3; parsedFloatCount++)
        {
            // FIXME: symbols other than spaces will result in silent runtime error
            parsedFloats[parsedFloatCount] = strtof(floatStart, &floatEnd);
            floatStart = floatEnd;
        }
        return cv::Point3f(parsedFloats[0], parsedFloats[1], parsedFloats[2]);
    }

private:
    bool _fileExist;
    std::filesystem::path _fileBasename;
    std::ifstream _fileHandle;
    uint64_t _dataStart;
    const std::filesystem::path _xyzFilePath;
};