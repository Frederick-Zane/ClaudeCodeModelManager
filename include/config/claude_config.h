#pragma once
#include <filesystem>
#include <nlohmann/json.hpp>
#include "config/model_registry.h"

class ClaudeCodeAdapter {
public:
    bool load();
    bool load_from(const std::filesystem::path& path);

    bool switch_model(const ModelConfig& model, const ProviderConfig& provider);

    bool backup();
    bool save_atomic();

private:
    std::filesystem::path config_path_;
    nlohmann::json config_;
};
