#pragma once
#include <filesystem>
#include <string>

namespace atomic_write {
    bool write(const std::filesystem::path& target, const std::string& content);
    bool write_unchecked(const std::filesystem::path& target, const std::string& content);
}
