#include <stdint.h>
#include <filesystem>
#include <string>
#include <opencv2/opencv.hpp>

namespace finprj
{
    class ImagePair
    {
    public:
        ImagePair(const cv::Mat &image, const std::filesystem::path &filePath); 
        ImagePair() noexcept;

    public:
        const std::filesystem::path FilePath;
        cv::Mat Image;
        cv::Mat Right;
        cv::Mat Left;
    };

    class ScanImageIo
    {
    public:
        ScanImageIo(const std::filesystem::path &imagePathRoot, const std::string &extension, std::error_code& filesystemErrorCode) noexcept;

        ImagePair GetNextPair();
        ImagePair GetPairByIndex(const int32_t imagePairIndex);
        static void get_blue_pixel_mask(const cv::Mat image,cv::Mat &outputMask);

    private:
        static std::string _get_image_pair_file_name(int32_t pairIndex, const std::string &imageExtension);

    private:
        int32_t _imageCount;
        int32_t _currentImagePair;
        const std::string _imageExtension;
        const std::filesystem::path _imagePathRoot;
        const bool _pathIsValid;
    };
}