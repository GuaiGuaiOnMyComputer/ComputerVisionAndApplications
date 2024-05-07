#include <filesystem>
#include <vector>

namespace hw4
{
    namespace fs = std::filesystem;

    class AssetCheck
    {
        public:
        static bool check_if_asset_exist(const fs::path& filePath);
        static bool check_if_asset_exist(const std::vector<fs::path>& filePath);

        template<size_t Tcount>
        static bool check_if_asset_exist(const std::array<fs::path, Tcount>& filePaths)
        {
            for (size_t i = 0; i < filePaths.size(); i++)
            {
                if (!check_if_asset_exist(filePaths[i]))
                    return false;
            }
            return true;
        }

    };

} // namespace hw3