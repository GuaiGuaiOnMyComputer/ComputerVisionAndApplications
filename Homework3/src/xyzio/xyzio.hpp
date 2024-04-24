#include <opencv2/opencv.hpp>
#include <filesystem>
#include <fstream>
#include <string_view>

namespace hw3
{
    namespace fs = std::filesystem;

    class XyzIo
    {
    public:
        struct PointCoorAndNormal
        {
            cv::Point3d Coor;
            cv::Vec3d Normal;
            constexpr static int32_t Dimensions = 3;
            constexpr static int32_t Components = 2;

            PointCoorAndNormal(std::array<double, Dimensions> xyzCoordinates, std::array<double, Dimensions> xyzNormals) noexcept;
            PointCoorAndNormal(const std::array<double, Dimensions * Components> &data) noexcept;
            PointCoorAndNormal();
            ~PointCoorAndNormal();
        };

        template<class T>
        static std::vector<T> load_points_from_file(const fs::path &filePath, const uint32_t emptyLineCount)
        {
            std::ifstream fileHandle(filePath, std::ifstream::in);
            uint64_t lineCount = _getLineCount(fileHandle, emptyLineCount);
            std::vector<T> parsedLines(lineCount);

            std::string lineContent;
            for (size_t line = 0; line < lineCount; line++)
            {
                std::getline(fileHandle, lineContent);
                parsedLines.at(line) = _parseLine<T,  T::Dimensions,  T::Components>(lineContent);
            }
            return parsedLines;
        }

        static bool write_xyz_point_cloud_file(const fs::path &filePath, const std::vector<std::vector<cv::Point3d>> &pointsInAllSlices);

    private:
        static uint64_t _getLineCount(std::ifstream &fileHandle, const uint32_t emptyLineCount);
        static std::error_code _create_output_directory_if_not_exist(const fs::path &filePath);

        template<class T, size_t Tdimensions, size_t Tcomponents>
        static T _parseLine(const std::string &lineContent)
        {
            std::array<double, Tdimensions * Tcomponents> parsedLine; // an array of fixed size storing all the parsed numbers within a line of the file
            size_t parsedStringLength{0};
            for (size_t c = 0; c < Tcomponents; c++)
            {
                for (size_t d = 0; d < Tdimensions; d++)
                {
                    size_t valueStringLen;
                    parsedLine.at(d + c * Tdimensions) = std::stof(lineContent.data() + parsedStringLength, &valueStringLen);
                    parsedStringLength += valueStringLen;
                }
            }
            return T(parsedLine);
        }
    };
}