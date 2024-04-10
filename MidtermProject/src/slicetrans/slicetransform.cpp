#include <opencv2/opencv.hpp>
#include "slicetransform.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdint.h>

namespace midproj
{
    /// @brief Acquires 4 averaged coordinates of the red pixels on the 4 frame beams in a redPixelMapOnBeam.
    /// @param redPixelMapOnBeam The red pixel map in image coordinate system to extract 4 averaged coordinate from.
    /// @return Array of 4 average coordinates of the red pixels on the 4 frame beams.
    std::array<cv::Point2f, SliceTransform::BEAM_COUNT> SliceTransform::get_red_pixels_on_frame(cv::InputArray redPixelMapOnBeam)
    {
        assert(s_beamLinesAreFitted);
        std::array<cv::Point2f, BEAM_COUNT> avgCoorOnFrameBeams;
        std::vector<cv::Point2i> redPixelCoors2i;
        cv::findNonZero(redPixelMapOnBeam, redPixelCoors2i);
        const size_t pixelOnFrameCount = redPixelCoors2i.size();
        std::vector<SliceTransform::BeamIndex> beamIndiceOfEachPixel(pixelOnFrameCount);
        std::vector<cv::Point2f> redPixelCoors2f(pixelOnFrameCount);
        for (size_t i = 0; i < pixelOnFrameCount; i++)
            redPixelCoors2f.at(i) = cv::Point2f(redPixelCoors2i.at(i));

        for (size_t i = 0; i < pixelOnFrameCount; i++)
            beamIndiceOfEachPixel.at(i) = _get_pixel_nearest_beam_index(redPixelCoors2f.at(i));

        for (uint32_t beamIndex = 0; beamIndex != BEAM_COUNT; beamIndex++)
            avgCoorOnFrameBeams.at(beamIndex) = _get_avg_coordinate_of_pixel_on_beam(redPixelCoors2f, beamIndiceOfEachPixel, static_cast<BeamIndex>(beamIndex));
        return avgCoorOnFrameBeams;
    }

    std::array<cv::Point2f, SliceTransform::CORNER_COUNT> SliceTransform::get_corner_image_coordinates()
    {
        return SliceTransform::s_frameCornersImgCoor;
    }

    /// @brief Acquires a copy of the static private class member s_frontPanelHomographyMat
    /// @return Copy of the specified class member
    cv::Mat SliceTransform::get_front_panel_homography_mat() 
    {
        return SliceTransform::s_frontPanelHomographyMat.clone();
    }

    /// @brief Acquires a copy of the static private class member s_leftPanelHomographyMat.
    /// @return Copy of the specified class member.
    cv::Mat SliceTransform::get_left_panel_homography_mat() 
    {
        return SliceTransform::s_leftPanelHomographyMat.clone();
    }

    /// @brief Initializes the static private class member s_frontPanelHomgraphyMat used to find the world x coordinate of each slice. Front panel homography mat is shared among all slices.
    void SliceTransform::init_front_panel_homography()
    {
        std::array<cv::Point2f, 4> frontPanelVerticsWorld;
        frontPanelVerticsWorld[NEAR_TL] = cv::Point2f(s_frameCornersWorldCoor[NEAR_TL].x, s_frameCornersWorldCoor[NEAR_TL].z);
        frontPanelVerticsWorld[NEAR_TR] = cv::Point2f(s_frameCornersWorldCoor[NEAR_TR].x, s_frameCornersWorldCoor[NEAR_TR].z);
        frontPanelVerticsWorld[NEAR_BL] = cv::Point2f(s_frameCornersWorldCoor[NEAR_BL].x, s_frameCornersWorldCoor[NEAR_BL].z);
        frontPanelVerticsWorld[NEAR_BR] = cv::Point2f(s_frameCornersWorldCoor[NEAR_BR].x, s_frameCornersWorldCoor[NEAR_BR].z);

        std::array<cv::Point2f, 4> frontPanelVerticsImage;
        frontPanelVerticsImage[NEAR_TL] = cv::Point2f(s_frameCornersImgCoor[NEAR_TL].x, s_frameCornersImgCoor[NEAR_TL].y);
        frontPanelVerticsImage[NEAR_TR] = cv::Point2f(s_frameCornersImgCoor[NEAR_TR].x, s_frameCornersImgCoor[NEAR_TR].y);
        frontPanelVerticsImage[NEAR_BL] = cv::Point2f(s_frameCornersImgCoor[NEAR_BL].x, s_frameCornersImgCoor[NEAR_BL].y);
        frontPanelVerticsImage[NEAR_BR] = cv::Point2f(s_frameCornersImgCoor[NEAR_BR].x, s_frameCornersImgCoor[NEAR_BR].y);
        s_frontPanelHomographyMat = cv::findHomography(frontPanelVerticsImage, frontPanelVerticsWorld);
    }

