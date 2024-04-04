#include <opencv2/opencv.hpp>
#include <vector>
#include <filesystem>

namespace midproj
{
    namespace fs = std::filesystem;

    class VirtualCamera
    {

    public:
        VirtualCamera CreateFromFrameCornerPointPairs(const fs::path& pathTo2DPoints, const fs::path& pathTo3DPoints)
        {

        }

    private:

        VirtualCamera(){}

    private:
        cv::Mat _projectMatrix; // the product of the camera matrix and the T matrix, representing the combination of camrea's intrinsic and extrinsic paramerters

    };

} // namespace midproj
