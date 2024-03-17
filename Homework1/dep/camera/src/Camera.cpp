#include <array>
#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/core/types.hpp>

/// @brief container of a camera matrix and an rT matrix that can project 3D points onto a given 2D plane.
class Camera
{
public:
    Camera(const char* name):_cameraName{name}
    {
        _kMat = cv::Mat(3, 3, CV_32FC1);
        _rtMat = cv::Mat(3, 4, CV_32FC1);
        _transformMat = cv::Mat(3, 4, CV_32FC1);
    }

    Camera(): _cameraName{"No name"}
    {
        _kMat = cv::Mat(3, 3, CV_32FC1);
        _rtMat = cv::Mat(3, 4, CV_32FC1);
        _transformMat = cv::Mat(3, 4, CV_32FC1);
    }

    Camera(const char* name, const std::string& parameterString, const char* kMatName, const char* rtMatName): _kMat{0}, _rtMat{0}, _cameraName{name}, _transformMat{0}
    {
        GetParameterFromText(parameterString, kMatName, rtMatName);
    }

    /// @brief Loads the camera matrix and rT matrix from a string.
    /// @param textBuff The string containg information about both the camrea matrix and the rT matrix.
    /// @param kMatName Name of the camera matrix in textBuff
    /// @param rtMatName Name of the rT matrix in textBuff
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
        for (size_t i = 0; i < 3; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {
                _rtMat.at<float>(i, j) = strtof(floatStartPtr, &floatEndPtr);
                floatStartPtr = floatEndPtr;
            }
        }

        floatStartPtr = &textBuff[kMatStart];
        for (size_t i = 0; i < 3; i++)
        {
            for (size_t j = 0; j < 3; j++)
            {
                _kMat.at<float>(i, j) = strtof(floatStartPtr, &floatEndPtr);
                floatStartPtr = floatEndPtr;
            }
        }
        _initTransformMatrix();
    }

    /// @brief Loads the camera matrix and rT matrix from a file. Calls GetParameterFromText internally.
    /// @param textBuff Path to the file containg information about both the camrea matrix and the rT matrix.
    /// @param kMatName Name of the camera matrix in the file.
    /// @param rtMatName Name of the rT matrix in the file.
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

    /// @brief Projects a non-homogeneous point in 3D space onto a plane.
    /// @param pointInWorldNoneHomo The coordinate of a non-homogeneous point.
    /// @return The homogeneous coordinate of the projected point on the plane.
    cv::Mat MapToImage(const cv::Point3f& pointInWorldNoneHomo)
    {
        cv::Vec4f pointInWorldHomo(pointInWorldNoneHomo.x, pointInWorldNoneHomo.y, pointInWorldNoneHomo.z, 1);
        cv::Mat pointInImageHomo = _transformMat *pointInWorldHomo;
        pointInImageHomo /= pointInImageHomo.at<float>(2);
        return pointInImageHomo;
    }

public:

private:

    /// @brief Multiplies the _kMat and _rtMat together
    void _initTransformMatrix()
    {
        _transformMat = _kMat * _rtMat;
    }

private:
    cv::Mat _rtMat;
    cv::Mat _kMat;
    cv::Mat _transformMat;
    std::string _cameraName;
};