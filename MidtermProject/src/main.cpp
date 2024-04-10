#include "masking.hpp"
#include "xyzio.hpp"
#include "slicetransform.hpp"
#include "scanimgio.hpp"
#include "predefines.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <stdint.h>
#include <filesystem>
#include <iostream>
#include <sstream>


int main(int32_t, char**)
{
    // Check if all of the require images are present
    // Load them all into cv::Mat or exit the program if they are not
    midproj::ScanImageIo::asset_check(PATH_TO_ALL_SCAN_IMAGES, {FRAME_CORNER_2D_FILE_PATH, FRAME_CORNER_3D_FILE_PATH, SCULPTURE_ROI_FILE_PATH});
    std::vector<cv::Mat> scanImages = midproj::ScanImageIo::load_all_images(PATH_TO_ALL_SCAN_IMAGES, SCAN_IMAGE_COUNT);

    // A Binary image where the sculpture and the frames are true, and false anywhere else.
    const cv::Mat foregroundMask = midproj::get_foreground_mask(scanImages, IMG_SIZE);

    // Vector of binarized scan images in which the red pixels are true and anywhere else false.
    std::vector<cv::Mat> redPixelMaps(SCAN_IMAGE_COUNT);
    for (size_t imgIndex = 0; imgIndex < SCAN_IMAGE_COUNT; imgIndex++)
    {
        redPixelMaps.at(imgIndex) = midproj::get_red_pixel_map(scanImages.at(imgIndex), foregroundMask);
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
    midproj::SliceTransform::init_front_panel_homography();

    // A vector of vectors containing the sculptures's red pixels' world coordinates.
    // The inner vector contains the red pixels' world coordinate and the outer vector contains such information derived from each scan images.
    std::vector<std::vector<cv::Point3d>> sculpturePointCloudWorldInEachSlice(SCAN_IMAGE_COUNT);

    // A Binary image where the red pixels in a scan image on the frame beams are true, and false anywhere else.
    cv::Mat onBeamRedPixelMap;
    // A Binary image where the red pixels in a scan image on the sculpture are true, and false anywhere else.
    cv::Mat sculptureRedPixelMap;
    for (size_t imgIndex = 0; imgIndex < SCAN_IMAGE_COUNT; imgIndex++)
    {
        cv::bitwise_and(scannedFrameMask, redPixelMaps.at(imgIndex), onBeamRedPixelMap);
        std::array<cv::Point2f, midproj::SliceTransform::BEAM_COUNT> avgRedPixelOnEachBeam = midproj::SliceTransform::get_red_pixels_on_frame(onBeamRedPixelMap);
        cv::bitwise_and(scannedSculptureMask, redPixelMaps.at(imgIndex), sculptureRedPixelMap);

        midproj::SliceTransform::refine_sculpture_pixel_map(sculptureRedPixelMap, 2);
        sculpturePointCloudWorldInEachSlice.at(imgIndex) = midproj::SliceTransform::get_slice_world_coordinate(avgRedPixelOnEachBeam, sculptureRedPixelMap);
    }
    const bool fileOutputStatus = midproj::XyzIo::write_xyz_point_cloud_file(OUTPUT_XYZ_FILE, sculpturePointCloudWorldInEachSlice);
    if (!fileOutputStatus)
        std::cout << "File output error." << '\n';
    else
        std::cout << "Xyz file has been written to " << std::filesystem::absolute(OUTPUT_XYZ_FILE) << '\n';
    return 0;
}