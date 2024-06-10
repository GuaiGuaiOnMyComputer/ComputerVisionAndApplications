#pragma once
#include <cstdint>
#include <array>

namespace finprj
{
    /// @brief This class contains a lot of static arrays representing some predefined camera parameters and relative paths to important assets.
    class AssetConfig
    {
    public:
        static inline constexpr char SideBySideImagePathRoot[] = "../assets/SBS images";
        static inline constexpr char CameraParameterFilePath[] = "../CameraParameter.txt";
        static inline constexpr char PredictedXyzOutputPath[]  = "../output/PredictedXyz.txt";
        static inline constexpr char SideBySideImageFileExtension[] = ".jpg";
        static inline constexpr int32_t SideBySideImageCount = 179;
        static inline constexpr int32_t SideBySideImageWidth = 1440;
        static inline constexpr int32_t SideBySideImageHeight = 1280;
        static inline constexpr std::array<double, 3 * 3> LeftCameraK{
            1000.0, 0.0, 360.0,
            0.0, 1000.0, 640.0,
            0.0, 0.0, 1.0};
        static inline constexpr std::array<double, 3 * 3> RightCameraK{
            1000.0, 0.0, 360.0,
            0.0, 1000.0, 640.0,
            0.0, 0.0, 1.0};
        static inline constexpr std::array<double, 3 * 4> LeftCameraRt{
            0.88649035, -0.46274707, -0.00, -14.42428,
            -0.070794605, -0.13562201, -0.98822814, 86.532959,
            0.45729965, 0.8760547, -0.1529876, 235.35446};
        static inline constexpr std::array<double, 3 * 4> RightCameraRt{
            0.98480779, -0.17364818, -4.9342116E-8, -0.98420829,
            -0.026566068, -0.15066338, -0.98822814, 85.070221,
            0.17160401, 0.97321475, -0.1529876, 236.97873};
        static inline constexpr std::array<double, 3 * 3> FundementalMatrix{
            3.283965767647195E-7, -6.76098398189792E-6, 0.0021123144539793737,
            -8.046341661808292E-6, 3.05632173594769E-8, 0.05124913199417346,
            0.0048160232373805345, -0.051062699158041805, 1.0706286680443888
        };
        static inline constexpr std::array<int64_t, 4> ScanObjectLeftRoi{
            154, 52, 360, 830
        };

        static inline constexpr std::array<double, 3 * 4> TestParameter_LeftCameraP{
            2.0179, 1.5967, -0.5695, 113.8802,
            0.2820, -0.7636, -2.4258, 305.7125,
            -0.0009, 0.0023, -0.0018, 1.0000
        };

        static inline constexpr std::array<double, 3 * 4> TestParameter_RightCameraP{
            2.8143, -1.3450, -0.5673, 347.4957,
            -0.4439, -0.4444, -3.0134, 371.1864,
            0.0023, 0.0023, -0.0018, 1.000
        };

        static inline constexpr std::array<double, 3 * 3> TestParameter_FundementalMatrix{
            0,  0,  -0.0007,
            0,  0,  0.0105,
            -0.0011, -0.0093, 1
        };

        static inline constexpr std::array<double, 2> TestParameter_PointOnLeft1{227, 212};
        static inline constexpr std::array<double, 2> TestParameter_PointOnLeft2{275, 322};
        static inline constexpr std::array<double, 2> TestParameter_PointOnRight1{201, 144};
        static inline constexpr std::array<double, 2> TestParameter_PointOnRight2{275, 261};
    };
};