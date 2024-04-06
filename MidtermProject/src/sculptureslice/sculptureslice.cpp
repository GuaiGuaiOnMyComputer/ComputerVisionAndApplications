#include <opencv2/opencv.hpp>
#include <filesystem>
#include "redpixelfinder.hpp"

namespace 
{
    namespace fs = std::filesystem;

    class SculptureSlice
    {
    public:
        SculptureSlice(const fs::path& scanProfileImagePath): _imgPath{scanProfileImagePath}
        {
        }

        void get_red_scan_line()
        {
            cv::Mat scanProfileImage = cv::imread(_imgPath.string());
            midproj::find_red_pixels(scanProfileImage, _redPixelCoordinates);
        }

        // dummy constructor
        SculptureSlice()
        {
        }
    private:
        std::vector<cv::Point2i> _redPixelCoordinates;
        const cv::Mat _redProfile;
        const fs::path _imgPath;
    };
} // namespace SculptureSlice
