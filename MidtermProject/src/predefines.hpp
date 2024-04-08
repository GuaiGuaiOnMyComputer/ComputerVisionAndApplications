#include <filesystem>
#include <opencv2/opencv.hpp>

#if true
template class std::vector<cv::Point2i>;
template class std::vector<cv::Point2f>;
template class std::vector<cv::Mat>;
template class std::vector<midproj::SliceTransform::BeamIndex>;
#endif

const std::filesystem::path FRAME_CORNER_2D_FILE_PATH("assets//FrameCornerCoordinates2d.xyz");
const std::filesystem::path FRAME_CORNER_3D_FILE_PATH("assets//FrameCornerCoordinates3d.xyz");
const std::filesystem::path SCULPTURE_ROI_FILE_PATH("assets//SculptureRoi.xyz");
const std::filesystem::path PATH_TO_ALL_SCAN_IMAGES("assets//ShadowStrip");
const cv::Size2i IMG_SIZE(1080, 1080);
const cv::Rect2i SCULPTURE_ROI(379, 263, 220, 417);
constexpr int32_t SCAN_IMAGE_COUNT = 55;