    /// @brief Initialize the static private class member s_leftPanelHomographyMat used to find world y and z coordinates of each slice. Left panel homography mat is specific to each slice.
    /// @param pointsOnBeams Average coordiantes of the red pixels on frame beams in e slice.
    void SliceTransform::init_left_panel_homography(const std::array<cv::Point2f, BEAM_COUNT>& pointsOnBeams)
    {
        std::array<cv::Point2f, BEAM_COUNT> frameCornersOnWorldCoor2d;
        frameCornersOnWorldCoor2d[NEAR_TOP] = cv::Point2f(s_frameCornersWorldCoor[NEAR_TL].y, s_frameCornersWorldCoor[NEAR_TL].z);
        frameCornersOnWorldCoor2d[NEAR_BOT] = cv::Point2f(s_frameCornersWorldCoor[NEAR_BL].y, s_frameCornersWorldCoor[NEAR_BL].z);
        frameCornersOnWorldCoor2d[FAR_TOP] = cv::Point2f(s_frameCornersWorldCoor[FAR_TL].y, s_frameCornersWorldCoor[FAR_TL].z);
        frameCornersOnWorldCoor2d[FAR_BOT] = cv::Point2f(s_frameCornersWorldCoor[FAR_BL].y, s_frameCornersWorldCoor[FAR_BL].z);
        SliceTransform::s_leftPanelHomographyMat = cv::findHomography(pointsOnBeams, frameCornersOnWorldCoor2d);
    }

    /// @brief Get the world coordinates of every true pixel in sculpturePixelMap
    /// @param pointsOnBeams 4 averaged coordinates of the red pixels on the frame beams in image coordinate system.
    /// @param sculpturePixelMap A binary image where the pixels of red scan line on the sculpture is true, and false anywhere else.
    /// @return World coordinate of all the true pixels in sculpturePixelMap.
    std::vector<cv::Point3d> SliceTransform::get_slice_world_coordinate(const std::array<cv::Point2f, BEAM_COUNT>& pointsOnBeams, cv::InputArray sculpturePixelMap)
    {
        std::vector<cv::Point2f> sculptureRedPixelCoorsImg2f;
        cv::findNonZero(sculpturePixelMap, sculptureRedPixelCoorsImg2f);
        const size_t sculptureRedPixelCount = sculptureRedPixelCoorsImg2f.size();
        std::vector<cv::Point3d> sculptureRedPixelCoorsWorld3f(sculptureRedPixelCount);
        double sliceWorldX = _get_world_x_from_front_panel_homography(pointsOnBeams);

        const std::vector<cv::Vec2f> yzOnWorldCoors = _get_world_yz_from_left_panel_homography(sculptureRedPixelCoorsImg2f);

        for (size_t i = 0; i < sculptureRedPixelCount; i++)
            sculptureRedPixelCoorsWorld3f.at(i) = cv::Point3d(sliceWorldX, yzOnWorldCoors.at(i)[0], yzOnWorldCoors.at(i)[1]);

        return sculptureRedPixelCoorsWorld3f;
    }

