#include <opencv2/opencv.hpp>
#include "slicetransform.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace midproj
{
    /// @brief Acquires 4 averaged coordinates of the red pixels on the 4 frame beams in a beamRedPixelMap.
    /// @param beamRedPixelMap The red pixel map to extract 4 averaged coordinate from.
    /// @param scannedFrameMask A mask where the area on the frame beam that is scanned by the red line is white and black elsewhere
    /// @return Array of 4 average coordinates of the red pixels on the 4 frame beams.
    std::array<cv::Point2f, SliceTransform::BEAM_COUNT> SliceTransform::get_red_pixels_on_frame(cv::InputArray beamRedPixelMap)
    {
        assert(s_beamLinesAreFitted);
        std::array<cv::Point2f, BEAM_COUNT> avgCoorOnFrameBeams;
        std::vector<cv::Point2i> redPixelCoors2i;
        cv::findNonZero(beamRedPixelMap, redPixelCoors2i);
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

    /// @brief Loads the corner points into private static member s_frameBeamCorners. The corner order of cornerPoints must not be changed.
    /// @param cornerPoints 8 of the corner coordinates of the frame.
    void SliceTransform::load_frame_corners_from_vector(const std::vector<cv::Point2i>& cornerPoints)
    {
        s_frameBeamCorners.CornerNearTopLf = cornerPoints[2];
        s_frameBeamCorners.CornerNearTopRt = cornerPoints[3];
        s_frameBeamCorners.CornerNearBotLf = cornerPoints[6];
        s_frameBeamCorners.CornerNearBotRt = cornerPoints[7];
        s_frameBeamCorners.CornerFarTopLf = cornerPoints[1];
        s_frameBeamCorners.CornerFarTopRt = cornerPoints[0];
        s_frameBeamCorners.CornerFarBotRt = cornerPoints[4];
        s_frameBeamCorners.CornerFarBotLf = cornerPoints[5];
    }

    /// @brief Fit the line of the 4 beams using the point coordinates specified by s_frameBeamCorners.
    void SliceTransform::fit_frame_beam_lines()
    {
        using PointPair = std::array<cv::Point2i, 2>;

        // fit the line of beam from a pair of corner points using cv::fitLine
        std::array<cv::Vec4f, BEAM_COUNT> beamLines4f;
        cv::fitLine(PointPair{s_frameBeamCorners.CornerNearTopLf, s_frameBeamCorners.CornerNearTopRt}, beamLines4f[NEAR_TOP], cv::DistanceTypes::DIST_L1, 0, 0.01, 0.01);
        cv::fitLine(PointPair{s_frameBeamCorners.CornerNearBotLf, s_frameBeamCorners.CornerNearBotRt}, beamLines4f[NEAR_BOT], cv::DistanceTypes::DIST_L1, 0, 0.01, 0.01);
        cv::fitLine(PointPair{s_frameBeamCorners.CornerFarTopLf, s_frameBeamCorners.CornerFarTopRt}, beamLines4f[FAR_TOP], cv::DistanceTypes::DIST_L1, 0, 0.01, 0.01);
        cv::fitLine(PointPair{s_frameBeamCorners.CornerFarBotLf, s_frameBeamCorners.CornerFarBotRt}, beamLines4f[FAR_BOT], cv::DistanceTypes::DIST_L1, 0, 0.01, 0.01);

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
            s_beamLines[beamIndex] = cv::Vec3f(a, b, c);
        }

        s_beamLinesAreFitted = true;
    }

    void SliceTransform::set_world_beam_length(double length)
    {
        SliceTransform::s_beamLengthWorld = length;
    }

    /// @brief Classify which beam the given point belongs to by evaluating its distance to the beam's line.
    /// @param point Point to classify.
    /// @return Index of the beam that point belongs to. Returns -1 when point is far away from all 4 beams.
    SliceTransform::BeamIndex SliceTransform::_get_pixel_nearest_beam_index(const cv::Point2f& point)
    {
        for (int32_t beamIndex = 0; beamIndex != BEAM_COUNT; beamIndex++)
        {
            if (_check_if_on_line(SliceTransform::s_beamLines[beamIndex], point, 12))
                return static_cast<SliceTransform::BeamIndex>(beamIndex);
        }
        return static_cast<SliceTransform::BeamIndex>(-1); // returns an error -1 if this pixel is not near to any beam lines
    }

    /// @brief Find the average location of pixels in allFrameBeamRedPixelCoors that are close to the beam spevified by beamIndex.
    /// @param allFrameBeamRedPixelCoors All the coordinates of pixels on beams. Elements must have the same order as allPixelBeamIndece.
    /// @param allPixelBeamIndice The beam indice of each pixel in allFrameBeamRedPixelCoors found by calling method _get_pixel_nearest_beam_index. Must have save element order as allFrameBeamRedPixelCoors.
    /// @param beamIndex Index of the beam to determine the average coordinate.
    /// @return The average coordinate of the pixels on a specific beam.
    cv::Point2f SliceTransform::_get_avg_coordinate_of_pixel_on_beam(const std::vector<cv::Point2f>& allFrameBeamRedPixelCoors, const std::vector<SliceTransform::BeamIndex>& allPixelBeamIndice, const SliceTransform::BeamIndex beamIndex)
    {
        cv::Point2f avgPixelCoorOnBeam(0, 0);
        size_t pixelsOnBeamCount = std::count_if(allPixelBeamIndice.cbegin(), allPixelBeamIndice.cend(), 
            [&beamIndex](SliceTransform::BeamIndex beamOfPixel){ return beamOfPixel == beamIndex; });

        for (size_t i = 0; i < allFrameBeamRedPixelCoors.size(); i++)
            avgPixelCoorOnBeam += (allPixelBeamIndice.at(i) == beamIndex) * allFrameBeamRedPixelCoors.at(i);
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

    std::array<cv::Vec3f, SliceTransform::BEAM_COUNT> SliceTransform::s_beamLines;
    SliceTransform::FrameBeamCornerPoints SliceTransform::s_frameBeamCorners;
    bool SliceTransform::s_beamLinesAreFitted = false;
    double SliceTransform::s_beamLengthWorld = -1;
} // namespace midproj
