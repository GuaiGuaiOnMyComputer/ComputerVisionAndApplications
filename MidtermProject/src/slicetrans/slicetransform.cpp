#include <opencv2/opencv.hpp>
#include "slicetransform.hpp"

namespace midproj
{
    std::vector<cv::Point2f> SliceTransform::get_red_pixels_on_frame(cv::InputArray redPixelMap)
    {
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
    }

    SliceTransform::FrameBeamLines s_beamLines;
} // namespace midproj
