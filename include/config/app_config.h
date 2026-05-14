#pragma once
#include <string>
#include <optional>
#include <filesystem>
#include "config/model_registry.h"

class AppConfig {
public:
    bool load();
    bool load_from(const std::filesystem::path& path);

    const ModelConfig* get_model(const std::string& name) const;
    const ProviderConfig* get_provider(const std::string& name) const;

    std::string current_model_name() const;
    void set_current_model(const std::string& name);
    bool save();

    bool add_model(const std::string& name, const std::string& provider, const std::string& model);
    bool add_provider(const std::string& name, const std::string& base_url, const std::string& auth_token);
    bool remove_model(const std::string& name);
    bool remove_provider(const std::string& name);

    const std::vector<ModelConfig>& models() const;
    const std::vector<ProviderConfig>& providers() const;

private:
    std::filesystem::path config_path_;
    std::string current_model_;
    std::vector<ModelConfig> models_;
    std::vector<ProviderConfig> providers_;
};
