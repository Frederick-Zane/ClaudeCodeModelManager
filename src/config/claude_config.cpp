#include "config/claude_config.h"
#include "utils/paths.h"
#include "utils/backup.h"
#include "utils/atomic_write.h"
#include "utils/fs.h"
#include <fstream>
#include <iostream>

bool ClaudeCodeAdapter::load() {
    config_path_ = paths::claude_config();
    return load_from(config_path_);
}

bool ClaudeCodeAdapter::load_from(const std::filesystem::path& path) {
    config_path_ = path;

    if (!std::filesystem::exists(path)) {
        config_ = nlohmann::json::object();
        return true;
    }

    try {
        std::ifstream ifs(path);
        config_ = nlohmann::json::parse(ifs);
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "ERROR: invalid JSON in " << path << ": " << e.what() << std::endl;
        return false;
    }
}

bool ClaudeCodeAdapter::switch_model(const ModelConfig& model, const ProviderConfig& provider) {
    // Ensure parent directory exists
    if (!config_path_.parent_path().empty()) {
        fs_util::ensure_dir(config_path_.parent_path());
    }

    // Backup existing config (skip if it doesn't exist yet)
    if (std::filesystem::exists(config_path_)) {
        if (!backup()) return false;
    }

    // Ensure env object exists
    if (!config_.contains("env") || !config_["env"].is_object()) {
        config_["env"] = nlohmann::json::object();
    }

    // Patch env fields
    config_["env"]["ANTHROPIC_AUTH_TOKEN"] = provider.auth_token;
    config_["env"]["ANTHROPIC_BASE_URL"] = provider.base_url;
    config_["env"]["ANTHROPIC_MODEL"] = model.model;

    if (!save_atomic()) return false;
    return true;
}

bool ClaudeCodeAdapter::backup() {
    return backup::create(config_path_);
}

bool ClaudeCodeAdapter::save_atomic() {
    return atomic_write::write_unchecked(config_path_, config_.dump(2));
}
