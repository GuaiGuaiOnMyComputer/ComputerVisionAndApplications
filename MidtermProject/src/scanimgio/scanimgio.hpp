#include <vector>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <stdint.h>

namespace midproj
{
    class ScanImageIo
    {
    public:
        static bool asset_check(const std::filesystem::path &pathToImageAssetFolder, const std::vector<std::filesystem::path> &pathToEachXyzAssets);
        static std::vector<cv::Mat> load_all_images(const std::filesystem::path &allImageFolderPath, uint32_t imageCount);
    private:
        ScanImageIo();
    };
}
