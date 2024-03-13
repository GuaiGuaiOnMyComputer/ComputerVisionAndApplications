#include "XyzParser.hpp"
#include <filesystem>
#include <format>
#include <string>
#include <array>
#include <vector>
#include <stdint.h>
#include <fstream>

class XyzParser
{
public:
    XyzParser(const std::filesystem::path xyzFilePath) : _xyzFilePath{xyzFilePath}, _fileExist{false}, _dataStart{0}
    {
        _fileBasename = _xyzFilePath.filename();
        _fileExist = std::filesystem::exists(_xyzFilePath);
        _fileHandle = std::ifstream();
    }

    void ParseAll(const int64_t headerLen = -1, std::vector<float>& out_trackPoints)
    {
        if (!_fileExist)
            throw std::runtime_error(std::format("File not found at {}", _xyzFilePath));

        const uint64_t _dataStart = headerLen > -1 ? headerLen : _getHeaderLen();
        _fileHandle.open(_xyzFilePath, std::ios_base::in);
        _fileHandle.seekg(_dataStart); // skip the header if there is any

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
        _fileHandle.close();
        return;
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

    std::array<float, 3> _parseLine(const std::string& lineContent)
    {
        std::array<float, 3> xyz;
        size_t floatStart{0};
        char** floatEnd{NULL};
        for (size_t parsedFloatCount = 0; parsedFloatCount < 3; parsedFloatCount++)
        {
            // FIXME: symbols other than spaces will result in silent runtime error
            xyz[parsedFloatCount] = strtof(&lineContent[floatStart], floatEnd);
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