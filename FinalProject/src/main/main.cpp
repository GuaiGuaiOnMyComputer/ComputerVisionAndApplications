#include <opencv2/opencv.hpp>
#include "scanimageio.hpp"

int main(int, char**)
{
    std::error_code fileSystemErrorCode;
    finprj::ScanImageIo scanImageIo(std::filesystem::path("../assets/SBS images/"), ".jpg", fileSystemErrorCode);
    for (size_t i = 0; i < 150; i++)
    {
        finprj::ImagePair imgPair = scanImageIo.GetNextPair();
    }
    return 0;
}