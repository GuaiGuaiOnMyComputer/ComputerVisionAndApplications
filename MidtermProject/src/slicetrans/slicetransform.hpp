#include <opencv2/opencv.hpp>

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
        
        struct FrameBeamLines
        {
            cv::Vec4f BeamNearTop;
            cv::Vec4f BeamNearBot;
            cv::Vec4f BeamFarTop;
            cv::Vec4f BeamFarBot;
        };

        static std::vector<cv::Point2f> get_red_pixels_on_frame(cv::InputArray redPixelMap);
        static FrameBeamLines fit_frame_beam_lines(FrameBeamCornerPoints cornerPoints);


        private : 
        static cv::Vec4f _nearUpperBeamLine;
        static cv::Vec4f _nearLowerBeamLine;
        static cv::Vec4f _farUpperBeamLine;
        static cv::Vec4f _farLowerBeamLine;
    };

    cv::Vec4f SliceTransform::_nearUpperBeamLine;
    cv::Vec4f SliceTransform::_nearLowerBeamLine;
    cv::Vec4f SliceTransform::_farUpperBeamLine;
    cv::Vec4f SliceTransform::_farLowerBeamLine;
}