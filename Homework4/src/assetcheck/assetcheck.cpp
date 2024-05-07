#include <filesystem>
#include <iostream>
#include <vector>
#include "assetcheck.hpp"

namespace hw4
{
    namespace fs = std::filesystem;

    bool AssetCheck::check_if_asset_exist(const fs::path& filePath)
    {
        if (!fs::exists(filePath))
        {
            std::cout << "File required at path: " << fs::absolute(filePath) << " is not found!";
            return false;
        }
        return true;
    }

    bool AssetCheck::check_if_asset_exist(const std::vector<fs::path>& filePaths)
    {
        for (size_t i = 0; i < filePaths.size(); i++)
        {
            if (!check_if_asset_exist(filePaths[i]))
                return false;
        }
        return true;
    }
} // namespace hw4
