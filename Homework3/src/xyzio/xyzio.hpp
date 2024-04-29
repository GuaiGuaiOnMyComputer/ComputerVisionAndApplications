#pragma once

#include <opencv2/opencv.hpp>
#include <filesystem>
#include <fstream>
#include <stdint.h>

namespace hw3
{
    namespace fs = std::filesystem;

    class XyzIo
    {
    private:
        class _DataPoint
        {
        protected:
            _DataPoint() noexcept;
            virtual ~_DataPoint() noexcept;
        };


    public:

        template<class T>
        class CoorAndNormal3D : public _DataPoint, public cv::Point3_<T>, public cv::Vec<T, 3>
        {
        public:
            static constexpr int32_t Dimensions = 3;
            static constexpr int32_t Components = 2;

            CoorAndNormal3D(std::array<T, Dimensions> xyzCoordinates, std::array<T, Dimensions> xyzNormals) noexcept
                : cv::Point3_<T>(xyzCoordinates[0], xyzCoordinates[1], xyzCoordinates[2]), 
                cv::Vec<T, Dimensions>(xyzNormals[0], xyzNormals[1], xyzNormals[2]) 
            {
            }

            CoorAndNormal3D(const std::array<T, Dimensions * Components> &data) noexcept
                : cv::Point3_<T>(data[0], data[1], data[2]),
                cv::Vec<T, Dimensions>(data[3], data[4], data[5]) 
            {
            }

            CoorAndNormal3D() noexcept = default;
            ~CoorAndNormal3D() noexcept override {}
        };

        template<class T>
        class Coor2D : public _DataPoint, public cv::Point_<T>
        {
        public:
            static constexpr int32_t Dimensions = 2;
            static constexpr int32_t Components = 1;
            static constexpr cv::DataType<T> type = CV_MAKE_TYPE(sizeof(T), Components);

            Coor2D(const std::array<T, Dimensions * Components> &data) noexcept : cv::Point_<T>(data[0], data[1]) {}
            Coor2D() noexcept = default;
            ~Coor2D() noexcept override {}
        };

        template<class T>
        class Coor3D : public _DataPoint, public cv::Point3_<T>
        {
        public:
            static constexpr int32_t Dimensions = 3;
            static constexpr int32_t Components = 1;
            static constexpr cv::DataType<T> type = CV_MAKE_TYPE(sizeof(T), Components);

            Coor3D(const std::array<T, Dimensions * Components> &data) noexcept : cv::Point3_<T>(data[0], data[1], data[2]) {}
            Coor3D() noexcept = default;
            ~Coor3D() noexcept override {}
        };

        template<class T>
        class Rgb : public _DataPoint, public cv::Vec<T, 3>
        {
        public: 
            static constexpr int32_t Dimensions = 3;
            static constexpr int32_t Components = 1;
            static constexpr cv::DataType<T> type = CV_MAKE_TYPE(sizeof(T), Components);

            Rgb(const std::array<T, Dimensions> &data) noexcept : cv::Vec<T, 3>(data[0], data[1], data[2]) {};
            Rgb(const T r, const T g, const T b): cv::Vec<T, 3>(r, g, b) {};
            Rgb(const cv::Vec<T, 3> &rgbAsVec) : cv::Vec<T, 3>(rgbAsVec[0], rgbAsVec[1], rgbAsVec[2]){};
            Rgb() noexcept = default;
            ~Rgb() noexcept override {}
        };

        template<class TrowType, class TelementType>
        static std::vector<TrowType> load_points_from_file(const fs::path &filePath, const uint32_t emptyLineCount)
        {
            std::ifstream fileHandle(filePath, std::ifstream::in);
            uint64_t lineCount = _getLineCount(fileHandle, emptyLineCount);
            std::vector<TrowType> parsedLines(lineCount);

            std::string lineContent;
            for (size_t line = 0; line < lineCount; line++)
            {
                std::getline(fileHandle, lineContent);
                parsedLines.at(line) = _parseLine<TrowType, TelementType, TrowType::Dimensions, TrowType::Components>(lineContent);
            }
            return parsedLines;
        }

        using Coor3D_f = XyzIo::Coor3D<float>;
        using Coor3D_d = XyzIo::Coor3D<double>;
        using Coor2D_f = XyzIo::Coor2D<float>;
        using Coor2D_d = XyzIo::Coor2D<double>;
        using Rgb_ui8 = XyzIo::Rgb<uint8_t>;
        using CoorAndNormal3D_f = XyzIo::CoorAndNormal3D<float>;
        using CoorAndNormal3D_d = XyzIo::CoorAndNormal3D<double>;

        static bool write_xyz_point_cloud_file(const fs::path &filePath, const std::vector<std::vector<cv::Point3d>> &pointsInAllSlices);

    private:
        static uint64_t _getLineCount(std::ifstream &fileHandle, const uint32_t emptyLineCount);
        static std::error_code _create_output_directory_if_not_exist(const fs::path &filePath);

        template<class TrowType, class TelementType, size_t Tdimensions, size_t Tcomponents>
        static TrowType _parseLine(const std::string &lineContent)
        {
            std::array<TelementType, Tdimensions * Tcomponents> parsedLine; // an array of fixed size storing all the parsed numbers within a line of the file
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
            return TrowType(parsedLine);
        }
    };


}

template<> class cv::DataType<hw3::XyzIo::Coor2D<float>>
{
public:
    typedef float       value_type;
    typedef value_type  work_type;
    typedef value_type  channel_type;
    typedef value_type  vec_type;
    enum { generic_type = 0,
           depth        = CV_32F,
           channels     = 2,
           fmt          = (int)'f',
           type         = CV_MAKETYPE(depth, channels)
         };
};

template<> class cv::DataType<hw3::XyzIo::Coor2D<double>>
{
public:
    typedef double      value_type;
    typedef value_type  work_type;
    typedef value_type  channel_type;
    typedef value_type  vec_type;
    enum { generic_type = 0,
           depth        = CV_64F,
           channels     = 2,
           fmt          = (int)'f',
           type         = CV_MAKETYPE(depth, channels)
         };
};

template<> class cv::DataType<hw3::XyzIo::Coor3D<float>>
{
public:
    typedef float       value_type;
    typedef value_type  work_type;
    typedef value_type  channel_type;
    typedef value_type  vec_type;
    enum { generic_type = 0,
           depth        = CV_32F,
           channels     = 3,
           fmt          = (int)'f',
           type         = CV_MAKETYPE(depth, channels)
         };
};

template<> class cv::DataType<hw3::XyzIo::Coor3D<double>>
{
public:
    typedef double      value_type;
    typedef value_type  work_type;
    typedef value_type  channel_type;
    typedef value_type  vec_type;
    enum { generic_type = 0,
           depth        = CV_64F,
           channels     = 3,
           fmt          = (int)'f',
           type         = CV_MAKETYPE(depth, channels)
         };
};