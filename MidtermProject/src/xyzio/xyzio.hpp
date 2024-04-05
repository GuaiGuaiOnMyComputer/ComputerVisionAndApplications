#include <opencv2/opencv.hpp>
#include <filesystem>

namespace midproj
{
    namespace fs = std::filesystem;
    class XyzIo
    {
    public:
        static std::vector<cv::Point2i> load_points_from_file_2i(const fs::path &filePath);
        static std::vector<cv::Point2f> load_points_from_file_2f(const fs::path &filePath);
    private:
        static cv::Point2i _parseLineAsPoint_2i(const std::string &lineContent);
        static cv::Point2f _parseLineAsPoint_2f(const std::string &lineContent);
        static uint64_t _getLineCount(std::ifstream &fileHandle);
    };
}