#include <opencv2/opencv.hpp>
#include <filesystem>
#include <string>
#include <stdint.h>
#include "scanimageio.hpp"

namespace finprj
{
    namespace fs = std::filesystem;

    ImagePair::ImagePair() noexcept : FilePath{fs::path("NoImage")}, Image{cv::Mat()}, Right{cv::Mat()}, Left{cv::Mat()}
    {
    }

    ImagePair::ImagePair(const cv::Mat &image, const fs::path &filepath) : FilePath{filepath}, Image{image}
    {
        Left = cv::Mat(Image, cv::Range::all(), cv::Range(0, Image.cols / 2));
        Right = cv::Mat(Image, cv::Range::all(), cv::Range(Image.cols / 2 + 1, Image.cols));
    }

    ScanImageIo::ScanImageIo(const std::filesystem::path &imagePathRoot, const std::string &extension, std::error_code& filesystemErrorCode) noexcept 
        :_imageCount{0}, _currentImagePair{0}, _imageExtension{extension}, _imagePathRoot{fs::absolute(imagePathRoot)}, _pathIsValid{fs::is_directory(_imagePathRoot)}
    {
        const fs::directory_iterator imagePathRootIterator(_imagePathRoot, filesystemErrorCode);
        for(const fs::directory_entry& itemInDir : imagePathRootIterator)
        {
            if (extension == itemInDir.path().extension())
                _imageCount++;
        }
    }

    ImagePair ScanImageIo::GetNextPair()
    {
        if (_currentImagePair > _imageCount)
            return ImagePair();

        uint8_t prependingZeroCount{0};
        if (_currentImagePair > 99)
            prependingZeroCount = 0;
        else if(_currentImagePair <= 99 && _currentImagePair > 9)
            prependingZeroCount = 1;
        else
            prependingZeroCount = 2;
        std::string fileName = std::string(prependingZeroCount, '0') + std::to_string(_currentImagePair) + _imageExtension;
        return ImagePair(cv::imread((_imagePathRoot / fileName).string()), fileName);
    }
}