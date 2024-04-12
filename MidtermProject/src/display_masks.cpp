#include "masking.hpp"
#include "xyzio.hpp"
#include "slicetransform.hpp"
#include "predefines.hpp"
#include "scanimgio.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <stdint.h>
#include <filesystem>

cv::Mat annotateCornersAndBeams(const cv::Mat &originalImage);
void get_front_and_left_panel_homography_projection(const std::vector<cv::Point3d>& pixelsInWorldCoor, cv::Mat& leftPanelProjection, cv::Mat& frontPanelProjection);

int main(int, char**)
{
    constexpr int32_t ARIBRITARY_IMG_INDEX = 33;

    // Check if all of the require images are present
    // Load them all into cv::Mat or exit the program if they are not
    midproj::ScanImageIo::asset_check(PATH_TO_ALL_SCAN_IMAGES, {FRAME_CORNER_2D_FILE_PATH, FRAME_CORNER_3D_FILE_PATH, SCULPTURE_ROI_FILE_PATH});
    std::vector<cv::Mat> scanImages = midproj::ScanImageIo::load_all_images(PATH_TO_ALL_SCAN_IMAGES, SCAN_IMAGE_COUNT);

    // A Binary image where the sculpture and the frames are true, and false anywhere else.
    const cv::Mat foregroundMask = midproj::get_foreground_mask(scanImages, IMG_SIZE);

    // Vector of binarized scan images in which the red pixels are true and anywhere else false.
    std::vector<cv::Mat> redPixelMaps(SCAN_IMAGE_COUNT);
    cv::Mat aribritaryOriginalImage, aribritaryRedPixelMap;
    for (size_t imgIndex = 0; imgIndex < SCAN_IMAGE_COUNT; imgIndex++)
    {
        redPixelMaps.at(imgIndex) = midproj::get_red_pixel_map(scanImages.at(imgIndex), foregroundMask);
        if (imgIndex == ARIBRITARY_IMG_INDEX)
        {
            aribritaryOriginalImage = scanImages[imgIndex].clone();
            aribritaryRedPixelMap = redPixelMaps.at(imgIndex).clone();
        }
        scanImages.at(imgIndex).release(); // dispose the scanned image Mat object to save memory.
    }

    // A binary image in which everywhere the red scan line can reach is true, and anywhere else false.
    const cv::Mat scannedAreaMask = midproj::get_scanned_area_mask(redPixelMaps, foregroundMask, IMG_SIZE);

    const cv::Mat sculptureAreaMask = midproj::get_sculpture_area_mask(IMG_SIZE, SCULPTURE_ROI);

    // A binary image in which everywhere on the sculpture that the red scan line can reach is true, and anywhere else false.
    const cv::Mat scannedSculptureMask = midproj::get_scanned_sculpture_mask(scannedAreaMask, sculptureAreaMask);

    // A binary image in which everywhere on the frame that the red scan line can reach is true, and anywhere else false.
    const cv::Mat scannedFrameMask = midproj::get_scanned_frame_mask(scannedAreaMask, sculptureAreaMask);

    // Coordinates of the frame corners in the image coordinate system. Values measured using photo editing software.
    const std::vector<cv::Point2i> frameCornerCoorsImage2i = midproj::XyzIo::load_points_from_file_2i(FRAME_CORNER_2D_FILE_PATH);

    // Coordinates of the frame corners in the world coordinate system. Values given from homework instructions.
    const std::vector<cv::Point3i> frameCornerCoorsWorld3i = midproj::XyzIo::load_points_from_file_3i(FRAME_CORNER_3D_FILE_PATH);

    midproj::SliceTransform::load_frame_corners_from_vector(frameCornerCoorsImage2i, frameCornerCoorsWorld3i);
    midproj::SliceTransform::fit_frame_beam_lines();
    const std::array<cv::Point2f, midproj::SliceTransform::BEAM_COUNT> avgPixelCoorsOnBeams = midproj::SliceTransform::get_red_pixels_on_frame(aribritaryRedPixelMap);
    midproj::SliceTransform::init_front_panel_homography();
    midproj::SliceTransform::init_left_panel_homography(avgPixelCoorsOnBeams);
    aribritaryOriginalImage = annotateCornersAndBeams(aribritaryOriginalImage);

    cv::Mat aribritaryFrontPanelHomography(cv::Size2i(200, 200), CV_8UC1, cv::Scalar(0));
    cv::Mat aribritaryLeftPanelHomography(cv::Size2i(200, 200), CV_8UC1, cv::Scalar(0));
    cv::Mat aribritarySculptureRedPixelMap;
    cv::bitwise_and(aribritaryRedPixelMap, scannedSculptureMask, aribritarySculptureRedPixelMap);
    std::vector<cv::Point3d> aribritaryPointCoorsInWorld = midproj::SliceTransform::get_slice_world_coordinate(avgPixelCoorsOnBeams, aribritarySculptureRedPixelMap);
    get_front_and_left_panel_homography_projection(aribritaryPointCoorsInWorld, aribritaryLeftPanelHomography, aribritaryFrontPanelHomography);

    cv::namedWindow("ForegroundMask");
    cv::namedWindow("SculptureAreaMask");
    cv::namedWindow("ScannedAreaMask");
    cv::namedWindow("ScannedSculptureMask");
    cv::namedWindow("ScannedFrameMask");
    cv::namedWindow("CornersAndBeamLines");
    cv::namedWindow("LeftPanelHomographyProjection");
    cv::namedWindow("FrontPanelHomographyProjection");
    cv::namedWindow("ControlPanel");

    int32_t w{500}, h{500};
    cv::createTrackbar("w", "ControlPanel", &w, IMG_SIZE.width);
    cv::createTrackbar("h", "ControlPanel", &h, IMG_SIZE.height);
    const cv::Point2i imgCenter(IMG_SIZE.width / 2, IMG_SIZE.height / 2);

    constexpr int32_t exitButtonKey = 69;           // press lower Upper case E to exit
    constexpr int32_t takeSnapshotButtonKey = 32;   // press space bar to take snapshot
    const std::filesystem::path maskImagesOutputPath("reportassets\\maskimages");
    while (true)
    {
        cv::Size2i roiSize(w, h);
        const cv::Point2i roiTopLeft(imgCenter.x - roiSize.width / 2, imgCenter.y - roiSize.height / 2);
        const cv::Rect2i roi(roiTopLeft, roiSize);
        cv::imshow("ForegroundMask", foregroundMask(roi));
        cv::imshow("SculptureAreaMask", sculptureAreaMask(roi));
        cv::imshow("ScannedAreaMask", scannedAreaMask(roi));
        cv::imshow("ScannedSculptureMask", scannedSculptureMask(roi));
        cv::imshow("ScannedFrameMask", scannedFrameMask(roi));
        cv::imshow("CornersAndBeamLines", (aribritaryOriginalImage(roi)));
        cv::imshow("LeftPanelHomographyProjection", aribritaryLeftPanelHomography);
        cv::imshow("FrontPanelHomographyProjection", aribritaryFrontPanelHomography);

        const uint32_t keyCode = cv::waitKey(1);
        if (keyCode == takeSnapshotButtonKey)
        {
            std::filesystem::create_directories(maskImagesOutputPath);

            const std::vector<int32_t> imageWriteParams
            {
                cv::IMWRITE_JPEG_QUALITY,
                70,
            };

            cv::imwrite((maskImagesOutputPath / std::filesystem::path("ForegroundMask.jpg")).string(), foregroundMask(roi));
            cv::imwrite((maskImagesOutputPath / std::filesystem::path("SculptureAreaMask.jpg")).string(), sculptureAreaMask(roi));
            cv::imwrite((maskImagesOutputPath / std::filesystem::path("ScannedAreaMask.jpg")).string(), scannedAreaMask(roi));
            cv::imwrite((maskImagesOutputPath / std::filesystem::path("ScannedSculptureMask.jpg")).string(), scannedSculptureMask(roi));
            cv::imwrite((maskImagesOutputPath / std::filesystem::path("ScannedFrameMask.jpg")).string(), scannedFrameMask(roi));
            cv::imwrite((maskImagesOutputPath / std::filesystem::path("CornersAndBeamLines.jpg")).string(), aribritaryOriginalImage(roi));
            cv::imwrite((maskImagesOutputPath / std::filesystem::path("RedPixelMap.jpg")).string(), aribritaryRedPixelMap(roi));
            cv::imwrite((maskImagesOutputPath / std::filesystem::path("FrontPanelHomographyProjection.jpg")).string(), aribritaryFrontPanelHomography);
            cv::imwrite((maskImagesOutputPath / std::filesystem::path("LeftPanelHomographyProjection.jpg")).string(), aribritaryLeftPanelHomography);
        }
        if (keyCode == exitButtonKey)
            break;
    }
    return 0;
}

