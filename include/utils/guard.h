#pragma once
#include <filesystem>

namespace guard {
    // Returns true if target is a live Claude Code config that must not be written to.
    bool is_protected(const std::filesystem::path& target);
}
