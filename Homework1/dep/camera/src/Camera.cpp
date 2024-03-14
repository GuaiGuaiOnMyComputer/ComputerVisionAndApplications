#include <array>
#include <string>
#include <algorithm>

class Camera
{
public:
    Camera(const char* name): _kMat{0}, _rtMat{0}, _cameraName{name}
    {
    }

    Camera(): _kMat{0}, _rtMat{0}, _cameraName{"No name"}
    {
    }

    Camera(const char* name, const std::string& parameterString, const char* kMatName, const char* rtMatName): _kMat{0}, _rtMat{0}, _cameraName{name}
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
            _rtMat[i] = strtof(floatStartPtr, &floatEndPtr);
            floatStartPtr = floatEndPtr;
        }

        floatStartPtr = &textBuff[kMatStart];
        for (size_t i = 0; i < 3 * 3; i++)
        {
            _kMat[i] = strtof(floatStartPtr, &floatEndPtr);
            floatStartPtr = floatEndPtr;
        }
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
    }

    /// @brief Remaps the points in pointsInWorld to the image coordinate system in place.
    /// @param pointsInWorld The points in world coordinate. Each point occupies 3 consecutive elements in this std::vector as its x, y, and z coordinate
    void WorldToImageCoordinate(std::vector<float>& pointsInWorld)
    {
    }

public:


private:
    std::array<float, 4 * 3> _rtMat;
    std::array<float, 3 * 3> _kMat;
    std::string _cameraName;
};