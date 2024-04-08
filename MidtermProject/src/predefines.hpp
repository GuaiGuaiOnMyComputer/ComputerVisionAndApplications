#include <filesystem>
#include <opencv2/opencv.hpp>

#if true
template class std::vector<cv::Point2i>;
template class std::vector<cv::Point2f>;
template class std::vector<cv::Point3i>;
template class std::vector<cv::Point3f>;
template class std::vector<cv::Mat>;
template class std::vector<midproj::SliceTransform::BeamIndex>;
template class std::array<cv::Point2f, 4>;
template class std::array<cv::Point2f, 8>;
template class std::array<cv::Point2i, 4>;
template class std::array<cv::Point2i, 8>;
template class std::array<cv::Point3f, 4>;
template class std::array<cv::Point3f, 8>;
template class std::array<cv::Point3i, 4>;
template class std::array<cv::Point3i, 8>;
#endif

const std::filesystem::path FRAME_CORNER_2D_FILE_PATH("assets//FrameCornerCoordinates2d.xyz");
const std::filesystem::path FRAME_CORNER_3D_FILE_PATH("assets//FrameCornerCoordinates3d.xyz");
const std::filesystem::path SCULPTURE_ROI_FILE_PATH("assets//SculptureRoi.xyz");
const std::filesystem::path PATH_TO_ALL_SCAN_IMAGES("assets//ShadowStrip");
const std::filesystem::path OUTPUT_XYZ_FILE("output//吳宇昕_m11225013.xyz");
const cv::Size2i IMG_SIZE(1080, 1080);
const cv::Rect2i SCULPTURE_ROI(379, 263, 220, 417);
constexpr int32_t SCAN_IMAGE_COUNT = 55;