    /// @brief Further reduce the thickness of the scan profile by erosion in sculpturePixelMap for better results.
    /// @param sculpturePixelMap Binary image where the red pixels in the sculpture surface is true, and false anywhere else.
    /// @param iterations Iterations of erosion
    void SliceTransform::refine_sculpture_pixel_map(cv::InputOutputArray sculpturePixelMap, int32_t iterations)
    {
        cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size2i(3, 3));
        cv::erode(sculpturePixelMap, sculpturePixelMap, structuringElement, cv::Point2i(-1, -1), iterations);
    }

    /// @brief Loads the predefined frame coordinates in both image coordinate space and world coordinate space into private static members s_frameCornersImgCoor and s_frameCornersWorldCoors.
    /// @param cornerPointsImg 8 of the predefined corner coordinates of the frame in image coordinate system.
    /// @param CornerPointsWorld 8 of the predefined corner coordinates of the frame in world coordiante system.
    void SliceTransform::load_frame_corners_from_vector(const std::vector<cv::Point2i>& cornerPointsImg, const std::vector<cv::Point3i>& cornerPointsWorld)
    {
        using midproj::SliceTransform;
        s_frameCornersImgCoor[NEAR_TL] = cornerPointsImg[2];
        s_frameCornersImgCoor[NEAR_TR] = cornerPointsImg[3];
        s_frameCornersImgCoor[NEAR_BL] = cornerPointsImg[6];
        s_frameCornersImgCoor[NEAR_BR] = cornerPointsImg[7];
        s_frameCornersImgCoor[FAR_TL] = cornerPointsImg[1];
        s_frameCornersImgCoor[FAR_TR] = cornerPointsImg[0];
        s_frameCornersImgCoor[FAR_BR] = cornerPointsImg[4];
        s_frameCornersImgCoor[FAR_BL] = cornerPointsImg[5];

        s_frameCornersWorldCoor[NEAR_TL] = cornerPointsWorld[2];
        s_frameCornersWorldCoor[NEAR_TR] = cornerPointsWorld[3];
        s_frameCornersWorldCoor[NEAR_BL] = cornerPointsWorld[6];
        s_frameCornersWorldCoor[NEAR_BR] = cornerPointsWorld[7];
        s_frameCornersWorldCoor[FAR_TL] = cornerPointsWorld[1];
        s_frameCornersWorldCoor[FAR_TR] = cornerPointsWorld[0];
        s_frameCornersWorldCoor[FAR_BR] = cornerPointsWorld[4];
        s_frameCornersWorldCoor[FAR_BL] = cornerPointsWorld[5];
    }

    /// @brief Fit the line of the 4 beams in image coordinate system using the point coordinates specified by s_frameBeamCorners.
    void SliceTransform::fit_frame_beam_lines()
    {
        using PointPair = std::array<cv::Point2i, 2>;
        using midproj::SliceTransform;

        // fit the line of beam from a pair of corner points using cv::fitLine
        std::array<cv::Vec4f, BEAM_COUNT> beamLines4f;
        cv::fitLine(PointPair{s_frameCornersImgCoor[NEAR_TL], s_frameCornersImgCoor[NEAR_TR]}, beamLines4f[NEAR_TOP], cv::DistanceTypes::DIST_L1, 0, 0.01, 0.01);
        cv::fitLine(PointPair{s_frameCornersImgCoor[NEAR_BL], s_frameCornersImgCoor[NEAR_BR]}, beamLines4f[NEAR_BOT], cv::DistanceTypes::DIST_L1, 0, 0.01, 0.01);
        cv::fitLine(PointPair{s_frameCornersImgCoor[FAR_TL], s_frameCornersImgCoor[FAR_TR]}, beamLines4f[FAR_TOP], cv::DistanceTypes::DIST_L1, 0, 0.01, 0.01);
        cv::fitLine(PointPair{s_frameCornersImgCoor[FAR_BL], s_frameCornersImgCoor[FAR_BR]}, beamLines4f[FAR_BOT], cv::DistanceTypes::DIST_L1, 0, 0.01, 0.01);

        // convert each line fitted by cv::fitLine from cv::Vec4f to cv::Vec3f containing 3 line coefficients
        // the output of cv::fitLine is a cv::Vec4f where:
        // 0: the i component of line direction vector 
        // 1: the j component of line direction vector
        // 2: the x component of an aribritary point coordinate on line
        // 3: the y component of an aribritary point coordinate on line
        for (size_t beamIndex = 0; beamIndex < BEAM_COUNT; beamIndex++)
        {
            // Line equation L:ax + by + c = 0
            //     For a point P(xp, yp) on L, axp + byp + c = 0
            //     c = -axp -byp
            //     the resulting fitted line in the form of cv::Vec3f is {a, b, c}, where a and b are normalized.
            // For a direction vector s{s1, s2} of line L: ax + by + c = 0:
            //     a = s2
            //     b = -s1

            const float a = beamLines4f[beamIndex][1];
            const float b = -beamLines4f[beamIndex][0];
            const float xp = beamLines4f[beamIndex][2];
            const float yp = beamLines4f[beamIndex][3];
            const float c = - a * xp - b * yp;
            s_beamLinesImgCoor[beamIndex] = cv::Vec3f(a, b, c);
        }

        s_beamLinesAreFitted = true;
    }

    /// @brief Classify which beam the given point belongs to by evaluating its distance to the beam's line.
    /// @param point Point to classify.
    /// @return Index of the beam that point belongs to. Returns -1 when point is far away from all 4 beams.
    SliceTransform::BeamIndex SliceTransform::_get_pixel_nearest_beam_index(const cv::Point2f& point)
    {
        for (int32_t beamIndex = 0; beamIndex != BEAM_COUNT; beamIndex++)
        {
            if (_check_if_on_line(SliceTransform::s_beamLinesImgCoor[beamIndex], point, 12))
                return static_cast<SliceTransform::BeamIndex>(beamIndex);
        }
        return static_cast<SliceTransform::BeamIndex>(-1); // returns an error -1 if this pixel is not near to any beam lines
    }

    double SliceTransform::_get_world_x_from_front_panel_homography(const std::array<cv::Point2f, SliceTransform::BEAM_COUNT> &pointsOnBeams)
    {
        const std::array<cv::Vec2f, 2> pointsOnFrontPanelBeamsImg = {pointsOnBeams[NEAR_TOP], pointsOnBeams[NEAR_BOT]};
        std::array<cv::Vec2f, 2> pointsOnFrontPanelBeamsWorld;
        cv::perspectiveTransform(pointsOnFrontPanelBeamsImg, pointsOnFrontPanelBeamsWorld, s_frontPanelHomographyMat);
        double avgXCoordinateWorld = (pointsOnFrontPanelBeamsWorld[0][0] + pointsOnFrontPanelBeamsWorld[1][0]) / 2.0;
        return avgXCoordinateWorld;
    }

    std::vector<cv::Vec2f> SliceTransform::_get_world_yz_from_left_panel_homography(const std::vector<cv::Point2f>& pixelsOnImageCoor)
    {
        std::vector<cv::Vec2f> pixelsOnLeftPanelCoor(pixelsOnImageCoor.size());
        cv::perspectiveTransform(pixelsOnImageCoor, pixelsOnLeftPanelCoor, SliceTransform::s_leftPanelHomographyMat);
        return pixelsOnLeftPanelCoor;
    }

    /// @brief Find the average location of pixels in onBeamPixelsImgCoor that are close to the beam spevified by beamIndex.
    /// @param onBeamPixelsImgCoor All the coordinates of pixels on beams. Elements must have the same order as allPixelBeamIndece.
    /// @param pixelBeamIndice The beam indice of each pixel in onBeamPixelsImgCoor found by calling method _get_pixel_nearest_beam_index. Must have save element order as onBeamPixelsImgCoor.
    /// @param beamIndex Index of the beam to determine the average coordinate.
    /// @return The average coordinate of the pixels on a specific beam.
    cv::Point2f SliceTransform::_get_avg_coordinate_of_pixel_on_beam(const std::vector<cv::Point2f>& onBeamPixelsImgCoor, const std::vector<SliceTransform::BeamIndex>& pixelBeamIndice, const SliceTransform::BeamIndex beamIndex)
    {
        cv::Point2f avgPixelCoorOnBeam(0, 0);
        size_t pixelsOnBeamCount = std::count_if(pixelBeamIndice.cbegin(), pixelBeamIndice.cend(), 
            [&beamIndex](SliceTransform::BeamIndex beamOfPixel){ return beamOfPixel == beamIndex; });

        for (size_t i = 0; i < onBeamPixelsImgCoor.size(); i++)
            avgPixelCoorOnBeam += (pixelBeamIndice.at(i) == beamIndex) * onBeamPixelsImgCoor.at(i);
        avgPixelCoorOnBeam.x /= pixelsOnBeamCount;
        avgPixelCoorOnBeam.y /= pixelsOnBeamCount;
        return avgPixelCoorOnBeam;
    }

    /// @brief Determines if a given point is close to a given line.
    /// @param line2d The 2D line coefficients.
    /// @param point Point coordinate to check.
    /// @param maxDistance Threshold of the distance between point and line beyond which the point is considered off the line and vice versa.
    /// @return True if distance between point and line2d is below maxDistance.
    inline bool SliceTransform::_check_if_on_line(const cv::Vec3f& line2d, const cv::Point2f& point, const double maxDistance)
    {
        return std::fabs(line2d[0] * point.x + line2d[1] * point.y + line2d[2]) < maxDistance;
    }

    std::array<cv::Vec3f, SliceTransform::BEAM_COUNT> SliceTransform::s_beamLinesImgCoor;
    std::array<cv::Point2f, SliceTransform::CORNER_COUNT> SliceTransform::s_frameCornersImgCoor;
    std::array<cv::Point3f, SliceTransform::CORNER_COUNT> SliceTransform::s_frameCornersWorldCoor;
    bool SliceTransform::s_beamLinesAreFitted = false;
    cv::Mat SliceTransform::s_frontPanelHomographyMat;
    cv::Mat SliceTransform::s_leftPanelHomographyMat;
} // namespace midproj
