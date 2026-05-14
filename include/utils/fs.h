#pragma once
#include <filesystem>

namespace fs_util {
    bool ensure_dir(const std::filesystem::path& dir);
}
