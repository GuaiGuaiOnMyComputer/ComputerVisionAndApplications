#include <array>
#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <vector>

class Camera
{
public:
    Camera(const char* name): _kMat{0}, _rtMat{0}, _cameraName{name}, _transformMat{0}
    {
    }

    Camera(): _kMat{0}, _rtMat{0}, _cameraName{"No name"}, _transformMat{0}
    {
    }

    Camera(const char* name, const std::string& parameterString, const char* kMatName, const char* rtMatName): _kMat{0}, _rtMat{0}, _cameraName{name}, _transformMat{0}
    {
        GetParameterFromText(parameterString, kMatName, rtMatName);
    }

    void GetParameterFromText(const std::string& textBuff, const char* kMatName, const char* rtMatName)
    {
        // specify the begining character of the context of the arrays
        // offset strlen(kMatName) + 2 characters to skip the name of the matrix and the \n [ charachters
        size_t kMatStart = textBuff.find(kMatName) + strlen(kMatName) + 2;
        size_t rtMatStart= textBuff.find(rtMatName) + strlen(rtMatName) + 2;

        // parse the floats as elements in _rtMat and _kMat
        // \t \n characters and empty spaces are skipped by strtof
        char *floatEndPtr = nullptr;
        const char *floatStartPtr = &textBuff[rtMatStart];
        for (size_t i = 0; i < 3 * 4; i++)
        {
            _rtMat.at<float>(i) = strtof(floatStartPtr, &floatEndPtr);
            floatStartPtr = floatEndPtr;
        }

        floatStartPtr = &textBuff[kMatStart];
        for (size_t i = 0; i < 3 * 3; i++)
        {
            _kMat.at<float>(i) = strtof(floatStartPtr, &floatEndPtr);
            floatStartPtr = floatEndPtr;
        }
        _initTransformMatrix();
    }

    void GetParameterFromFile(const std::filesystem::path filePath, const char* kMatName, const char* rtMatName)
    {
        std::ifstream cameraParamFileHandle;
        try
        {
            cameraParamFileHandle.open(filePath, std::ios::in);
            cameraParamFileHandle.seekg(0, cameraParamFileHandle.end);
            size_t characterCount = cameraParamFileHandle.tellg();
            cameraParamFileHandle.seekg(0, cameraParamFileHandle.beg);
            std::string fileContent(characterCount, '\0');
            cameraParamFileHandle.read(fileContent.data(), fileContent.capacity());
            cameraParamFileHandle.close();
            GetParameterFromText(fileContent, kMatName, rtMatName);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            cameraParamFileHandle.close();
        }
        _initTransformMatrix();
    }

    /// @brief Remaps the points in pointsInWorldNoneHomo to the image coordinate system in place.
    /// @param pointsInWorldNoneHomo The points in world coordinate. Each point occupies 3 consecutive elements in this std::vector as its x, y, and z components
    std::vector<cv::Point3f> WorldToImageCoordinate(const std::vector<cv::Point3f>& pointsInWorldNoneHomo)
    {
        std::vector<cv::Point3f> pointsInImageHomo(pointsInWorldNoneHomo.size());
        for (size_t i = 0; i < pointsInWorldNoneHomo.size(); i++)
        {
            cv::Mat pointInImageHomo = MapToImage(pointsInWorldNoneHomo[i]);
            pointsInImageHomo[i] = cv::Point3f(pointInImageHomo.at<float>(0), pointInImageHomo.at<float>(1), pointInImageHomo.at<float>(2));
        }
        return pointsInImageHomo;
    }

    cv::Mat MapToImage(const cv::Point3f& pointInWorldNoneHomo)
    {
        cv::Vec4f pointInWorldHomo(pointInWorldNoneHomo.x, pointInWorldNoneHomo.y, pointInWorldNoneHomo.z, 1);
        cv::Vec2f pointInImageNoneHomo;
        return _transformMat *pointInWorldHomo;
    }

public:

private:
    void _initTransformMatrix()
    {
        // TODO: please implement this
    }

private:
    cv::Mat _rtMat;
    cv::Mat _kMat;
    cv::Mat _transformMat;
    std::string _cameraName;
};