#pragma once
#include <filesystem>

namespace paths {
    std::filesystem::path config_dir();
    std::filesystem::path config_file();
    std::filesystem::path backup_dir();
    std::filesystem::path draft_dir();
    std::filesystem::path draft_settings();
    std::filesystem::path claude_config();
}
