#include <filesystem>
#include <string>
#include <array>
#include <vector>
#include <iostream>
#include <stdint.h>
#include <fstream>
// #include "XyzParser.hpp"
// TODO: include header file instead

class XyzParser
{
public:
    XyzParser(const std::filesystem::path xyzFilePath) : _xyzFilePath{xyzFilePath}, _fileExist{false}, _dataStart{0}
    {
        _fileBasename = _xyzFilePath.filename();
        _fileExist = std::filesystem::exists(_xyzFilePath);
        _fileHandle = std::ifstream();
    }

    /// @brief Read the entire file and returns the parsed floats as xyz coordinates. The resultant vector will be in 1D with length=3*rows elements. Every 3 floats in this vector represents a point's xyz coordinate.
    /// @param out_trackPoints An output variable storing all the parsed floats.
    /// @param headerLen Number of characters in the file header. Skip the file header without parsing it as floats.
    void ParseAll(std::vector<float>& out_trackPoints, int64_t headerLen = -1)
    {
        if (!_fileExist)
            throw std::runtime_error("File not found at location specified by member _xyzFilePath");

        out_trackPoints = ParseAll(headerLen);
    }

    /// @brief Read the entire file and returns the parsed floats as xyz coordinates. The returned vector will be in 1D with length=3*rows elements. Every 3 floats in this vector represents a point's xyz coordinate.
    /// @param headerLen Number of characters in the file header. Skip the file header without parsing it as floats.
    /// @return A 1D vector of length=nPoints*3
    std::vector<float> ParseAll(int64_t headerLen = -1)
    {
        if (!_fileExist)
            throw std::runtime_error("File not found at location specified by member _xyzFilePath");

        std::vector<float> out_trackPoints;
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
                std::array<float, 3> lineAsFloats = _parseLine(lineContent);

                // copy parsed floats into out_trackPoints
                for (size_t parsedFloatIndex = 0; parsedFloatIndex < 3; parsedFloatIndex++)
                    out_trackPoints.push_back(lineAsFloats[parsedFloatIndex]);
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

    std::array<float, 3> _parseLine(std::string& lineContent)
    {
        std::array<float, 3> xyz;
        char* floatStart = &lineContent[0];
        char* floatEnd{NULL};
        for (size_t parsedFloatCount = 0; parsedFloatCount < 3; parsedFloatCount++)
        {
            // FIXME: symbols other than spaces will result in silent runtime error
            xyz[parsedFloatCount] = strtof(floatStart, &floatEnd);
            floatStart = floatEnd;
        }
        return xyz;
    }

private:
    bool _fileExist;
    std::filesystem::path _fileBasename;
    std::ifstream _fileHandle;
    uint64_t _dataStart;
    const std::filesystem::path _xyzFilePath;
};