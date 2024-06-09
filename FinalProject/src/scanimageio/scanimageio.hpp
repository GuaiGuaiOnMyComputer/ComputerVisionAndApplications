#include <stdint.h>
#include <filesystem>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

namespace finprj
{
    class ImagePair
    {
    public:
        ImagePair(const cv::Mat &image, const std::filesystem::path &filePath) noexcept; 
        ImagePair() noexcept;

    public:
        const std::filesystem::path FilePath;
        cv::Mat Image;
        const cv::Rect LeftRoi;
        const cv::Rect RightRoi;
        cv::Mat Left;
        cv::Mat Right;
    };

    class ScanImageIo
    {
    public:
        explicit ScanImageIo(const std::filesystem::path &imagePathRoot, const std::string &extension, std::error_code& filesystemErrorCode) noexcept;

        ImagePair GetNextPair();
        ImagePair GetPairByIndex(const int32_t imagePairIndex);
        size_t GetImageCount();
        static void init_roi_mask(const cv::Rect &scanObjectRoiLeft, const int64_t scanImageWidth, const int64_t scanImageHeight);
        static void get_blue_pixel_mask_by_color(const cv::Mat &image, cv::Mat &outputMask, const cv::Mat& predefinedRoiMask, const bool usePredefinedRoiMask, const float bluePixelValueMultiple = 1.55);
        static void get_blue_pixel_mask_by_time_difference(const cv::Mat &previousImage, const cv::Mat &nextImage, const int32_t differenceThreshold, cv::Mat &out_bluePixelMask);
        static void get_blue_pixel_mask(const ImagePair &previousImagePair, const ImagePair &nextImagePair, cv::Mat1b &out_mask, const cv::Mat &predefinedRoiMask, const bool usePredefinedRoiMask, const uint8_t differenceThreshold, const float bluePixelValueMultiple = 1.55);
        static void get_blue_pixel_coors(const cv::Mat &bluePixelMask, std::vector<cv::Point>& out_bluePixelCoors);
        static void get_blue_pixel_coors(const cv::Mat &bluePixelMask, cv::Mat_<cv::Point>& out_bluePixelCoors);

    private:
        static std::string _get_image_pair_file_name(int32_t pairIndex, const std::string &imageExtension);

    public:
        static cv::Rect s_ModelRoi;
        static cv::Mat s_ModelRoiMask;

    private:
        size_t _imageCount;
        size_t _currentImagePair;
        const std::string _imageExtension;
        const std::filesystem::path _imagePathRoot;
        const bool _pathIsValid;
    };
}