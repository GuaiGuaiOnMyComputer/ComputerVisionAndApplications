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

    XyzIo::_DataPoint::_DataPoint() noexcept = default;
    XyzIo::_DataPoint::~_DataPoint() noexcept = default;

    bool XyzIo::write_xyz_normal_and_rgb(const fs::path& filePath, const std::vector<XyzIo::CoorAndNormal3D<float>> &coorsAndNors, const std::vector<XyzIo::Rgb<uint8_t>> &rgbs)
    {
        assert(coorsAndNors.size() == rgbs.size());

        bool outputPathCreateSuccess = XyzIo::_create_output_directory_if_not_exist(filePath.parent_path());
        if (!outputPathCreateSuccess)
        {
            std::cout << "Failed to create the output directory " << fs::absolute(filePath) << std::endl;
            exit(EXIT_FAILURE);
        }

        std::ofstream outputFile(filePath, std::ios_base::out);
        outputFile.seekp(0, std::ios_base::beg);

        for (size_t i = 0; i < coorsAndNors.size(); i++)
        {
            outputFile << std::setprecision(4) << coorsAndNors[i].x << ' ' << coorsAndNors[i].y << ' ' << coorsAndNors[i].z << ' ' << rgbs[i].R << ' ' << rgbs[i].G << ' ' << rgbs[i].B << '\n';
        } 
        outputFile.close();
        return true;
    }

    uint64_t XyzIo::_getLineCount(std::ifstream& fileHandle, const uint32_t emptyLineCount)
    {
        fileHandle.seekg(0, std::ios_base::beg);
        uint64_t lineCount = 1 + std::count(std::istreambuf_iterator<char>(fileHandle), std::istreambuf_iterator<char>(), '\n') - emptyLineCount;
        fileHandle.seekg(0, std::ios_base::beg);
        return lineCount;
    }

    bool XyzIo::_create_output_directory_if_not_exist(const fs::path& filePath)
    {
        std::error_code errorCode;
        bool createSuccess = fs::create_directories(filePath, errorCode);
        return createSuccess;
    }

} // namespace hw3
