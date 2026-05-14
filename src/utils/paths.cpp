#include "utils/paths.h"
#include <cstdlib>

namespace paths {
    std::filesystem::path config_dir() {
        const char* home = std::getenv("HOME");
        if (!home) return {};
        return std::filesystem::path(home) / ".config" / "ccm";
    }

    std::filesystem::path config_file() {
        return config_dir() / "config.toml";
    }

    std::filesystem::path backup_dir() {
        return config_dir() / "backups";
    }

    std::filesystem::path draft_dir() {
        return config_dir() / "draft";
    }

    std::filesystem::path draft_settings() {
        return draft_dir() / "settings.json";
    }

    std::filesystem::path claude_config() {
        const char* home = std::getenv("HOME");
        if (!home) return {};
        return std::filesystem::path(home) / ".claude" / "settings.json";
    }
}
