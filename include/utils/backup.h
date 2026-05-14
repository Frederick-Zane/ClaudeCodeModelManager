#pragma once
#include <filesystem>

namespace backup {
    bool create(const std::filesystem::path& source);
    bool rotate(const std::filesystem::path& backup_dir, size_t max_count = 20);
}
