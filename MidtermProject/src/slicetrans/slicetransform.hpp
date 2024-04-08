#include <opencv2/opencv.hpp>
#include <array>
#include <filesystem>

namespace midproj
{
    class SliceTransform
    {
    public:
        struct FrameBeamCornerPoints
        {
            cv::Point2i CornerNearTopLf;
            cv::Point2i CornerNearTopRt;
            cv::Point2i CornerNearBotLf;
            cv::Point2i CornerNearBotRt;
            cv::Point2i CornerFarTopLf;
            cv::Point2i CornerFarTopRt;
            cv::Point2i CornerFarBotRt;
            cv::Point2i CornerFarBotLf;
        };

        enum BeamIndex : int32_t
        {
            NEAR_TOP = 0,
            NEAR_BOT,
            FAR_TOP,
            FAR_BOT,
            BEAM_COUNT
        };

        static std::array<cv::Point2f, BEAM_COUNT> get_red_pixels_on_frame(cv::InputArray redPixelMap);
        static void fit_frame_beam_lines();
        static void load_frame_corners_from_vector(const std::vector<cv::Point2i>& cornerPoints);
        static void set_world_beam_length(double length);

    private:
        static inline bool _check_if_on_line(const cv::Vec3f& line2d, const cv::Point2f& point, const double maxDistance);
        static BeamIndex _get_pixel_nearest_beam_index(const cv::Point2f &point);
        static cv::Point2f _get_avg_coordinate_of_pixel_on_beam(const std::vector<cv::Point2f> &allRedPixelCoors, const std::vector<BeamIndex> &allPixelBeamIndice, const BeamIndex beamIndex);

    private : 
        static std::array<cv::Vec3f, BEAM_COUNT> s_beamLines;
        static bool s_beamLinesAreFitted;
        static FrameBeamCornerPoints s_frameBeamCorners;
        static double s_beamLengthWorld;
    };
}