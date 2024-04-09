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
        static std::vector<cv::Point3i> load_points_from_file_3i(const fs::path &filePath);
        static std::vector<cv::Point3f> load_points_from_file_3f(const fs::path &filePath);
        static bool write_xyz_point_cloud_file(const fs::path &filePath, const std::vector<std::vector<cv::Point3d>> &pointsInAllSlices);

    private:
        static cv::Point2i _parseLineAsPoint_2i(const std::string &lineContent);
        static cv::Point2f _parseLineAsPoint_2f(const std::string &lineContent);
        static cv::Point3i _parseLineAsPoint_3i(const std::string &lineContent);
        static cv::Point3f _parseLineAsPoint_3f(const std::string &lineContent);
        static uint64_t _getLineCount(std::ifstream &fileHandle);
        static std::error_code _create_output_directory_if_not_exist(const fs::path &filePath);
    };
}