class XyzParser
{
public:
    XyzParser(std::filesystem::path xyzFilePath);
    void ParseAll(const int64_t headerLen = -1, std::vector<float> &out_trackPoints);

    virtual ~XyzParser(){};

private:
    uint64_t _getHeaderLen();
    std::array<float, 3> _parseLine(const std::string &lineCount);

private:
    bool _fileExist;
    std::filesystem::path _fileBasename;
    std::ifstream _fileHandle;
    uint64_t _dataStart;
    const std::filesystem::path _xyzFilePath;
};