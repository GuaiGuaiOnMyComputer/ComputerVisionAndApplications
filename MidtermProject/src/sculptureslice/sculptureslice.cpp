#include <opencv2/opencv.hpp>
#include <filesystem>
#include <string.h>
#include "masking.hpp"

namespace midproj
{
    namespace fs = std::filesystem;

    class SculptureSlice
    {
    public:

        SculptureSlice(const cv::Mat& scanImage, const int32_t imgIndex): _scanImage{scanImage}, _redPixelMap{cv::Mat()}, _imageIndex{imgIndex}
        {
        }

        void get_red_scan_line(cv::InputArray foregroundMask)
        {
            _redPixelMap = midproj::find_red_pixels(_scanImage, foregroundMask, _redPixelCoordinates);
        }

        // dummy constructor
        SculptureSlice(): _imageIndex{-1}, _scanImage{cv::Mat()}
        {
        }

        virtual ~SculptureSlice()
        {
        }

        // SculptureSlice& operator=(const SculptureSlice& another)
        // {
            // if (this == &another)
            //     return *this;
            // this->_redPixelCoordinates = another._redPixelCoordinates;
            // this->_imageIndex = another._imageIndex;
            // this->_scanImage = another._scanImage;
            // return *this;
        // }
    private:
        std::vector<cv::Point2i> _redPixelCoordinates;
        cv::Mat _redPixelMap;
        const int32_t _imageIndex;
        const cv::Mat _scanImage;
    };
} // namespace midproj
