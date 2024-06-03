#pragma once

namespace finprj
{
    class AssetConfig
    {
    public:
        static inline constexpr char SideBySideImagePathRoot[] = "../assets/SBS images";
        static inline constexpr char CameraParameterFilePath[] = "../CameraParameter.txt";
        static inline constexpr char SideBySideImageFileExtension[] = ".jpg";
        static inline constexpr int32_t SideBySideImageCount = 179;
        static inline constexpr double LeftCameraMatrix[] = {
            1000.0, 0.0, 360.0,
            0.0, 1000.0, 640.0,
            0.0, 0.0, 1.0};
        static inline constexpr double RightCameraMatrix[] = {
            1000.0, 0.0, 360.0,
            0.0, 1000.0, 640.0,
            0.0, 0.0, 1.0};
        static inline constexpr double LeftCameraRt[] = {
            0.88649035, -0.46274707, -0.00, -14.42428,
            -0.070794605, -0.13562201, -0.98822814, 86.532959,
            0.45729965, 0.8760547, -0.1529876, 235.35446};
        static inline constexpr double RightCameraRt[] = {
            0.98480779, -0.17364818, -4.9342116E-8, -0.98420829,
            -0.026566068, -0.15066338, -0.98822814, 85.070221,
            0.17160401, 0.97321475, -0.1529876, 236.97873};
        static inline constexpr double FundementalMatrix[] = {
            3.283965767647195E-7, -6.76098398189792E-6, 0.0021123144539793737,
            -8.046341661808292E-6, 3.05632173594769E-8, 0.05124913199417346,
            0.0048160232373805345, -0.051062699158041805, 1.0706286680443888
        };
        
    };
};