void get_front_and_left_panel_homography_projection(const std::vector<cv::Point3d>& pixelsInWorldCoor, cv::Mat& leftPanelProjection, cv::Mat& frontPanelProjection)
{
    for (const cv::Point3d& ptWorld : pixelsInWorldCoor)
    {
        cv::Vec2i pointOnFrontPanel(200 - (ptWorld.z + 100), 200 - (ptWorld.x + 100));
        cv::Vec2i pointOnLeftPanel(200 - (ptWorld.z + 100), 200 - (ptWorld.y + 100));

        leftPanelProjection.at<cv::Vec2i>(pointOnLeftPanel) = 255;
        frontPanelProjection.at<cv::Vec2i>(pointOnFrontPanel) = 255;
    }
}

cv::Mat annotateCornersAndBeams(const cv::Mat& originalImage)
{
    using st = midproj::SliceTransform;

    cv::Mat annotatedImage = originalImage.clone();
    const std::array<cv::Point2f, st::CORNER_COUNT> corners = st::get_corner_image_coordinates();
    for (const cv::Point2f& cornerImageCoor : corners)
    {
        cv::circle(annotatedImage, cornerImageCoor, 10, cv::Scalar(22, 188, 18), -1);
    }
    cv::line(annotatedImage, corners[st::NEAR_TL], corners[st::NEAR_TR], cv::Scalar(255, 255, 0), 5);
    cv::line(annotatedImage, corners[st::NEAR_BL], corners[st::NEAR_BR], cv::Scalar(255, 255, 0), 5);
    cv::line(annotatedImage, corners[st::FAR_TL], corners[st::FAR_TR], cv::Scalar(255, 255, 0), 5);
    cv::line(annotatedImage, corners[st::FAR_BL], corners[st::FAR_BR], cv::Scalar(255, 255, 0), 5);
    return annotatedImage;
}