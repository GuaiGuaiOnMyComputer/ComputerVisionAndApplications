#include <array>
#include <string>

class Camera
{
public:
    Camera(): _kMat{0}, _rtMat{0}, _cameraName{"No name"}
    {
    }

    Camera(const std::string& parameterString, const char* kMatName, const char* rtMatName): _kMat{0}, _rtMat{0}, _cameraName{"No name"}
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
        char **floatEndPtr;
        for (size_t i = 0; i < 3 * 4; i++)
            _rtMat[i] = strtof(&textBuff[rtMatStart], floatEndPtr);

        for (size_t i = 0; i < 3 * 3; i++)
            _kMat[i] = strtof(&textBuff[kMatStart], floatEndPtr);
    }
public:


private:
    std::array<float, 4 * 3> _rtMat;
    std::array<float, 3 * 3> _kMat;
    std::string _cameraName;
};