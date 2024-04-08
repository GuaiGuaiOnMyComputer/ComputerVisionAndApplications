#include <opencv2/opencv.hpp>
#include <array>
#include <filesystem>

namespace midproj
{
    class SliceTransform
    {
    public:

        enum BeamIndex : int32_t
        {
            NEAR_TOP = 0,   // Top beam on near surface
            NEAR_BOT,       // Bottom beam on near surface
            FAR_TOP,        // Top beam on far surface
            FAR_BOT,        // Bottom beam on far surface
            BEAM_COUNT      // number of beams = 4
        };

        enum CornerIndex : int32_t
        {
            NEAR_TL = 0,    // Top left corner on near surface
            NEAR_TR,        // Top right corner on near surface
            NEAR_BL,        // Bottom left corner on near surface
            NEAR_BR,        // Bottom right corner on near surface
            FAR_TL,         // Top left corner on far surface
            FAR_TR,         // Top right corner on far surface
            FAR_BL,         // Bottom left corner on far surface
            FAR_BR,         // Bottom right corner on far surface
            CORNER_COUNT    // Number of corners = 8
        };

        static std::array<cv::Point2f, BEAM_COUNT> get_red_pixels_on_frame(cv::InputArray redPixelMap);
        static void fit_frame_beam_lines();
        static void load_frame_corners_from_vector(const std::vector<cv::Point2i>& cornerPointsImg, const std::vector<cv::Point3i>& cornerPointsWorld);
        static void init_front_panel_homography();
        static void init_left_panel_homography();
        static std::vector<cv::Point3d> get_slice_world_coordinate(const std::array<cv::Point2f, BEAM_COUNT> &pointsOnBeams, cv::InputArray sculpturePixelMap);

    private:
        static inline bool _check_if_on_line(const cv::Vec3f& line2d, const cv::Point2f& point, const double maxDistance);
        static BeamIndex _get_pixel_nearest_beam_index(const cv::Point2f &point);
        static cv::Point2f _get_avg_coordinate_of_pixel_on_beam(const std::vector<cv::Point2f> &allRedPixelCoors, const std::vector<BeamIndex> &allPixelBeamIndice, const BeamIndex beamIndex);
        static double _get_world_x_from_front_panel_homography(const std::array<cv::Point2f, BEAM_COUNT>& pointsOnBeams);
        static std::vector<cv::Vec2f> _get_world_yz_from_homography_projection(cv::InputArray homographyMat, const std::vector<cv::Point2f>& pixelCoorsOnImage);

    private : 
        static std::array<cv::Vec3f, BEAM_COUNT> s_beamLines;
        static std::array<cv::Point2f, CORNER_COUNT> s_frameCornersOnImageCoor;
        static std::array<cv::Point3f, CORNER_COUNT> s_frameCornersOnWorldCoor;
        static cv::Mat s_frontPanelHomographyMat;
        static bool s_beamLinesAreFitted;
    };
}