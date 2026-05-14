#include "config/app_config.h"
#include "utils/paths.h"
#include "utils/atomic_write.h"
#include <toml++/toml.hpp>
#include <iostream>

bool AppConfig::load() {
    config_path_ = paths::config_file();
    return load_from(config_path_);
}

bool AppConfig::load_from(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        std::cerr << "ERROR: config not found: " << path << std::endl;
        return false;
    }

    try {
        auto tbl = toml::parse_file(path.string());

        if (auto cur = tbl["current"]["model"].value<std::string>()) {
            current_model_ = *cur;
        }

        models_.clear();
        if (auto* models_table = tbl["models"].as_table()) {
            for (auto& [key, val] : *models_table) {
                if (auto* t = val.as_table()) {
                    ModelConfig mc;
                    mc.name = std::string(key.str());
                    mc.provider = t->get("provider")->value_or<std::string>("");
                    mc.model = t->get("model")->value_or<std::string>("");
                    models_.push_back(std::move(mc));
                }
            }
        }

        providers_.clear();
        if (auto* provs_table = tbl["providers"].as_table()) {
            for (auto& [key, val] : *provs_table) {
                if (auto* t = val.as_table()) {
                    ProviderConfig pc;
                    pc.name = std::string(key.str());
                    pc.base_url = t->get("base_url")->value_or<std::string>("");
                    pc.auth_token = t->get("auth_token")->value_or<std::string>("");
                    providers_.push_back(std::move(pc));
                }
            }
        }

        return true;
    } catch (const toml::parse_error& e) {
        std::cerr << "ERROR: TOML parse error: " << e << std::endl;
        return false;
    }
}

const ModelConfig* AppConfig::get_model(const std::string& name) const {
    for (const auto& m : models_) {
        if (m.name == name) return &m;
    }
    return nullptr;
}

const ProviderConfig* AppConfig::get_provider(const std::string& name) const {
    for (const auto& p : providers_) {
        if (p.name == name) return &p;
    }
    return nullptr;
}

std::string AppConfig::current_model_name() const {
    return current_model_;
}

void AppConfig::set_current_model(const std::string& name) {
    current_model_ = name;
}

const std::vector<ModelConfig>& AppConfig::models() const {
    return models_;
}

const std::vector<ProviderConfig>& AppConfig::providers() const {
    return providers_;
}

bool AppConfig::save() {
    try {
        auto tbl = toml::parse_file(config_path_.string());
        tbl.insert_or_assign("current", toml::table{{"model", current_model_}});
        std::ostringstream oss;
        oss << tbl;
        return atomic_write::write(config_path_, oss.str());
    } catch (const toml::parse_error& e) {
        std::cerr << "ERROR: TOML parse error on save: " << e << std::endl;
        return false;
    }
}

bool AppConfig::add_model(const std::string& name, const std::string& provider, const std::string& model) {
    try {
        auto tbl = toml::parse_file(config_path_.string());
        auto* models = tbl["models"].as_table();
        if (!models) {
            tbl.insert("models", toml::table{});
            models = tbl["models"].as_table();
        }
        models->insert_or_assign(name, toml::table{
            {"provider", provider},
            {"model", model}
        });
        std::ostringstream oss;
        oss << tbl;
        if (!atomic_write::write(config_path_, oss.str())) return false;
        // Update in-memory state
        models_.push_back({name, provider, model});
        return true;
    } catch (const toml::parse_error& e) {
        std::cerr << "ERROR: TOML parse error: " << e << std::endl;
        return false;
    }
}

bool AppConfig::add_provider(const std::string& name, const std::string& base_url, const std::string& auth_token) {
    try {
        auto tbl = toml::parse_file(config_path_.string());
        auto* providers = tbl["providers"].as_table();
        if (!providers) {
            tbl.insert("providers", toml::table{});
            providers = tbl["providers"].as_table();
        }
        providers->insert_or_assign(name, toml::table{
            {"base_url", base_url},
            {"auth_token", auth_token}
        });
        std::ostringstream oss;
        oss << tbl;
        if (!atomic_write::write(config_path_, oss.str())) return false;
        providers_.push_back({name, base_url, auth_token});
        return true;
    } catch (const toml::parse_error& e) {
        std::cerr << "ERROR: TOML parse error: " << e << std::endl;
        return false;
    }
}

bool AppConfig::remove_model(const std::string& name) {
    try {
        auto tbl = toml::parse_file(config_path_.string());
        auto* models = tbl["models"].as_table();
        if (!models || !models->contains(name)) {
            std::cerr << "ERROR: model not found: " << name << std::endl;
            return false;
        }
        models->erase(name);
        std::ostringstream oss;
        oss << tbl;
        if (!atomic_write::write(config_path_, oss.str())) return false;
        std::erase_if(models_, [&](const ModelConfig& m) { return m.name == name; });
        return true;
    } catch (const toml::parse_error& e) {
        std::cerr << "ERROR: TOML parse error: " << e << std::endl;
        return false;
    }
}

bool AppConfig::remove_provider(const std::string& name) {
    // Check if any model depends on this provider
    for (const auto& m : models_) {
        if (m.provider == name) {
            std::cerr << "ERROR: provider '" << name << "' is used by model '" << m.name << "'" << std::endl;
            return false;
        }
    }
    try {
        auto tbl = toml::parse_file(config_path_.string());
        auto* providers = tbl["providers"].as_table();
        if (!providers || !providers->contains(name)) {
            std::cerr << "ERROR: provider not found: " << name << std::endl;
            return false;
        }
        providers->erase(name);
        std::ostringstream oss;
        oss << tbl;
        if (!atomic_write::write(config_path_, oss.str())) return false;
        std::erase_if(providers_, [&](const ProviderConfig& p) { return p.name == name; });
        return true;
    } catch (const toml::parse_error& e) {
        std::cerr << "ERROR: TOML parse error: " << e << std::endl;
        return false;
    }
}
