#include <filesystem>
#include <vector>

namespace hw3
{
    namespace fs = std::filesystem;

    class AssetCheck
    {
        public:
        static bool check_if_asset_exist(const fs::path& filePath);
        static bool check_if_asset_exist(const std::vector<fs::path>& filePath);
    };

} // namespace hw3