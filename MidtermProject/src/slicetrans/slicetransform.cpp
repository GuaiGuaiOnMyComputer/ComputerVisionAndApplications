#include <opencv2/opencv.hpp>
#include "slicetransform.hpp"

namespace midproj
{
    /// @brief Acquires 4 averaged coordinates of the red pixels on the 4 frame beams in a redPixelMap.
    /// @param redPixelMap The red pixel map to extract 4 averaged coordinate from.
    /// @param scannedFrameMask A mask where the area on the frame beam that is scanned by the red line is white and black elsewhere
    /// @return something
    std::vector<cv::Point2f> SliceTransform::get_red_pixels_on_frame(cv::InputArray redPixelMap, cv::InputArray scannedFrameMask)
    {
        assert(s_beamLinesAreFitted);
        std::vector<cv::Point2i> redPixelCoors;
        redPixelCoors.reserve(500);

        cv::findNonZero(redPixelMap, redPixelCoors);
    }

    void SliceTransform::fit_frame_beam_lines(FrameBeamCornerPoints cornerPoints)
    {
        using PointPair = std::array<cv::Point2i, 2>;
        cv::fitLine(PointPair{cornerPoints.CornerNearTopLf, cornerPoints.CornerNearTopRt}, s_beamLines.BeamNearTop, cv::DistanceTypes::DIST_L1, 0, 0.01, 0.01);
        cv::fitLine(PointPair{cornerPoints.CornerNearBotLf, cornerPoints.CornerNearBotRt}, s_beamLines.BeamNearBot, cv::DistanceTypes::DIST_L1, 0, 0.01, 0.01);
        cv::fitLine(PointPair{cornerPoints.CornerFarTopLf, cornerPoints.CornerFarTopRt}, s_beamLines.BeamFarTop, cv::DistanceTypes::DIST_L1, 0, 0.01, 0.01);
        cv::fitLine(PointPair{cornerPoints.CornerFarBotLf, cornerPoints.CornerFarBotRt}, s_beamLines.BeamFarBot, cv::DistanceTypes::DIST_L1, 0, 0.01, 0.01);
        s_beamLinesAreFitted = true;
    }

    bool SliceTransform::_check_if_on_line(cv::Vec4f line2d, cv::Point2f point, double maxDistance)
    {
        return line2d[0] * point.x + line2d[1] * point.y + line2d[2] + line2d[3] < maxDistance;
    }

    SliceTransform::FrameBeamLines s_beamLines;
    bool SliceTransform::s_beamLinesAreFitted = false;
} // namespace midproj
